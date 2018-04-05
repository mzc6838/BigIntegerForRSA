
#include <algorithm>
#include <cctype>
#include "BigIntegerForRsa.h"

// ���±�ʾΪ��̬������ֵ
const BigIntegerForRsa BigIntegerForRsa::ZERO = BigIntegerForRsa(0);
const BigIntegerForRsa BigIntegerForRsa::ONE = BigIntegerForRsa(1);
const BigIntegerForRsa BigIntegerForRsa::TWO = BigIntegerForRsa(2);
const BigIntegerForRsa BigIntegerForRsa::TEN = BigIntegerForRsa(10); 

/**
* ��������:���ݸ����Ĵ���������һ���µĴ�����
* ��������:val��������Ĵ�����
*/
BigIntegerForRsa::BigIntegerForRsa(const BigIntegerForRsa & val) {
	*this = val;
}

/**
* ��������:���ݸ�����ʮ�������ַ������ݹ���һ��������
* ��������:str�������������
*/
BigIntegerForRsa::BigIntegerForRsa(const std::string & str) : is_negative(false) {
	std::string t(str);
	if (t.size() && t.at(0) == '-') {
		if (t.size() > 1)
			is_negative = true;
		t = t.substr(1);
	}
	int cnt = (8 - (t.size() % 8)) % 8;// ���ĳ��Ȳ���8�ı���,����0
	std::string temp;

	for (int i = 0; i<cnt; ++i)
		temp.push_back('0');

	t = temp + t;

	for (size_t i = 0; i<t.size(); i += base_char) {
		base_t sum = 0;
		for (int j = 0; j<base_char; ++j) {    // 8λʮ��������ɴ�������һλ
			char ch = t[i + j];
			int num = hexToNum(ch);
			sum = ((sum << 4) | (num));
		}
		data.push_back(sum);
	}
	reverse(data.begin(), data.end());// ��λ�ں�
	*this = trim();// ȥ����λ��0
}

/**
* ��������:���ݸ�����long_t�������ݹ���һ��������
* ��������:num�������������
*/
BigIntegerForRsa::BigIntegerForRsa(const long_t & num) : is_negative(false) {
	long_t t = num;
	if (t < 0) {
		is_negative = true;
		t = -t;
	}
	do {
		base_t temp = (t&base_num);    // ÿ�ν�ȡ��32λ
		data.push_back(temp);
		t >>= base_int;
	} while (t);
}

/**
* ��������:�������ӷ�����
* ��������:val�������
*/
BigIntegerForRsa BigIntegerForRsa::add(const BigIntegerForRsa & val) {
	BigIntegerForRsa ans(*this);
	if (ans.is_negative == val.is_negative) {// ͬ��
		int len = val.data.size() - ans.data.size();

		while ((len--) > 0)    // ������λ����,��λ��0
			ans.data.push_back(0);

		int carry = 0;    // ��λ
		for (size_t i = 0; i<val.data.size(); ++i) {
			base_t temp = ans.data[i];
			ans.data[i] += val.data[i] + carry;    // �޷��������,����ȡ������
												   // ��λ:һ�������޽�λ������,һ�����н�λ�ų���(����ʮ�������,9+9+1,��9,9+0+0,��9)
			carry = (temp>ans.data[i] ? 1 : (temp>(temp + val.data[i]) ? 1 : 0));
		}

		for (size_t i = val.data.size(); i<ans.data.size() && carry != 0; ++i) {// ���н�λ
			base_t temp = ans.data[i];
			ans.data[i] += carry;
			carry = temp > ans.data[i];
		}

		if (carry)    // ���н�λ
			ans.data.push_back(carry);
	}
	else {    // ���
		BigIntegerForRsa a = abs();
		BigIntegerForRsa b = val.abs();
		int flag = a.compareTo(b);
		// ����ֵ���,����Ϊ0,�����þ���ֵ��ļ�ȥС��,���������ֵ���
		if (flag == -1) {
			ans = b.subtract(a);
			ans.is_negative = val.is_negative;
		}
		else if (flag == 0)
			ans = ZERO;
		else {
			ans = a.subtract(b);
			ans.is_negative = is_negative;
		}
	}
	return ans;
}

/**
* ��������:��������������
* ��������:val�������
*/
BigIntegerForRsa BigIntegerForRsa::subtract(const BigIntegerForRsa & val) {
	BigIntegerForRsa ans(*this);
	BigIntegerForRsa a = abs();
	BigIntegerForRsa b = val.abs();
	if (ans.is_negative == val.is_negative) {// ͬ��
		int flag = a.compareTo(b);
		if (flag == 1) {// a�ľ���ֵ����b�ľ���ֵ,ֱ�Ӽ�
			int borrow = 0;    // ��λ
							   // ������С��
			for (size_t i = 0; i<val.data.size(); ++i) {
				base_t temp = ans.data[i];
				ans.data[i] -= val.data[i] + borrow;
				// ��λ:һ�������޽�λ������,��һ�����н�λ�ų���(����ʮ�������,9-0-0,��9,9-9-1,��9)
				borrow = temp<ans.data[i] ? 1 : (temp - borrow<val.data[i] ? 1 : 0);
			}
			for (size_t i = val.data.size(); i<ans.data.size() && borrow != 0; ++i) {// ���н�λ
				base_t temp = ans.data[i];
				ans.data[i] -= borrow;
				borrow = temp < (base_t)borrow;
			}
			ans = ans.trim();// ȥ����λ�����0
		}
		else if (flag == 0)
			ans = ZERO;
		else {// a�ľ���ֵС��b�ľ���ֵ
			ans = b.subtract(a);
			ans.is_negative = !is_negative;
		}
	}
	else {    // ���
		ans = a.add(b);    // ת��Ϊ�ӷ�
		ans.is_negative = is_negative;
	}
	return ans;
}

/**
* ��������:�������˷�����
* ��������:val�������
*/
BigIntegerForRsa BigIntegerForRsa::multiply(const BigIntegerForRsa & val) const {
	if (equals(ZERO) || val.equals(ZERO))
		return ZERO;
	// ��λ���ٵ���Ϊ����
	const BigIntegerForRsa & big = data.size()>val.data.size() ? (*this) : val;
	const BigIntegerForRsa & small = (&big) == (this) ? val : (*this);

	BigIntegerForRsa ans;
	bit t(small);    // ת��Ϊ�����ƽ�������

	for (int i = t.size() - 1; i >= 0; --i)
		if (t.at(i)) {
			BigIntegerForRsa temp(big);
			temp.is_negative = false;
			temp = temp.shiftLeft(i);    // ��λ����
			ans = ans.add(temp);
		}
	ans.is_negative = !(is_negative == val.is_negative);
	return ans;
}

/**
* ��������:��������������
* ��������:val�������
*/
BigIntegerForRsa BigIntegerForRsa::divide(const BigIntegerForRsa & val) {
	BigIntegerForRsa temp;
	BigIntegerForRsa ans = divideAndRemainder(val, temp);
	return ans;
}

/**
* ��������:������ȡ������
* ��������:val�������
*/
BigIntegerForRsa BigIntegerForRsa::remainder(const BigIntegerForRsa & val) {
	BigIntegerForRsa ans;
	divideAndRemainder(val, ans);
	return ans;
}

/**
* ��������:������ȡģ����(��ͬ��ȡ��,�ú������Ƿ���������)
* ��������:m����ģ��
*/
BigIntegerForRsa BigIntegerForRsa::mod(const BigIntegerForRsa & m) {
	BigIntegerForRsa ans = remainder(m);
	if (ans.is_negative)
		ans = ans.add(m);
	return ans;
}

/**
* ��������:���������������ȡ������,�������ֱ�ӷ���,ȡ������m����
* ��������:val��ʾ����,m��ʾȡ����
*/
BigIntegerForRsa BigIntegerForRsa::divideAndRemainder(const BigIntegerForRsa & val, BigIntegerForRsa & m) {
	BigIntegerForRsa a = abs();
	BigIntegerForRsa b = val.abs();
	int flag = a.compareTo(b);
	if (flag == 0)// ����ֵ���
		return (is_negative == val.is_negative) ? BigIntegerForRsa(1) : BigIntegerForRsa(-1);
	if (flag == -1) {
		m = *this;
		return ZERO;
	}
	BigIntegerForRsa ans;

	bit bit_b(b);
	// λ������
	while (true) {// a�ľ���ֵ����b�ľ���ֵ
		bit bit_a(a);
		int len = bit_a.size() - bit_b.size();
		BigIntegerForRsa temp;
		// �ҵ���λ
		while (len >= 0) {
			temp = b.shiftLeft(len);
			if (temp.compareTo(a) != 1)// �ҵ���������λ��ʹ�õ�ǰ��a���ڵ���b
				break;
			--len;
		}
		if (len < 0)    // ��ǰ��aС��b��
			break;
		base_t num = 0;
		while (temp.compareTo(a) != 1) {
			a = a.subtract(temp);
			++num;    // ͳ�Ƶ�ǰ��a�����ڵ��ڼ�����λ���b
		}
		temp = BigIntegerForRsa(num);
		if (len)
			temp = temp.shiftLeft(len);// ��λ�������ǰ��a��b�ļ���
		ans = ans.add(temp);
	}
	ans.is_negative = !(is_negative == val.is_negative);
	m.data = a.data;
	m.is_negative = is_negative;
	return ans;
}

/**
* ��������:�������ݳ�����
* ��������:exponent����ָ��
*/
BigIntegerForRsa BigIntegerForRsa::pow(const BigIntegerForRsa & exponent) {
	BigIntegerForRsa ans(1);
	bit t(exponent);    // ת��Ϊ������,��������
	for (int i = t.size() - 1; i >= 0; --i) {
		ans = ans.multiply(ans);
		if (t.at(i))
			ans = multiply(ans);// �Ӹ�λ��ʼ,λȨ�ۼ�ЧӦ
	}
	return ans;
}

/**
* ��������:������ģ������
* ��������:exponent����ָ��,m����ģ��
*/
BigIntegerForRsa BigIntegerForRsa::modPow(const BigIntegerForRsa & exponent, const BigIntegerForRsa & m) const {
	BigIntegerForRsa ans(1);
	bit t(exponent);
	for (int i = t.size() - 1; i >= 0; --i) {
		ans = ans.multiply(ans).mod(m);
		if (t.at(i))
			ans = multiply(ans).mod(m);
	}
	return ans;
}

/**
* ��������:��չŷ������㷨��˷���Ԫ
* ��������:m��������Ԫʱ��ģ��
*/
BigIntegerForRsa BigIntegerForRsa::modInverse(const BigIntegerForRsa & m) {
	if (equals(ZERO) || m.equals(ZERO))
		return ZERO;    // ��һ����Ϊ0,�Ͳ����ڳ˷���Ԫ
	BigIntegerForRsa a[3], b[3], t[3];
	// ���½��г��ȱ任
	a[0] = 0; a[1] = 1; a[2] = *this;
	b[0] = 1; b[1] = 0; b[2] = m;

	for (t[2] = a[2].mod(b[2]); !t[2].equals(ZERO); t[2] = a[2].mod(b[2])) {
		BigIntegerForRsa temp = a[2].divide(b[2]);
		for (int i = 0; i<3; ++i) {
			t[i] = a[i].subtract(temp.multiply(b[i]));// ������һ��a[2]-temp*b[2]�ͱ�Ϊ������С��
			a[i] = b[i];
			b[i] = t[i];
		}
	}
	if (b[2].equals(ONE)) {// ���Լ��Ϊ1,���ڳ˷���Ԫ
		if (b[1].is_negative)// ��ԪΪ����
			b[1] = b[1].add(m);// ��Ϊ����,ʹ����m��ʣ�༯��
		return b[1];
	}
	return ZERO;// ���Լ����Ϊ1,�޳˷���Ԫ
}

/**
* ��������:��λ����,����
* ��������:len������λ��λ��
*/
BigIntegerForRsa BigIntegerForRsa::shiftLeft(const unsigned len) {
	int index = len >> base_bit;    // ������ÿһλ��Ҫ�ƶ�����λ
	int shift = len&base_temp;    // ��ʣ�¶���λ
	BigIntegerForRsa ans(*this);

	int inc = (shift == 0) ? index : index + 1;// �ж����λҪ�࿪��������һλ
	for (int i = 0; i < inc; ++i)
		ans.data.push_back(0);    // ��λ��0

	if (index) {
		inc = (shift == 0) ? 1 : 2;// �ж����λҪԤ��һλ
		for (int i = ans.data.size() - inc; i >= index; --i)
			ans.data[i] = ans.data[i - index];
		for (int i = 0; i<index; ++i)
			ans.data[i] = 0;
	}
	if (shift) {
		base_t t = base_num;
		t <<= base_int - shift;    // ���ڽ�ȡ��λ
								   // ����
		base_t temp = 0;
		for (size_t i = 0; i<ans.data.size(); ++i) {
			base_t tmp = ans.data[i];
			ans.data[i] = (tmp << shift) | temp;// ���ƺ���ϴ�������λ�ĸ�λ
			temp = (tmp&t) >> (base_int - shift);// ��ȡ�ô�����λ�ĸ�λ
		}
	}
	ans = ans.trim();
	return ans;
}

/**
* ��������:��λ����,����
* ��������:len������λ��λ��
*/
BigIntegerForRsa BigIntegerForRsa::shiftRight(const unsigned len) {
	bit val(*this);
	if (len >= val.size())// ��ǰ������λ��С�ڵ�����λλ��,����0
		return ZERO;
	int index = len >> base_bit;// ������ÿһλ��Ҫ�ƶ�����λ
	int shift = len&base_temp;// ��ʣ�¶���λ
	BigIntegerForRsa ans(*this);

	if (index) {
		for (int i = 0; i<index; ++i)
			ans.data[i] = ans.data[i + index];
		for (int i = 0; i<index; ++i)
			ans.data.pop_back();    // ��λɾ��
	}
	if (shift) {
		base_t t = base_num;
		t >>= base_int - shift;    // ���ڽ�ȡ��λ
								   // ����
		base_t temp = 0;
		for (int i = ans.data.size() - 1; i >= 0; --i) {
			base_t tmp = ans.data[i];
			ans.data[i] = (tmp >> shift) | temp;// ���ƺ���ϴ�������λ�ĵ�λ
			temp = (tmp&t) << (base_int - shift);// ��ȡ�ô�����λ�ĵ�λ
		}
	}
	ans = ans.trim();
	return ans;
}

/**
* ��������:�������ȽϺ���,-1��ʾ��������ҪС,0��ʾ���,1��ʾ��������Ҫ��
* ��������:val����Ҫ��֮�ȽϵĴ�����
*/
int BigIntegerForRsa::compareTo(const BigIntegerForRsa & val) const {
	if (is_negative != val.is_negative) {// ���Ų�ͬ,������С
		if (is_negative == true)
			return -1;
		return 1;
	}
	int flag = 0;
	if (data.size() < val.data.size())// λ����С
		flag = -1;
	else if (data.size() > val.data.size())// λ���ϴ�
		flag = 1;
	else {    // λ�����,�Ӹ�λ��ʼһһ�Ƚ�
		for (std::vector<base_t>::const_reverse_iterator it = data.rbegin(), ite = val.data.rbegin(); it != data.rend(); ++it, ++ite)
			if ((*it) != (*ite)) {
				flag = (*it)<(*ite) ? -1 : 1;    // ��λС,��С
				break;
			}
	}
	if (is_negative)    // ��Ϊ����,С�ķ�����
		flag = -flag;
	return flag;
}

/**
* ��������:�������Ƿ���Ⱥ���
* ��������:val��ʾҪ��֮�ȽϵĴ�����
*/
bool BigIntegerForRsa::equals(const BigIntegerForRsa & val) const {
	return (is_negative == val.is_negative) && (data == val.data);// ���ź����ݶ�Ҫ���
}

/**
* ��������:��һ��long_t���͵�����ת��Ϊ������������
* ��������:num��ʾ��������
*/
BigIntegerForRsa BigIntegerForRsa::valueOf(const long_t & num) {
	return BigIntegerForRsa(num);
}

/**
* ��������:��������ת��Ϊʮ�������ַ���������
*/
std::string BigIntegerForRsa::toString() const {
	std::string ans;
	base_t t = base_num;
	t <<= base_int - 4;    // ���ڽ�ȡ��4λ
	for (int i = data.size() - 1; i >= 0; --i) {
		base_t temp = data[i];
		for (int j = 0; j<base_char; ++j) {
			base_t num = t&temp;// ÿ�ν�ȡ��4λ
			num >>= base_int - 4;    // ����4λ�Ƶ���4λ
			temp <<= 4;
			if (num < 10)
				ans.push_back((char)('0' + num));
			else
				ans.push_back((char)('A' + num - 10));
		}
	}
	while (ans.size()>0 && ans.at(0) == '0')// ȥ����λ���õ�0
		ans = ans.substr(1);
	if (ans.empty())    // �մ�˵��Ϊ0
		ans.push_back('0');
	if (is_negative)    // Ϊ�������ϸ���
		ans = "-" + ans;
	return ans;
}

/**
* ��������:���ش������ľ���ֵ
*/
BigIntegerForRsa BigIntegerForRsa::abs() const {
	BigIntegerForRsa ans;
	ans.data = data;// ֻ��������,����Ĭ��Ϊ��
	return ans;
}

//��������λ����ֵ
unsigned int BigIntegerForRsa::getData(int pos)
{
	return data[pos];
}

//����data�ĳ���
int BigIntegerForRsa::dataSize()
{
	return data.size();
}

// ������������غ�����Ҫ��Ϊ��ʹ����ʹ��
// ������������ʹ�û�������һ��,��һһ����
BigIntegerForRsa operator + (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	BigIntegerForRsa t(a);
	return t.add(b);
}

BigIntegerForRsa operator - (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	BigIntegerForRsa t(a);
	return t.subtract(b);
}

BigIntegerForRsa operator * (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	BigIntegerForRsa t(a);
	return t.multiply(b);
}

BigIntegerForRsa operator / (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	BigIntegerForRsa t(a);
	return t.divide(b);
}

BigIntegerForRsa operator % (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	BigIntegerForRsa t(a);
	return t.remainder(b);
}

bool operator < (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return a.compareTo(b) == -1;
}

bool operator > (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return b < a;
}

bool operator == (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return a.equals(b);
}

bool operator <= (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return !(a > b);
}

bool operator >= (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return !(a < b);
}

bool operator != (const BigIntegerForRsa & a, const BigIntegerForRsa & b) {
	return !(a == b);
}

BigIntegerForRsa operator + (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a + BigIntegerForRsa(b);
}

BigIntegerForRsa operator - (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a - BigIntegerForRsa(b);
}

BigIntegerForRsa operator * (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a*BigIntegerForRsa(b);
}

BigIntegerForRsa operator / (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a / BigIntegerForRsa(b);
}

BigIntegerForRsa operator % (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a%BigIntegerForRsa(b);
}

bool operator < (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a < BigIntegerForRsa(b);
}

bool operator > (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a > BigIntegerForRsa(b);
}

bool operator == (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a == BigIntegerForRsa(b);
}

bool operator <= (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a <= BigIntegerForRsa(b);
}

bool operator >= (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a >= BigIntegerForRsa(b);
}

bool operator != (const BigIntegerForRsa & a, const BigIntegerForRsa::long_t & b) {
	return a != BigIntegerForRsa(b);
}

std::ostream & operator << (std::ostream & out, const BigIntegerForRsa & val) {
	out << val.toString();
	return out;
}

/**
* ��������:�����ô�������һ������,ȥ������λ���õ�0�󲢷���
*/
BigIntegerForRsa BigIntegerForRsa::trim() {
	size_t cnt = 0;
	// ����λΪ0��Ԫ�ص�����
	for (std::vector<base_t>::const_reverse_iterator it = data.rbegin(); it != data.rend(); ++it) {
		if ((*it) == 0)
			++cnt;
		else
			break;
	}
	if (cnt == data.size())    // ֻ������������
		--cnt;
	BigIntegerForRsa ans(*this);
	for (size_t i = 0; i<cnt; ++i)
		ans.data.pop_back();
	return ans;
}

/**
* ��������:���ݸ������ַ�ȷ��������Ӧ��ʮ������
* ��������:ch����������ַ�
*/
int BigIntegerForRsa::hexToNum(char ch) {
	int ans = 0;
	if (isdigit(ch))
		ans = ch - '0';
	else if (islower(ch))
		ans = ch - 'a' + 10;
	else
		ans = ch - 'A' + 10;
	return ans;
}

/**
* ��������:���ݸ����Ĵ�������ʼ��
* ��������:val��������Ĵ�����
*/
BigIntegerForRsa::bit::bit(const BigIntegerForRsa & val) {
	bit_vector = val.data;
	base_t temp = bit_vector[bit_vector.size() - 1];// ���������λ
	length = bit_vector.size() << base_bit;    // ������һλռ������32λ
	base_t t = 1 << (base_int - 1);    // ���ڽ�ȡһ�����Ķ��������λ

	if (temp == 0)    // ���������λΪ0,��ȥ32
		length -= base_int;
	else {
		while (!(temp & t)) {// �Ӹ�λ��ʼ���������Ķ�����λ,Ϊ0���ȼ�һ
			--length;
			t >>= 1;    // ����һλ��ʾ�����һλ
		}
	}
}

/**
* ��������:���������ĵ�idλ������λ�Ƿ�Ϊ1
* ��������:id�����idλ
*/
bool BigIntegerForRsa::bit::at(size_t id) {
	size_t index = id >> base_bit;// ȷ�����ڴ������ڼ�λ
	size_t shift = id&base_temp;// ȷ�����ڴ�������һλ�Ķ����Ƶڼ�λ
	base_t t = bit_vector[index];
	return (t & (1 << shift));
}