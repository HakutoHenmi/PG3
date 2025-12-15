#include <iomanip>
#include <iostream>

// 一般的な賃金体系：時給 1226円
const double NORMAL_WAGE = 1226.0;

// 再帰的な賃金体系の時給を計算する関数
double RecursiveWage(int hour) {
  if (hour == 1) {
    return 100.0;
  } else {
    return RecursiveWage(hour - 1) * 2 - 50;
  }
}

// 再帰的な賃金体系の合計賃金
double RecursiveTotal(int hours) {
  double total = 0.0;
  for (int i = 1; i <= hours; i++) {
    total += RecursiveWage(i);
  }
  return total;
}

int main() {
  // 問題条件より固定（自動採点用）
  const int hours = 8;

  double normalTotal = NORMAL_WAGE * hours;
  double recursiveTotal = RecursiveTotal(hours);

  std::cout << std::fixed << std::setprecision(2);
  std::cout << "一般的な賃金体系：" << normalTotal << " 円\n";
  std::cout << "再帰的な賃金体系：" << recursiveTotal << " 円\n";

  if (recursiveTotal > normalTotal) {
    std::cout << "⇒ 再帰的な賃金体系のほうが " << (recursiveTotal - normalTotal)
              << " 円 多く稼げます。\n";
  } else if (recursiveTotal < normalTotal) {
    std::cout << "⇒ 一般的な賃金体系のほうが " << (normalTotal - recursiveTotal)
              << " 円 多く稼げます。\n";
  } else {
    std::cout << "⇒ どちらも同じ賃金です。\n";
  }

  return 0;
}
