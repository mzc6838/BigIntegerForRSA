

#include <iostream>
#include "EncryptDecrypt.h"

//int main() {
//	EncryptDecrypt encrypt_decrypt;
//	encrypt_decrypt.reset();// 设置密钥长度
//
//	char ch;
//	std::string str;
//	bool ok = true;
//
//	do {
//		encrypt_decrypt.menu();// 菜单显示
//		std::cout << ">";
//		std::cin >> str;
//		if (str.empty()) {
//			std::cout << "输入错误!请重新输入!" << std::endl;
//			continue;
//		}
//		ch = str.at(0);
//		switch (ch) {
//		case 'e':
//		case 'E':
//			if (!encrypt_decrypt.encrypt())
//				std::cout << "加密失败,请重试!" << std::endl;
//			break;
//		case 'd':
//		case 'D':
//			if (!encrypt_decrypt.decrypt())
//				std::cout << "解密失败,请重试!" << std::endl;
//			break;
//		case 'p':
//		case 'P':
//			encrypt_decrypt.print();// 打印相关信息
//			break;
//		case 'r':
//		case 'R':
//			encrypt_decrypt.reset();// 重新设置密钥长度
//			break;
//		case 'q':
//		case 'Q':
//			ok = false;    // 退出
//			break;
//		default:
//			break;
//		}
//	} while (ok);
//	return 0;
//}

int main()
{
	BigIntegerForRsa e(65537), d(std::string("‭1FFE831387CB5001‬")), n(std::string("‭2A221C4529F9A6B1‬"));
	BigIntegerForRsa input_1(std::string("68")), input_2(std::string("20"));

	BigIntegerForRsa out_1, out_2;

	std::cout << (out_1 = input_1.modPow(e, n)) << std::endl << (out_2 = input_2.modPow(e, n)) << std::endl << std::endl;

	std::cout << out_1.modPow(d, n) << std::endl << out_2.modPow(d, n) << std::endl;
	
	

	system("pause");
}