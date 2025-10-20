#include <iomanip>
#include <iostream>

// 一般的な賃金体系：時給 1226円
const double NORMAL_WAGE = 1226.0;

// 再帰的な賃金体系の時給を計算する関数
double RecursiveWage(int hour) {
  if (hour == 1) {
    return 100.0; // 最初の1時間は100円
  } else {
    // 「前の1時間でもらった時給 * 2 - 50」
    return RecursiveWage(hour - 1) * 2 - 50;
  }
}

// 再帰的な賃金体系の合計賃金を計算する関数
double RecursiveTotal(int hours) {
  double total = 0.0;
  for (int i = 1; i <= hours; i++) {
    total += RecursiveWage(i);
  }
  return total;
}

int main() {
  int hours;

  std::cout << "勤務時間を入力してください（時間）: ";
  std::cin >> hours;

  if (hours <= 0) {
    std::cout << "勤務時間は1以上を入力してください。" << std::endl;
    return 0;
  }

  // 一般的な賃金体系の計算
  double normalTotal = NORMAL_WAGE * hours;

  // 再帰的な賃金体系の計算
  double recursiveTotal = RecursiveTotal(hours);

  // 結果の表示
  std::cout << std::fixed << std::setprecision(2);
  std::cout << "\n【結果】" << std::endl;
  std::cout << "一般的な賃金体系：" << normalTotal << " 円" << std::endl;
  std::cout << "再帰的な賃金体系：" << recursiveTotal << " 円" << std::endl;

  // どちらが得かを比較
  if (recursiveTotal > normalTotal) {
    std::cout << "\n⇒ 再帰的な賃金体系のほうが "
              << (recursiveTotal - normalTotal) << " 円 多く稼げます！"
              << std::endl;
  } else if (recursiveTotal < normalTotal) {
    std::cout << "\n⇒ 一般的な賃金体系のほうが "
              << (normalTotal - recursiveTotal) << " 円 多く稼げます。"
              << std::endl;
  } else {
    std::cout << "\n⇒ どちらも同じ賃金です。" << std::endl;
  }

  return 0;
}
