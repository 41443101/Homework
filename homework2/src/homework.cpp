#include <iostream>
#include <algorithm>   // sort
#include <cmath>       // pow, fabs

using namespace std;

class Polynomial;

class Term // 係數 + 指數 
{
    friend class Polynomial;
    friend ostream& operator<<(ostream&, const Polynomial&);
private:
    float coef;  // 係數
    int exp;     // 指數
};

class Polynomial // 多項式類別
{
public:
    Polynomial();                               // p(x) = 0
    Polynomial(const Polynomial& p);            // 複製建構子
    Polynomial& operator=(const Polynomial& p); // 指定運算子
    ~Polynomial();                              // 解構子

    Polynomial Add(Polynomial poly);   // 加法
    Polynomial Mult(Polynomial poly);  // 乘法
    float Eval(float f);               // 代入 x = f

    friend istream& operator>>(istream& in, Polynomial& p);
    friend ostream& operator<<(ostream& out, const Polynomial& p);

private:
    Term* termArray;  // 非零項陣列(指數由大到小)
    int capacity;     // 陣列容量
    int terms;        // 項數
    void NewTerm(float c, int e);  // 尾端加一項
};

Polynomial::Polynomial() // 建構子:空多項式
{
    capacity = 2;
    terms = 0;
    termArray = new Term[capacity];
}

Polynomial::Polynomial(const Polynomial& p) // 複製建構子:開新陣列再抄資料

{
    capacity = p.capacity;
    terms = p.terms;
    termArray = new Term[capacity];
    for (int i = 0; i < terms; i++)
        termArray[i] = p.termArray[i];
}

Polynomial& Polynomial::operator=(const Polynomial& p)// 指定運算子:先刪舊的,再複製
{
    if (this != &p)
    {
        delete[] termArray;
        capacity = p.capacity;
        terms = p.terms;
        termArray = new Term[capacity];
        for (int i = 0; i < terms; i++)
            termArray[i] = p.termArray[i];
    }
    return *this;
}

Polynomial::~Polynomial()
{
    delete[] termArray;
}

void Polynomial::NewTerm(float c, int e) // 尾端加一項,滿了就擴大一倍
{
    if (terms == capacity)
    {
        capacity *= 2;
        Term* temp = new Term[capacity];
        for (int i = 0; i < terms; i++)
            temp[i] = termArray[i];
        delete[] termArray;
        termArray = temp;
    }
    termArray[terms].coef = c;
    termArray[terms].exp = e;
    terms++;
}

Polynomial Polynomial::Add(Polynomial poly) // 加法:i 走 *this,j 走 poly
{
    Polynomial res;
    int i = 0, j = 0;
    while (i < terms && j < poly.terms)
    {
        int e1 = termArray[i].exp;
        int e2 = poly.termArray[j].exp;
        if (e1 == e2)
        {
            float sum = termArray[i].coef + poly.termArray[j].coef; // 指數一樣,係數相加
            if (sum != 0) res.NewTerm(sum, e1);
            i++;
            j++;
        }
        else if (e1 > e2)
        {
            res.NewTerm(termArray[i].coef, e1); // 先放指數大的
            i++;
        }
        else
        {
            res.NewTerm(poly.termArray[j].coef, e2);
            j++;
        }
    }
    for (; i < terms; i++) // 剩下的直接接上
        res.NewTerm(termArray[i].coef, termArray[i].exp);
    for (; j < poly.terms; j++)
        res.NewTerm(poly.termArray[j].coef, poly.termArray[j].exp);
    return res;
}

Polynomial Polynomial::Mult(Polynomial poly) // 乘法:兩兩相乘  排序  合併同次項

{
    Polynomial res;
    if (terms == 0 || poly.terms == 0) return res;

    int total = terms * poly.terms;   // 乘出來的總項數
    Term* temp = new Term[total];
    int k = 0;
    for (int i = 0; i < terms; i++)
    {
        for (int j = 0; j < poly.terms; j++)
        {
            temp[k].coef = termArray[i].coef * poly.termArray[j].coef;
            temp[k].exp = termArray[i].exp + poly.termArray[j].exp;
            k++;
        }
    }
    sort(temp, temp + total, [](const Term& a, const Term& b)  // 指數由大到小排序
        {
            return a.exp > b.exp;
        });
    int i = 0;
    while (i < total) // 同指數的合併
    {
        int e = temp[i].exp;
        float sum = 0;
        while (i < total && temp[i].exp == e)
        {
            sum += temp[i].coef;
            i++;
        }
        if (sum != 0) res.NewTerm(sum, e);
    }
    delete[] temp;
    return res;
}

float Polynomial::Eval(float f) // 代入 f 計算
{
    float sum = 0;
    for (int i = 0; i < terms; i++)
        sum += termArray[i].coef * pow(f, termArray[i].exp);
    return sum;
}

istream& operator>>(istream& in, Polynomial& p) // 輸入:n c1 e1 c2 e2 ... cn en
{
    int n;
    in >> n;
    Polynomial res;
    for (int i = 0; i < n; i++)
    {
        float c;
        int e;
        in >> c >> e;
        if (c == 0) continue;
        Polynomial one;        // 只有一項的多項式
        one.NewTerm(c, e);
        res = res.Add(one);    // 用 Add 自動排序、合併
    }
    p = res;
    return in;
}

ostream& operator<<(ostream& out, const Polynomial& p) // 輸出:3x^2 - x + 1
{
    if (p.terms == 0)
    {
        out << 0;
        return out;
    }
    for (int i = 0; i < p.terms; i++)
    {
        float c = p.termArray[i].coef;
        int e = p.termArray[i].exp;
        if (i == 0) out << c;
        else out << (c < 0 ? " - " : " + ") << fabs(c);
        if (e > 0) out << "x";
        if (e > 1) out << "^" << e;
    }
    return out;
}

int main()
{
    Polynomial A, B;
    cout << "Input A: ";
    cin >> A;
    cout << "Input B: ";
    cin >> B;
    cout << "A     = " << A << endl;
    cout << "B     = " << B << endl;
    cout << "A + B = " << A.Add(B) << endl;
    cout << "A * B = " << A.Mult(B) << endl;
    cout << "A(2)  = " << A.Eval(2) << endl;
    return 0;
}
