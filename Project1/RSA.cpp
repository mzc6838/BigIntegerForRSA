
#include <cassert>
#include <sstream>
#include <ctime>
#include <thread>
#include <Windows.h>
#include <iostream>
#include "RSA.h"

/**
* ��������:��ʼ��RSA����������Ϣ
* ��������:len��ʾ�������Ķ�����λ��
*/
void RSA::init(const unsigned len, RSA &output,unsigned int _seed) {
	srand((unsigned)time(NULL));
	// ����������p��q
	
	std::thread t1(&RSA::createPrime, len, 5, std::ref(output.p), (unsigned int)(_seed / 3 * 2));
	std::thread t2(&RSA::createPrime, len, 5, std::ref(output.q), (unsigned int)(_seed / 3));

	t1.join();
	t2.join();

	// �����n
	std::cout << output.p;

	output.n = output.p * output.q;
	// �����n��ŷ������
	output.eul = (output.p - 1)*(output.q - 1);
	// ���üӽ���ָ��e��d
	output.createExponent(output.eul);
}

/**
* ��������:ʹ�ù�Կ���м���
* ��������:m��ʾҪ���ܵ�����
*/
BigIntegerForRsa RSA::encryptByPublic(const BigIntegerForRsa & m) {
	return m.modPow(e, n);
}

/**
* ��������:ʹ��˽Կ���н���
* ��������:c��ʾҪ���ܵ�����
*/
BigIntegerForRsa RSA::decryptByPrivate(const BigIntegerForRsa & c) {
	return c.modPow(d, n);
}
/**
*���ⲿ�趨rsa��������
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
* ��������:����һ������Ϊlen������
* ��������:len���������Ķ����Ƴ���
*/
BigIntegerForRsa RSA::createOddNum(unsigned len) {
	static const char hex_table[] = { '0', '1', '2', '3', '4', '5', '6', '7',
		'8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	len >>= 2;    // ʮ����������,ÿλռ4λ������
	if (len) {
		std::ostringstream oss;
		for (size_t i = 0; i<len - 1; ++i)
			oss << hex_table[rand() % 16];
		oss << hex_table[1];// ���һλΪ����
		return BigIntegerForRsa(oss.str());
	}
	return BigIntegerForRsa("F");
}

/**
* ��������:�ж�һ�����Ƿ�Ϊ����,����������������������㷨,ʧ����Ϊ(1/4)^k
* ��������:num����Ҫ�ж�����,k������Դ���
*/
bool RSA::isPrime(const BigIntegerForRsa & num, const unsigned k) {
	//assert(num != BigIntegerForRsa::ZERO);// ����num�Ƿ�Ϊ0
	if (num == BigIntegerForRsa::ONE)
		return false;    // 1��������
	if (num == BigIntegerForRsa::TWO)
		return true;    // 2������

	BigIntegerForRsa t = num - 1;
	BigIntegerForRsa::bit b(t);// ��������
	if (b.at(0) == 1)    // ��һ֮��Ϊ����,ԭ��Ϊż��
		return false;
	// num-1 = 2^s*d
	size_t s = 0;    // ͳ�ƶ�����ĩβ�м���0
	BigIntegerForRsa d(t);
	for (size_t i = 0; i<b.size(); ++i) {
		if (!b.at(i)) {
			++s;
			d = d.shiftRight(1);// �����d
		}
		else
			break;
	}

	for (size_t i = 0; i<k; ++i) {// ����k��
		BigIntegerForRsa a = createRandomSmaller(num);// ����һ������[1,num-1]֮��������a
		BigIntegerForRsa x = a.modPow(d, num);
		if (x == BigIntegerForRsa::ONE)// ����Ϊ����
			continue;
		bool ok = true;
		// ��������0<=j<s,a^(2^j*d) mod num != -1
		for (size_t j = 0; j<s && ok; ++j) {
			if (x == t)
				ok = false;    // ��һ�����,����Ϊ����
			x = x.multiply(x).mod(num);
		}
		if (ok)    // ȷʵ������,һ��Ϊ����
			return false;
	}
	return true;    // ͨ�����в���,����Ϊ����
}

/**
* ��������:�������һ����valС����
* ��������:val����Ƚϵ��Ǹ���
*/
BigIntegerForRsa RSA::createRandomSmaller(const BigIntegerForRsa & val) {
	BigIntegerForRsa::base_t t = 0;
	do {
		t = rand();
	} while (t == 0);// ������ɷ�0��

	BigIntegerForRsa mod(t);
	BigIntegerForRsa ans = mod%val;    // ��valҪС
	if (ans == BigIntegerForRsa::ZERO)// �������
		ans = val - BigIntegerForRsa::ONE;
	return ans;
}

/**
* ��������:����һ�������Ƴ���Ϊlen�Ĵ�����
* ��������:len����������ĳ���,k�����������Ĵ���
*/
void RSA::createPrime(unsigned len, const unsigned k, BigIntegerForRsa &ans, unsigned int _seed = 55533) {
	srand(_seed);
	//assert(k > 0);
	ans = createOddNum(len);// ������һ������
	while (!isPrime(ans, k)) {// ���Լ��
		ans = ans.add(BigIntegerForRsa::TWO);// ��һ������
	}
	//return true;
}

/**
* ��������:�����ṩ��ŷ�������ɹ�Կ��˽Կָ��
* ��������:eul��ʾ�ṩ��ŷ����
*/
void RSA::createExponent(const BigIntegerForRsa & eul) {
	e = 65537;
	d = e.modInverse(eul);
}