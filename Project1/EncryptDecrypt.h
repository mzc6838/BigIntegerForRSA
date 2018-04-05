#pragma once

#ifndef __ENCRYPTDECRYPT_H__
#define __ENCRYPTDECRYPT_H__

#include <string>
#include "RSA.h"
class EncryptDecrypt {
public:
	EncryptDecrypt() {}
	~EncryptDecrypt() {}

	void menu();    // �˵���ʾ
	bool encrypt();    // ����
	bool decrypt();    // ����
	void print();    // ��ӡRSA�����Ϣ
	void reset();    // ����RSA�����Ϣ
protected:
	void load(int);    // ���ݸ���λ������RSA����
	bool islegal(const std::string &);// �ж������ַ����Ƿ�Ϸ�
private:
	RSA rsa;
};

#endif // __ENCRYPTDECRYPT_H__