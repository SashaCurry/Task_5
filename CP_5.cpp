#include "iostream"
#include "cmath"
#include "vector"
#include "string"
#include "set"
#include "map"
#include "boost/multiprecision/cpp_int.hpp"

using namespace std;
using namespace boost::multiprecision;

cpp_int powMy(cpp_int x, cpp_int y) {
	cpp_int res = 1;
	for (int i = 0; i < y; i++)
		res *= x;
	return res;
}


vector <cpp_int> deg2(cpp_int el, cpp_int n) {//Раскладываем число на степени двойки
	vector <cpp_int> res;
	while (n != 0) {
		if (n / el == 1) {
			res.push_back(el);
			n -= el;
			el = 1;
		}
		else
			el *= 2;
	}
	return res;
}


cpp_int multMod(cpp_int n, cpp_int mod, vector <pair <cpp_int, cpp_int>> lst) {//Умножаем число по модулю
	if (lst.size() == 1) {
		cpp_int res = 1;
		for (int i = 0; i < lst[0].second; i++)
			res = res * lst[0].first % mod;
		return res;
	}
	else if (lst[0].second == 1) {
		cpp_int el = lst[0].first;
		lst.erase(lst.begin());
		return (el * multMod(n, mod, lst)) % mod;
	}
	else {
		for (int i = 0; i < lst.size(); i++)
			if (lst[i].second > 1) {
				lst[i].first = (lst[i].first * lst[i].first) % mod;
				lst[i].second /= 2;
			}
		return multMod(n, mod, lst);
	}
}


cpp_int powClosed(cpp_int x, cpp_int y, cpp_int mod) {//Возводим число в степени по модулю
	if (y == 0)
		return 1;

	vector <cpp_int> lst = deg2(1, y);
	vector <pair <cpp_int, cpp_int>> xDegs;
	for (int i = 0; i < lst.size(); i++)
		xDegs.push_back(make_pair(x, lst[i]));

	cpp_int res = multMod(x, mod, xDegs);
	return res;
}


cpp_int decForm(string x) {
	cpp_int res = 0, deg = 1;
	if (x.back() == '1')
		res += 1;
	for (int i = 1; i < x.length(); i++) {
		deg = deg * 2;
		if (x[x.length() - i - 1] == '1')
			res += deg;
	}
	return res;
}


cpp_int usualEuclid(cpp_int a, cpp_int b) {
	if (a < b)
		swap(a, b);
	if (a < 0 || b < 0)
		throw string{ "Выполнение невозможно: a < 0 или b < 0" };
	else if (b == 0)
		return a;

	cpp_int r = a % b;
	return usualEuclid(b, r);
}


pair <cpp_int, cpp_int> advancedEuclid(cpp_int a, cpp_int b) {
	if (a < 0 || b < 0)
		throw string{ "Выполнение невозможно: a < 0 или b < 0" };

	cpp_int q, aPrev = a, aCur = b, aNext = -1;
	cpp_int xPrev = 1, xCur = 0, xNext;
	cpp_int yPrev = 0, yCur = 1, yNext;
	while (aNext != 0) {
		q = aPrev / aCur;
		aNext = aPrev % aCur;
		aPrev = aCur; aCur = aNext;

		xNext = xPrev - (xCur * q);
		xPrev = xCur; xCur = xNext;

		yNext = yPrev - (yCur * q);
		yPrev = yCur; yCur = yNext;
	}

	return make_pair(xPrev, yPrev);
}


cpp_int funEuler(cpp_int n) {
	cpp_int res = 1;
	for (int i = 2; i < n; i++)
		if (usualEuclid(n, i) == 1)
			res++;
	return res;
}


bool miller_rabin(cpp_int n, int k = 10) {
	if (n == 0 || n == 1)
		return false;

	cpp_int d = n - 1;
	cpp_int s = 0;
	while (d % 2 == 0) {
		s++;
		d = d / 2;
	}

	cpp_int nDec = n - 1;
	for (int i = 0; i < k; i++) {
		cpp_int a = rand() % nDec;
		if (a == 0 || a == 1)
			a = a + 2;

		cpp_int x = powClosed(a, d, n);
		if (x == 1 || x == nDec)
			continue;

		bool flag = false;
		for (int j = 0; j < s; j++) {
			x = (x * x) % n;
			if (x == nDec) {
				flag = true;
				break;
			}
		}
		if (!flag)
			return false;
	}

	return true;
}


cpp_int generateSimpleNum() {
	cpp_int q = rand() % 1000;
	while (funEuler(q) != q - 1)
		q++;

	cpp_int s, n = 2, nDec;
	while (!miller_rabin(n)) {
		string sBin = "";
		int sBinSize = 32 + rand() % 32;
		for (int i = 0; i < sBinSize; i++)
			sBin = sBin + to_string(rand() % 2);
		s = decForm(sBin);

		n = (q * s) + 1;
		nDec = n - 1;
	}

	return n;
}


void guillouQuisquater(string J, string M) {
	hash <string> hashStr;
	cpp_int hashM(hashStr(M));
	cout << "\nХэш подписываемого сообщения M: " << hashM;
	cpp_int hashJ(hashStr(J));
	cout << "\nХэш атрибутов J: " << hashJ;

	cpp_int p = generateSimpleNum(), q = generateSimpleNum();
	cpp_int n = p * q;
	cpp_int phiN = (p - 1) * (q - 1);

	cpp_int v = generateSimpleNum();
	cout << "\n\nОткрытый ключ {n, v} = {" << n << ", " << v << "}";

	cpp_int vRev = advancedEuclid(v, phiN).first;
	if (vRev < 0)
		vRev += phiN;
	cpp_int B = powClosed(advancedEuclid(hashJ, n).first, vRev, n);
	if (B < 0)
		B += n;
	cout << "\nЗакрытый ключ B = " << B;
	cout << "\nJ * B^v (mod n) = " << hashJ * powClosed(B, v, n) % n;

	cpp_int r = abs(rand() * rand() * rand()) % n;
	cpp_int T = powClosed(r, v, n);
	cout << "\n\nАлиса выбирает случайное r = " << r << " и вычисляет T = r^v (mod n) = " << T;

	hash <cpp_int> hashCpp_int;
	cpp_int d = hashCpp_int(hashM * T) % v;
	cout << "\nАлиса вычисляет d = H(M, T) = " << d;

	cpp_int D = r * powClosed(B, d, n) % n;
	cout << "\nАлиса вычисляет D = r * B^d (mod n) = " << D;

	cout << "\n\nАлиса создала подпись {M, d, D, J} = {" << M << ", " << d << ", " << D << ", " << J << "}";

	cpp_int T_ = powClosed(D, v, n) * powClosed(hashJ, d, n) % n;
	cpp_int d_ = hashCpp_int(hashM * T_) % v;
	cout << "\nБоб вычисляет T' = D^v * J^d (mod n) = " << T_ << "\nБоб вычисляет d' = H(M, T') = " << d_;
	cout << ". Если d = d', то подпись Алисы действительна";
}


int main() {
	srand(time(0));
	setlocale(LC_ALL, "ru");
	cout << "\tСхема подписи Гиллу-Кискате \nВведите атрибуты пользователя, который хочет подписать сообщение: ";
	string J;
	getline(cin, J);
	cout << "Введите сообщение: ";
	string M;
	getline(cin, M);

	guillouQuisquater(J, M);
	cout << endl;
	return 0;
}