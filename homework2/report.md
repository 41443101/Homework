# 41443101

作業二

# 題目：多項式類別（Polynomial ADT）

## 解題說明

這題要用 C++ 寫一個 `Polynomial` 類別，表示一元多項式，例如 `3x² - x + 1`。類別要能做到：

| 功能 | 說明 |
|------|------|
| `Add` | 兩個多項式相加 |
| `Mult` | 兩個多項式相乘 |
| `Eval(f)` | 把 x = f 代進去算出值 |
| `operator>>` | 輸入多項式 |
| `operator<<` | 輸出多項式 |

輸入格式是先給項數 n，再給 n 組「係數 指數」，也就是 `n c1 e1 c2 e2 ... cn en`。

例如輸入 `3 3 2 -1 1 1 0` 代表 3x² - x + 1。

## 解題策略

### 資料結構：只存非零項的陣列

多項式如果有 x¹⁰⁰⁰ 這種高次項，用「每個次方存一格」的陣列會浪費很多空間。所以我只存**係數不是 0 的項**，每一項用 `Term` 記錄係數 `coef` 和指數 `exp`。

整個多項式用動態陣列 `termArray` 存，而且**固定指數由大到小排好**。這個排序是整份程式的關鍵，加法和輸出都靠它。

類別裡有三個資料：

- **termArray**：存非零項的陣列。
- **capacity**：陣列目前最多放幾項。
- **terms**：目前實際有幾項。

### 動態擴充（NewTerm）

`NewTerm(c, e)` 負責在尾端加一項。如果 `terms == capacity` 就把容量乘 2，開新陣列，把舊資料抄過去，再刪掉舊陣列。每次擴充都加倍。

### 加法：像合併兩個排序好的陣列

因為兩個多項式都已經由大到小排好，用兩個指標 `i`、`j` 一起往後走，每次比指數：

- **指數一樣**：係數相加，如果和不是 0 才放進結果，`i`、`j` 一起加 1。
- **`*this` 的指數比較大**：先把它放進結果，`i` 加 1。
- **`poly` 的指數比較大**：先把它放進結果，`j` 加 1。

有一邊走完以後，另一邊剩下的直接接到後面。

### 乘法：全部乘出來，排序，再合併

1. 兩層迴圈，把每一項都互相乘，係數相乘、指數相加，全部放進暫時陣列 `temp`（共 `terms × poly.terms` 個）。
2. 用 `sort` 把 `temp` 依指數由大到小排。
3. 排好之後，指數相同的項一定黏在一起，掃一遍把同指數的係數加起來，和不是 0 才放進結果。

乘法一定要有合併這一步，因為例如 (x+1)(x−1) 會乘出 `x²`、`−x`、`x`、`−1`，其中 `−x` 和 `x` 是同次項，要加起來變 0 消掉。

### 代入：逐項計算

`Eval(f)` 對每一項算 `coef × f^exp`，全部加起來。

### 輸入：用 Add 自動排序

使用者輸入的項不一定有排好，也可能有重複的指數（例如同時輸入 `2x²` 和 `3x²`）。我的做法是：每讀到一項，就把它做成「只有一項的多項式」，再用 `Add` 加進結果。因為 `Add` 本來就會排序、合併同次項、去掉係數。

### 輸出

由第一項開始印，第一項直接印係數，後面的項依係數正負印 ` + ` 或 ` - `，再印係數的絕對值。指數大於 0 才印 `x`，大於 1 才印 `^指數`。如果多項式是空的就印 `0`。

### 三大函式（複製建構子、指定運算子、解構子）

類別裡有 `new[]`，所以一定要自己寫：

- **複製建構子**：開一個新陣列，把項一個一個抄過來（深層複製），不能只複製指標，否則兩個物件會共用同一塊記憶體。
- **指定運算子**：先檢查是不是自己指定給自己，再 `delete[]` 舊的、開新的、抄資料。
- **解構子**：`delete[] termArray`。

## 程式實作

```cpp
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
    cout << "Input A (n c1 e1 c2 e2 ...): ";
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
```

## 效能分析

設 A 有 m 項、B 有 n 項（指的是非零項的數量）。

| 函式 | 時間複雜度 | 空間複雜度 | 說明 |
|------|-----------|-----------|------|
| `NewTerm` | 平均 O(1) | O(1) | 容量加倍，偶爾擴充一次，平均下來是常數 |
| 複製建構子、`operator=` | O(項數) | O(項數) | 把每一項抄一遍 |
| `Add` | **O(m + n)** | O(m + n) | 兩個指標各走一遍，不會回頭 |
| `Mult` | **O(mn log(mn))** | O(mn) | 乘出 mn 項，排序 O(mn log(mn))，合併 O(mn) |
| `Eval` | **O(m)** | O(1) | 每項算一次 `pow`，把 `pow` 當成常數時間 |
| `operator>>` | **O(n²)** | O(n) | 讀 n 項，第 k 次呼叫 `Add` 要 O(k)，加起來是 O(n²) |
| `operator<<` | O(項數) | O(1) | 每項印一次 |

## 測試與驗證

### 測試案例

| 測試 | A | B | 想驗證什麼 |
|------|---|---|-----------|
| 一 | 3x² − x + 1 | x + 1 | 一般情況 |
| 二 | x + 1 | x − 1 | 乘法時同次項要相消 |
| 三 | 輸入 `1 1`、`2 2`、`3 1`（沒排序、有重複指數） | 4 + 5x³ | 輸入要自動排序並合併 |
| 四 | 0（沒有任何項） | x + 1 | 空多項式 |
| 五 | 5 | 3 | 只有常數項 |
| 六 | 輸入 `2x²` 和 `−2x²` | x | 輸入時就消成 0 |
| 七 | x² + 1 | x² − 1 | 加法相消一項、乘法相消中間項 |
| 八 | 1.5x³ − 2.5x | 2x² | 小數係數、負係數 |

### 測試輸出

測試一：

輸入：

```text
3 3 2 -1 1 1 0
2 1 1 1 0
```

輸出：

```text
A     = 3x^2 - 1x + 1
B     = 1x + 1
A + B = 3x^2 + 2
A * B = 3x^3 + 2x^2 + 1
A(2)  = 11
```

手算驗證：(3x² − x + 1)(x + 1) = 3x³ + 3x² − x² − x + x + 1 = 3x³ + 2x² + 1，A(2) = 12 − 2 + 1 = 11，都正確。

測試二：

輸入：

```text
2 1 1 1 0
2 1 1 -1 0
```

輸出：

```text
A     = 1x + 1
B     = 1x - 1
A + B = 2x
A * B = 1x^2 - 1
A(2)  = 3
```

(x+1)(x−1) 乘出來的 `−x` 和 `+x` 有正確消掉。

測試三：

輸入：

```text
3 1 1 2 2 3 1
2 4 0 5 3
```

輸出：

```text
A     = 2x^2 + 4x
B     = 5x^3 + 4
A + B = 5x^3 + 2x^2 + 4x + 4
A * B = 10x^5 + 20x^4 + 8x^2 + 16x
A(2)  = 16
```

輸入的 `1x` 和 `3x` 被合併成 `4x`，而且 A 輸入時雖然是 x、2x²、3x 的亂序，輸出仍由大到小。

測試四：

輸入：

```text
0
2 1 1 1 0
```

輸出：

```text
A     = 0
B     = 1x + 1
A + B = 1x + 1
A * B = 0
A(2)  = 0
```

測試五：

輸入：

```text
1 5 0
1 3 0
```

輸出：

```text
A     = 5
B     = 3
A + B = 8
A * B = 15
A(2)  = 5
```

測試六：

輸入：

```text
2 2 2 -2 2
1 1 1
```

輸出：

```text
A     = 0
B     = 1x
A + B = 1x
A * B = 0
A(2)  = 0
```

測試七：

輸入：

```text
2 1 2 1 0
2 1 2 -1 0
```

輸出：

```text
A     = 1x^2 + 1
B     = 1x^2 - 1
A + B = 2x^2
A * B = 1x^4 - 1
A(2)  = 5
```

測試八：

輸入：

```text
2 1.5 3 -2.5 1
1 2 2
```

輸出：

```text
A     = 1.5x^3 - 2.5x
B     = 2x^2
A + B = 1.5x^3 + 2x^2 - 2.5x
A * B = 3x^5 - 5x^3
A(2)  = 7
```

手算驗證：A(2) = 1.5×8 − 2.5×2 = 12 − 5 = 7，正確。

### 測試結果總整理

| 測試 | 加法 | 乘法 | 代入 | 結果 |
|------|------|------|------|------|
| 一 | 3x²+2 | 3x³+2x²+1 | 11 | 通過 |
| 二 | 2x | x²−1 | 3 | 通過 |
| 三 | 5x³+2x²+4x+4 | 10x⁵+20x⁴+8x²+16x | 16 | 通過 |
| 四 | x+1 | 0 | 0 | 通過 |
| 五 | 8 | 15 | 5 | 通過 |
| 六 | x | 0 | 0 | 通過 |
| 七 | 2x² | x⁴−1 | 5 | 通過 |
| 八 | 1.5x³+2x²−2.5x | 3x⁵−5x³ | 7 | 通過 |

另外，我用 AddressSanitizer 和 UndefinedBehaviorSanitizer 編譯執行（`-fsanitize=address,undefined`），沒有出現記憶體洩漏、重複釋放或越界，代表自己寫的複製建構子、指定運算子和解構子是正確的。

### 編譯與執行指令

```shell
$ g++ -std=c++17 -o poly poly.cpp
$ ./poly
```

## 效能量測

### 量測方法

用 `<chrono>` 的 `steady_clock` 量時間，每個大小重複多次取平均。測試多項式是 n 個連續指數、係數都是 1 的稠密多項式（這是 `Mult` 合併最多、最壞的情況）。

```cpp
auto t = steady_clock::now();
for (int i = 0; i < rep; i++) f();
double ms = duration<double, milli>(steady_clock::now() - t).count() / rep;
```

編譯條件：`g++ -std=c++17 -O2`。數字會因電腦不同而不一樣，要看的是**成長趨勢**。

### 結果

**`Add`**（A、B 各 n 項）

| n | 時間 (ms) |
|---|----------|
| 1,000 | 0.0050 |
| 2,000 | 0.0080 |
| 4,000 | 0.0160 |
| 8,000 | 0.0440 |
| 16,000 | 0.0642 |
| 32,000 | 0.1169 |

n 變 2 倍，時間大約也變 2 倍，符合 O(m + n)。

**`Mult`**（A、B 各 n 項）

| n | 乘出的項數 | 時間 (ms) | 相對前一列 |
|---|-----------|----------|-----------|
| 100 | 10,000 | 0.286 | – |
| 200 | 40,000 | 1.241 | 約 4.3 倍 |
| 400 | 160,000 | 4.997 | 約 4.0 倍 |
| 800 | 640,000 | 21.64 | 約 4.3 倍 |
| 1,600 | 2,560,000 | 98.93 | 約 4.6 倍 |

n 變 2 倍，時間約變 4 倍多一點，符合 O(n² log n)（4 倍是 n² 的部分，多出的一點來自 log）。

**`Eval`**（n 項）

| n | 時間 (ms) |
|---|----------|
| 1,000 | 0.0170 |
| 10,000 | 0.1471 |
| 100,000 | 1.634 |

n 變 10 倍，時間約變 10 倍，符合 O(n)。

**`operator>>`**（輸入 n 項）

| n | 時間 (ms) | 相對前一列 |
|---|----------|-----------|
| 1,000 | 2.69 | – |
| 2,000 | 8.74 | 約 3.3 倍 |
| 4,000 | 32.88 | 約 3.8 倍 |
| 8,000 | 134.1 | 約 4.1 倍 |
| 16,000 | 524.4 | 約 3.9 倍 |

n 變 2 倍，時間約變 4 倍，符合 O(n²)，和效能分析推論的一樣。

### 量測結論

量測結果和理論分析一致：`Add`、`Eval` 是線性，`Mult` 是平方再乘一點 log，而 `operator>>` 因為用 `Add` 一項一項插入，是平方。如果真的要輸入上萬項，可以改成「先全部讀進來，再用 `sort` 排序合併」，會降到 O(n log n)。

## 心得討論

### 這份作業學到的

**一、深層複製很重要。** 這次的類別裡面有 `new[]` 出來的陣列，所以一定要自己寫複製建構子、指定運算子和解構子。最容易想到的是複製建構子，但 `Add(Polynomial poly)` 是傳值，每次呼叫都會用到複製建構子，沒寫好程式一定會當掉。指定運算子裡的 `if (this != &p)` 也很重要，不然 `a = a` 的時候會先把自己刪掉，再從已經刪掉的資料複製。

**二、維持「由大到小排好」這個規則，後面都變簡單。** 一開始我想過每次要用的時候再排序，後來發現只要讓 `termArray` 永遠保持排好，`Add` 就能像合併兩個排序陣列一樣，一次掃完。輸入的時候用 `Add` 把每一項加進去，這樣輸入、加法、輸出的規則就統一了。

**三、乘法的合併步驟。** 一開始以為乘法只要兩層迴圈乘完就好，後來測 (x+1)(x−1) 才發現會有 `−x` 和 `x` 兩項，一定要合併同次項。所以改成先全部乘出來，排序，再把相同指數的加在一起，這樣也順便處理了係數相消成 0 的情況。

**四、動態陣列加倍。** `NewTerm` 容量不夠時直接加倍，而不是一次加 1。我用效能分析的觀念想過，加 1 的話每加一項都要複製一次，總共是 O(n²)；加倍的話平均下來每項只要 O(1)。
