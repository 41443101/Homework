#include <iostream>
using namespace std;

int st[10000000];
int top = 0;

void push(int x) {
	st[top++] = x;
}

int pop() {
	return st[--top];
}

int ackermann(int m, int n) {
	top = 0;
	push(m);
	while (top > 0) {
		m = pop();
		if (m == 0) {
			n++;
		}
		else if (n == 0) {
			push(m - 1);
			n = 1;
		}
		else {
			push(m - 1);
			push(m);
			n--;
		}
	}
	return n;
}

int main()
{
	int m, n;
	while (cin >> m >> n) {
		cout << ackermann(m, n) << endl;
	}
	return 0;
}