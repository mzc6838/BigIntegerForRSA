
#include <cassert>
#include <sstream>
#include <ctime>
#include <thread>
#include <Windows.h>
#include <iostream>
#include "RSA.h"

/**
* 函数功能:初始化RSA对象的相关信息
* 参数含义:len表示大素数的二进制位数
*/
void RSA::init(const unsigned len, RSA &output,unsigned int _seed) {
	srand((unsigned)time(NULL));
	// 产生大素数p和q
	
	std::thread t1(&RSA::createPrime, len, 5, std::ref(output.p), (unsigned int)(_seed / 3 * 2));
	std::thread t2(&RSA::createPrime, len, 5, std::ref(output.q), (unsigned int)(_seed / 3));

	t1.join();
	t2.join();

	// 计算出n
	std::cout << output.p;

	output.n = output.p * output.q;
	// 计算出n的欧拉函数
	output.eul = (output.p - 1)*(output.q - 1);
	// 设置加解密指数e和d
	output.createExponent(output.eul);
}

/**
* 函数功能:使用公钥进行加密
* 参数含义:m表示要加密的明文
*/
BigIntegerForRsa RSA::encryptByPublic(const BigIntegerForRsa & m) {
	return m.modPow(e, n);
}

/**
* 函数功能:使用私钥进行解密
* 参数含义:c表示要解密的密文
*/
BigIntegerForRsa RSA::decryptByPrivate(const BigIntegerForRsa & c) {
	return c.modPow(d, n);
}
/**
*由外部设定rsa三个参数
*/
void RSA::setd(const BigIntegerForRsa &d)
{
	this->d = d;
}

void RSA::setn(const BigIntegerForRsa &n)
{
	this->n = n;
}

void RSA::sete(const BigIntegerForRsa &e)
{
	this->e = e;
}

BigIntegerForRsa RSA::getd() const
{
	return this->d;
}

/**
* 函数功能:生成一个长度为len的奇数
* 参数含义:len代表奇数的二进制长度
*/
BigIntegerForRsa RSA::createOddNum(unsigned len) {
	static const char hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	len >>= 2;    // 十六进制数据,每位占4位二进制
	if (len) {
		std::ostringstream oss;
		for (size_t i = 0; i<len - 1; ++i)
			oss << hex_table[rand() % 16];
		oss << hex_table[1];// 最后一位为奇数
		return BigIntegerForRsa(oss.str());
	}
	return BigIntegerForRsa("F");
}

/**
* 函数功能:判断一个数是否为素数,采用米勒拉宾大素数检测算法,失误率为(1/4)^k
* 参数含义:num代表要判定的数,k代表测试次数
*/
bool RSA::isPrime(const BigIntegerForRsa & num, const unsigned k) {
	//assert(num != BigIntegerForRsa::ZERO);// 测试num是否为0
	if (num == BigIntegerForRsa::ONE)
		return false;    // 1不是素数
	if (num == BigIntegerForRsa::TWO)
		return true;    // 2是素数

	BigIntegerForRsa t = num - 1;
	BigIntegerForRsa::bit b(t);// 二进制数
	if (b.at(0) == 1)    // 减一之后为奇数,原数为偶数
		return false;
	// num-1 = 2^s*d
	size_t s = 0;    // 统计二进制末尾有几个0
	BigIntegerForRsa d(t);
	for (size_t i = 0; i<b.size(); ++i) {
		if (!b.at(i)) {
			++s;
			d = d.shiftRight(1);// 计算出d
		}
		else
			break;
	}

	for (size_t i = 0; i<k; ++i) {// 测试k次
		BigIntegerForRsa a = createRandomSmaller(num);// 生成一个介于[1,num-1]之间的随机数a
		BigIntegerForRsa x = a.modPow(d, num);
		if (x == BigIntegerForRsa::ONE)// 可能为素数
			continue;
		bool ok = true;
		// 测试所有0<=j<s,a^(2^j*d) mod num != -1
		for (size_t j = 0; j<s && ok; ++j) {
			if (x == t)
				ok = false;    // 有一个相等,可能为素数
			x = x.multiply(x).mod(num);
		}
		if (ok)    // 确实都不等,一定为合数
			return false;
	}
	return true;    // 通过所有测试,可能为素数
}

/**
* 函数功能:随机生成一个比val小的数
* 参数含义:val代表比较的那个数
*/
BigIntegerForRsa RSA::createRandomSmaller(const BigIntegerForRsa & val) {
	BigIntegerForRsa::base_t t = 0;
	do {
		t = rand();
	} while (t == 0);// 随机生成非0数

	BigIntegerForRsa mod(t);
	BigIntegerForRsa ans = mod%val;    // 比val要小
	if (ans == BigIntegerForRsa::ZERO)// 必须非零
		ans = val - BigIntegerForRsa::ONE;
	return ans;
}

/**
* 函数功能:生成一个二进制长度为len的大素数
* 参数含义:len代表大素数的长度,k代表素数检测的次数
*/
void RSA::createPrime(unsigned len, const unsigned k, BigIntegerForRsa &ans, unsigned int _seed = 55533) {
	srand(_seed);
	//assert(k > 0);
	ans = createOddNum(len);// 先生成一个奇数
	while (!isPrime(ans, k)) {// 素性检测
		ans = ans.add(BigIntegerForRsa::TWO);// 下一个奇数
	}
	//return true;
}

/**
* 函数功能:根据提供的欧拉数生成公钥、私钥指数
* 参数含义:eul表示提供的欧拉数
*/
void RSA::createExponent(const BigIntegerForRsa & eul) {
	e = 65537;
	d = e.modInverse(eul);
}