#ifndef __BIGINTEGER_H__
#define __BIGINTEGER_H__

#include <string>
#include <vector>
#include <iostream>

class BigIntegerForRsa {
public:
	typedef long long long_t;
	typedef unsigned base_t;
	BigIntegerForRsa() : is_negative(false) { data.push_back(0); } // 默认为0
	BigIntegerForRsa(const BigIntegerForRsa &);                    // 利用给定的大整数初始化
	BigIntegerForRsa(const std::string &);                         // 利用给定的十六进制字符串初始化
	BigIntegerForRsa(const long_t &);                              // 利用给定的long_t类型数据初始化
	~BigIntegerForRsa() {}

	BigIntegerForRsa add(const BigIntegerForRsa &);        // 大整数加法
	BigIntegerForRsa subtract(const BigIntegerForRsa &);// 大整数减法
	BigIntegerForRsa multiply(const BigIntegerForRsa &) const;// 大整数乘法
	BigIntegerForRsa divide(const BigIntegerForRsa &);    // 大整数整除
	BigIntegerForRsa remainder(const BigIntegerForRsa &);    // 大整数取余
	BigIntegerForRsa mod(const BigIntegerForRsa &);        // 大整数取模
	BigIntegerForRsa divideAndRemainder(const BigIntegerForRsa &, BigIntegerForRsa &);// 大整数整除和取余
	BigIntegerForRsa pow(const BigIntegerForRsa &);        // 大整数幂乘
	BigIntegerForRsa modPow(const BigIntegerForRsa &, const BigIntegerForRsa &) const;// 大整数幂模运算
	BigIntegerForRsa modInverse(const BigIntegerForRsa &);// 用扩展欧几里得算法求乘法逆元

	BigIntegerForRsa shiftLeft(const unsigned);    // 移位运算,左移
	BigIntegerForRsa shiftRight(const unsigned);    // 移位运算,右移

	int compareTo(const BigIntegerForRsa &) const;// 比较运算
	bool equals(const BigIntegerForRsa &) const;// 判断是否等于给定数
	static BigIntegerForRsa valueOf(const long_t &);// 将给定数转换为大整数并返回
	std::string toString() const;    // 将大整数转换为十六进制字符串
	BigIntegerForRsa abs() const;        // 求大整数的绝对值

	base_t getData(int); //得到data中任意一位的值
	int dataSize();//返回data的长度

protected:
	// 以下运算符重载函数主要用于像基本类型一样使用大整数类型
	friend BigIntegerForRsa operator + (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend BigIntegerForRsa operator - (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend BigIntegerForRsa operator * (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend BigIntegerForRsa operator / (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend BigIntegerForRsa operator % (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator < (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator > (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator == (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator <= (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator >= (const BigIntegerForRsa &, const BigIntegerForRsa &);
	friend bool operator != (const BigIntegerForRsa &, const BigIntegerForRsa &);

	// 重载版本,使其能用于long_t类型
	friend BigIntegerForRsa operator + (const BigIntegerForRsa &, const long_t &);
	friend BigIntegerForRsa operator - (const BigIntegerForRsa &, const long_t &);
	friend BigIntegerForRsa operator * (const BigIntegerForRsa &, const long_t &);
	friend BigIntegerForRsa operator / (const BigIntegerForRsa &, const long_t &);
	friend BigIntegerForRsa operator % (const BigIntegerForRsa &, const long_t &);
	friend bool operator < (const BigIntegerForRsa &, const long_t &);
	friend bool operator > (const BigIntegerForRsa &, const long_t &);
	friend bool operator == (const BigIntegerForRsa &, const long_t &);
	friend bool operator <= (const BigIntegerForRsa &, const long_t &);
	friend bool operator >= (const BigIntegerForRsa &, const long_t &);
	friend bool operator != (const BigIntegerForRsa &, const long_t &);

	friend std::ostream & operator << (std::ostream &, const BigIntegerForRsa &);
	BigIntegerForRsa operator = (const std::string & str) { return (*this) = BigIntegerForRsa(str); }
	BigIntegerForRsa operator = (const long_t & num) { return (*this) = BigIntegerForRsa(num); }
private:
	BigIntegerForRsa trim();    // 去掉高位无用的0
	int hexToNum(char);    // 十六进制字符转换为十进制数
public:
	static const int base_bit = 5;    // 2^5=32,大整数每位存储的二进制位数
	static const int base_char = 8;    // 组成大整数的一位需要的十六进制位数
	static const int base_int = 32;    // 大整数一位对应的二进制位数
	static const int base_num = 0xffffffff;// 截取低位的辅助
	static const int base_temp = 0x1f;    // 截取模32的余数的辅助
	static const BigIntegerForRsa ZERO;    // 大整数常量0
	static const BigIntegerForRsa ONE;    // 大整数常量1
	static const BigIntegerForRsa TWO;    // 大整数常量2
	static const BigIntegerForRsa TEN;    // 大整数常量10
	
private:
	bool is_negative;                       // 是否为负数
		std::vector<base_t> data;           // 按位数据存储,高位在后
	class bit {                             // 便于大整数运算的二进制处理类
	public:
		bit(const BigIntegerForRsa &);      // 根据大整数初始化

		size_t size() { return length; }    // 返回大整数对应的二进制位数
		bool at(size_t);					// 返回第i位二进制是否为1
	private:
		std::vector<base_t> bit_vector;     // 二进制数据存储,每一个元素对应32位二进制
		size_t length;					    // 二进制的总位数
	};
	friend class RSA;    // RSA为其友元类
};

#endif // __BIGINTEGER_H__