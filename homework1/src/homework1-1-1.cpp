#include <iostream>

using namespace std;
int a(int m, int n) {
	if (m == 0)return n + 1;
	else if (m > 0 && n == 0)return a(m - 1, 1);
	else if (m > 0 && n > 0)return a(m - 1, a(m, n - 1));
}
int main()
{
	int m, n;
	while (cin >> m >> n) {
		cout << a(m, n) << endl;
	}
	return 0;
}