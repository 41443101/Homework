# 41443101

作業二

# 題目：多項式類別（Polynomial）

## 解題說明

這題要用 C++ 寫一個 `Polynomial` 類別，表示一元多項式，例如 `3x² - x + 1`。類別要能做到：

| 功能 | 說明 |
|------|------|
| `Add` | 兩個多項式相加 |
| `Mult` | 兩個多項式相乘 |
| `Eval(f)` | 把 x = f 代進去算出值 |
| `operator>>` | 輸入多項式 |
| `operator<<` | 輸出多項式 |

輸入格式是先給項數 n，再給 n 組「係數 指數」。輸入完 A 和 B 之後，再輸入要代入的 x 值。

例如輸入 `3 3 2 -1 1 1 0` 代表 3x² - x + 1。

## 解題策略

### 資料結構：只存非零項的陣列

多項式如果有 x¹⁰⁰⁰ 這種高次項，用「每個次方存一格」的陣列會浪費很多空間。所以我只存**係數不是 0 的項**，每一項用 `Term` 記錄係數 `coef` 和指數 `exp`。

整個多項式用動態陣列 `termArray` 存，而且**固定指數由大到小排好**。這個排序是整份程式的重點，加法和輸出都靠它。

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

`Eval(f)` 對每一項算 `coef × f^exp`，全部加起來。主程式會讀入使用者輸入的 x，再呼叫 `A.Eval(x)`。

### 輸入：用 Add 自動排序

使用者輸入的項不一定有排好，也可能有重複的指數（例如同時輸入 `2x²` 和 `3x²`）。我的做法是：每讀到一項，就把它做成「只有一項的多項式」，再用 `Add` 加進結果。因為 `Add` 本來就會排序、合併同次項、去掉係數 0。

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
    float x;
    cout << "Input A: ";
    cin >> A;
    cout << "Input B: ";
    cin >> B;
    cout << "Input x: ";
    cin >> x;
    cout << "A     = " << A << endl;
    cout << "B     = " << B << endl;
    cout << "A + B = " << A.Add(B) << endl;
    cout << "A * B = " << A.Mult(B) << endl;
    cout << "A(" << x << ")  = " << A.Eval(x) << endl;
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

| 測試 | A | B | x | 想驗證什麼 |
|------|---|---|---|-----------|
| 一 | 3x² − x + 1 | x + 1 | 2 | 一般情況 |
| 二 | x + 1 | x − 1 | 3 | 乘法時同次項要相消 |
| 三 | 輸入 `1 1`、`2 2`、`3 1`（沒排序、有重複指數） | 4 + 5x³ | 2 | 輸入要自動排序並合併 |
| 四 | 0（沒有任何項） | x + 1 | 5 | 空多項式 |
| 五 | 5 | 3 | 2 | 只有常數項 |
| 六 | 輸入 `2x²` 和 `−2x²` | x | 2 | 輸入時就消成 0 |
| 七 | x² + 1 | x² − 1 | 0.5 | 加法相消一項、乘法相消中間項、x 為小數 |
| 八 | 1.5x³ − 2.5x | 2x² | −2 | 小數係數、負係數、x 為負數 |

### 測試輸出

測試一：

輸入：

```text
3 3 2 -1 1 1 0
2 1 1 1 0
2
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
3
```

輸出：

```text
A     = 1x + 1
B     = 1x - 1
A + B = 2x
A * B = 1x^2 - 1
A(3)  = 4
```

(x+1)(x−1) 乘出來的 `−x` 和 `+x` 有正確消掉，A(3) = 3 + 1 = 4。

測試三：

輸入：

```text
3 1 1 2 2 3 1
2 4 0 5 3
2
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
5
```

輸出：

```text
A     = 0
B     = 1x + 1
A + B = 1x + 1
A * B = 0
A(5)  = 0
```

測試五：

輸入：

```text
1 5 0
1 3 0
2
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
2
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
0.5
```

輸出：

```text
A     = 1x^2 + 1
B     = 1x^2 - 1
A + B = 2x^2
A * B = 1x^4 - 1
A(0.5)  = 1.25
```

手算驗證：A(0.5) = 0.25 + 1 = 1.25，正確。

測試八：

輸入：

```text
2 1.5 3 -2.5 1
1 2 2
-2
```

輸出：

```text
A     = 1.5x^3 - 2.5x
B     = 2x^2
A + B = 1.5x^3 + 2x^2 - 2.5x
A * B = 3x^5 - 5x^3
A(-2)  = -7
```

手算驗證：A(−2) = 1.5×(−8) − 2.5×(−2) = −12 + 5 = −7，正確。

### 測試結果總整理

| 測試 | 加法 | 乘法 | 代入 | 結果 |
|------|------|------|------|------|
| 一 | 3x²+2 | 3x³+2x²+1 | A(2) = 11 | 通過 |
| 二 | 2x | x²−1 | A(3) = 4 | 通過 |
| 三 | 5x³+2x²+4x+4 | 10x⁵+20x⁴+8x²+16x | A(2) = 16 | 通過 |
| 四 | x+1 | 0 | A(5) = 0 | 通過 |
| 五 | 8 | 15 | A(2) = 5 | 通過 |
| 六 | x | 0 | A(2) = 0 | 通過 |
| 七 | 2x² | x⁴−1 | A(0.5) = 1.25 | 通過 |
| 八 | 1.5x³+2x²−2.5x | 3x⁵−5x³ | A(−2) = −7 | 通過 |

另外，我用 AddressSanitizer 和 UndefinedBehaviorSanitizer 編譯執行（`-fsanitize=address,undefined`），沒有出現記憶體洩漏、重複釋放或越界，代表自己寫的複製建構子、指定運算子和解構子是正確的。

### 編譯與執行指令

## 心得討論

### 這份作業學到的

**一、深層複製很重要。** 這次的類別裡面有 `new[]` 出來的陣列，所以一定要自己寫複製建構子、指定運算子和解構子。最容易想到的是複製建構子，但 `Add(Polynomial poly)` 是傳值，每次呼叫都會用到複製建構子，沒寫好程式一定會當掉。指定運算子裡的 `if (this != &p)` 也很重要，不然 `a = a` 的時候會先把自己刪掉，再從已經刪掉的資料複製。

**二、維持「由大到小排好」這個規則，後面都變簡單。** 一開始我想過每次要用的時候再排序，後來發現只要讓 `termArray` 永遠保持排好，`Add` 就能像合併兩個排序陣列一樣，一次掃完。輸入的時候用 `Add` 把每一項加進去，這樣輸入、加法、輸出的規則就統一了。

**三、乘法的合併步驟。** 一開始以為乘法只要兩層迴圈乘完就好，後來測 (x+1)(x−1) 才發現會有 `−x` 和 `x` 兩項，一定要合併同次項。所以改成先全部乘出來，排序，再把相同指數的加在一起，這樣也順便處理了係數相消成 0 的情況。

**四、動態陣列加倍。** `NewTerm` 容量不夠時直接加倍，而不是一次加 1。我用效能分析的觀念想過，加 1 的話每加一項都要複製一次，總共是 O(n²)；加倍的話平均下來每項只要 O(1)。

**五、理論和實測要對照著看。** 效能分析是用推的，效能量測是真的去跑。我把 n 加倍之後看時間變化：`Add` 約 2 倍、`Mult` 約 4 倍多一點、`operator>>` 約 4 倍，都和推出來的複雜度吻合。尤其 `operator>>` 一開始我沒有想到它會是 O(n²)，是量測之後才發現輸入一萬多項就明顯變慢。

### 和作業一的比較

作業一的重點是用陣列做**堆疊**，把遞迴改成非遞迴；這次是用陣列做**多項式**。兩次都是自己管理陣列和記憶體，不同的是，作業一的堆疊是全域陣列，不會被複製，而這次的物件會被複製、指定、傳值，所以多了「三大函式」要處理。

### 遇到的問題與可以改進的地方

1. **輸出係數 1 的格式。** 目前會印成 `1x`、`3x^2 - 1x + 1`，數學上通常寫成 `x`、`3x^2 - x + 1`。只要在 `operator<<` 裡，當係數的絕對值是 1 而且指數大於 0 時不印係數就可以改善。
2. **`operator>>` 太慢。** 如效能量測所示，輸入 n 項是 O(n²)，可以改成先讀進暫時陣列、排序、合併，變成 O(n log n)。
3. **`Add` 和 `Mult` 的參數傳值。** 每次呼叫都多複製一份，可以改成 `const Polynomial&` 傳參考，省掉不必要的複製。
4. **`sum != 0` 的浮點數比較。** 係數用 `float`，小數相加可能出現不剛好等於 0 的情況（例如 0.1 + 0.2 − 0.3）。這次測試的係數都能被精確表示，沒有遇到問題，但更嚴謹的做法是用一個很小的誤差範圍來判斷。
5. **沒有檢查輸入。** 輸入負的項數或負指數時，程式不會報錯，這題的規格沒有要求，但如果要做成真正可用的類別，應該加上檢查。

### 總結

這次作業讓我把「類別、動態記憶體、運算子多載、排序與合併」一次用到。最大的收穫是：先決定好資料的**不變條件**（這題是「由大到小排好、沒有 0 係數、沒有重複指數」），後面的函式就能寫得簡單，也比較不容易出錯。

## 申論及開發報告

### 這題用到的資料結構是動態陣列

多項式的項數在執行前不知道，使用者輸入才知道，而且相加、相乘之後項數還會再變，所以不能用固定大小的陣列。我用 `new[]` 在執行時才配置 `termArray`，並用 `capacity`（容量）和 `terms`（實際項數）兩個變數分開記錄，容量不夠時再擴大。這就是動態陣列（和 C++ `vector` 的原理一樣）。

### 為什麼只存非零項

多項式有兩種常見的存法：

| 存法 | 做法 | 優點 | 缺點 |
|------|------|------|------|
| 稠密（每個次方一格） | `coef[i]` 存 xⁱ 的係數 | 取任一項係數很快 | 像 x¹⁰⁰⁰ + 1 只有兩項卻要用 1001 格 |
| 稀疏（只存非零項） | 每項存（係數，指數） | 空間只跟項數有關 | 找特定次方要花時間 |

這題的 `Term` 結構就是稀疏的做法。因為 `Add`、`Mult`、`Eval`、輸出都是「走過每一項」的操作，不需要隨機取特定次方，所以稀疏存法比較適合，而且空間隨項數而不是最高次方成長。

### 為什麼要維持「指數由大到小」

整個類別有一個不變條件：`termArray` 永遠由大到小排好，沒有係數為 0 的項，也沒有重複的指數。這個條件帶來三個好處：

1. **加法可以用雙指標合併。** 兩邊都排好，每次比兩個指標指到的指數，只要往前走，時間是 O(m + n)，不需要排序。
2. **輸出順序固定。** 照陣列順序印就是標準寫法（高次在前）。
3. **輸入可以統一處理。** 不管使用者怎麼亂序輸入，`operator>>` 都用 `Add` 一項一項加進去，結果自然滿足這個條件，不需要另外寫排序和合併。

代價是每個會產生新多項式的函式（`Add`、`Mult`、`operator>>`）都要自己負責維持這個條件，所以它們都是用 `NewTerm` 依序從大到小放進結果。

### 為什麼容量用「加倍」擴充

`NewTerm` 在陣列滿了的時候把容量乘 2。假設一次只加 1，每加一項都要複製一次，加 n 項總共複製約 n²/2 次；加倍的話，複製的總次數是 1 + 2 + 4 + ... + n，不超過 2n，平均每一項只要 O(1)。代價是最多會浪費一半的空間，但換來速度很划算。

### 為什麼乘法用「排序再合併」

乘法的難處是：不同的兩項乘起來可能得到同樣的指數（例如 x² · 1 和 x · x），不能直接依序放進結果。我考慮過三種做法：

| 做法 | 時間 | 說明 |
|------|------|------|
| 每乘出一項就用 `Add` 加進結果 | 約 O(m²n²) | 每次 `Add` 都要掃一遍整個結果，太慢 |
| 開一個長度為「最高次方 + 1」的陣列累加 | O(mn + 最高次方) | 快，但稀疏的多項式（如 x¹⁰⁰⁰）會浪費空間 |
| 全部乘出來，排序，掃一遍合併 | O(mn log(mn)) | 空間只跟項數有關，不受次方大小影響 |

我選第三種，因為它和前面選擇稀疏存法的理由一致：空間只跟項數有關。排序用 `std::sort`，指數相同的項排完後一定相鄰，掃一遍就能把係數加起來，加起來是 0 的項就不放進結果，所以像 (x+1)(x−1) 的中間項會自動消掉。

### 為什麼一定要寫三大函式

類別裡面有指標 `termArray`。如果不寫複製建構子，編譯器預設的是「淺層複製」，兩個物件的指標會指到同一塊記憶體，其中一個物件被解構時 `delete[]` 之後，另一個就變成懸空指標，再被解構時就會重複釋放而當掉。這份程式裡 `Add(Polynomial poly)` 和 `Mult(Polynomial poly)` 都是傳值，每次呼叫都會觸發複製建構子，所以三大函式是必要的，不是多餘的：

- **複製建構子**：開新陣列，把每一項抄過來。
- **指定運算子**：先檢查自我指定，再刪舊的、開新的、抄資料。
- **解構子**：`delete[] termArray`，歸還記憶體。

我用 AddressSanitizer 測試過，沒有出現記憶體洩漏或重複釋放，確認這三個函式寫得正確。

### 封裝與 friend 的使用

`Term` 的 `coef` 和 `exp` 是 private，只有 `Polynomial` 和 `operator<<` 能存取，外面的程式不能直接改，避免有人破壞「由大到小排好」的不變條件。`operator>>` 和 `operator<<` 要用 `cin >> A`、`cout << A` 的寫法，左邊是串流物件而不是多項式，所以不能寫成成員函式，只能寫成 friend 函式，讓它們可以存取 `Polynomial` 的私有成員。

### 設計上的取捨

| 取捨 | 我的選擇 | 理由 |
|------|---------|------|
| 空間 vs 取值速度 | 稀疏存法 | 所有操作都是走過每一項，不需要隨機取值 |
| 簡單 vs 輸入速度 | `operator>>` 用 `Add` 插入 | 程式短、規則統一，缺點是 O(n²)，輸入項數不多時沒有影響 |
| 簡單 vs 記憶體 | `Add`、`Mult` 傳值 | 符合題目給的函式形式，缺點是多一次複製 |
| 速度 vs 空間 | 乘法用排序合併 | 空間不受最高次方影響，缺點是要多 O(mn) 的暫存空間 |

### 小結

這題的核心不是演算法有多難，而是**資料結構的設計**：選稀疏的動態陣列、維持由大到小的不變條件、用加倍擴充攤平成本。選定之後，加法變成雙指標合併、乘法變成排序合併、輸入輸出也都能用同一套規則處理。
```shell
$ g++ -std=c++17 -o poly poly.cpp
$ ./poly
```
