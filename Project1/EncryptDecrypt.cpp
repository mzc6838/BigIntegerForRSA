
#include <iostream>
#include <ctime>
#include "EncryptDecrypt.h"

/**
* ��������:�˵���ʾ
*/
void EncryptDecrypt::menu() {
	std::cout << "**********Welcome to use RSA encoder**********" << std::endl;
	std::cout << "               e: encrypt ����               " << std::endl;
	std::cout << "               d: decrypt ����               " << std::endl;
	std::cout << "               p: print   ��ʾ               " << std::endl;
	std::cout << "               r: reset   ����               " << std::endl;
	std::cout << "               q: quit    �˳�               " << std::endl;
	std::cout << "input your choice:" << std::endl;
}

/**
* ��������:��������
*/
bool EncryptDecrypt::encrypt() {
	std::string str;
	std::cout << "����16��������:" << std::endl;
	std::cout << ">";
	std::cin >> str;// ��������
	if (!std::cin || !islegal(str))
		return false;
	BigIntegerForRsa m(str);
	clock_t start = clock();
	BigIntegerForRsa c = rsa.encryptByPublic(m);
	clock_t finish = clock();

	std::cout << std::fixed;
	std::cout.precision(3);
	std::cout << "��ʱ: " << (double)(finish - start) / CLOCKS_PER_SEC << "s." << std::endl;
	std::cout << "����: " << m << std::endl;
	std::cout << "����: " << c << std::endl;
	return true;
}

/**
* ��������:��������
*/
bool EncryptDecrypt::decrypt() {
	std::string str;
	std::cout << "����16��������:" << std::endl;
	std::cout << ">";
	std::cin >> str;// ��������
	if (!std::cin || !islegal(str))
		return false;
	BigIntegerForRsa c(str);
	clock_t start = clock();
	BigIntegerForRsa m = rsa.decryptByPrivate(c);
	clock_t finish = clock();

	std::cout << std::fixed;
	std::cout.precision(3);
	std::cout << "��ʱ: " << (double)(finish - start) / CLOCKS_PER_SEC << "s." << std::endl;
	std::cout << "����: " << c << std::endl;
	std::cout << "����: " << m << std::endl;
	return true;
}

/**
* ��������:���RSA�����Ϣ
*/
void EncryptDecrypt::print() {
	std::cout << rsa << std::endl;
}

/**
* ��������:����RSA�����Ϣ
*/
void EncryptDecrypt::reset() {
	std::cout << "������Կ����: ";
	int len;
	std::cin >> len;
	load(len >> 1);
}

/**
* ��������:���ݸ���λ��len����rsa
*/
void EncryptDecrypt::load(int len) {
	std::cout << "��ʼ��..." << std::endl;
	clock_t start = clock();
	rsa.init(len, rsa, 7355608);    // ��ʼ��
	clock_t finish = clock();
	std::cout << "��ʼ�����." << std::endl;
	std::cout << std::fixed;
	std::cout.precision(3);
	std::cout << "��ʱ: " << (double)(finish - start) / CLOCKS_PER_SEC << "s." << std::endl;
}

/**
* ��������:�ж������ַ���str�Ƿ�Ϸ�
* ��������:str����������ַ���
*/
bool EncryptDecrypt::islegal(const std::string & str) {
	for (std::string::const_iterator it = str.begin(); it != str.end(); ++it) {
		if (!isalnum(*it))    // ������ĸ��������
			return false;
		if (isalpha(*it)) {
			char ch = tolower(*it);
			if (ch > 'f')    // ����ʮ�������ַ�'f'
				return false;
		}
	}
	return true;
}