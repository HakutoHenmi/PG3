#include <iostream>
using namespace std;

// ================================
// 関数テンプレート Min()
// 2つの値を比べて小さい方を返す
// ================================
template <typename T> T Min(T a, T b) { return (a < b) ? a : b; }

int main() {
  // int型の例
  int i1 = 10, i2 = 20;
  cout << "Min(int): " << Min(i1, i2) << endl;

  // float型の例
  float f1 = 3.14f, f2 = 2.71f;
  cout << "Min(float): " << Min(f1, f2) << endl;

  // double型の例
  double d1 = 1.234, d2 = 5.678;
  cout << "Min(double): " << Min(d1, d2) << endl;

  return 0;
}
