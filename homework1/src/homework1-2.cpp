#include<iostream>
#include <string>
using namespace std;

void pow(string A[], string s[], int i, int n, int t) {    // A[]:集合 s[]:子集 i:目前處理到A[i] n:集合大小 t:子集大小
	if (i == n) 
	{
		cout << "{";
		for (int j = 0; j < t; j++) 
		{
			cout << s[j];
			if (j != t - 1) cout << ",";
		}
		cout << "}" << endl;
		return;
	}
	pow(A, s, i + 1, n, t);        
	s[t] = A[i];                  
	pow(A, s, i + 1, n, t + 1);    
}

int main()
{
	int n;
	cin >> n;
	string* A = new string[n];
	string* s = new string[n];
	for (int i = 0; i < n; i++)
	{
		cin >> A[i];
	}
	cout << "冪集為:" << endl;
	pow(A, s, 0, n, 0);

	delete[] A;
	delete[] s;
	return 0;
}