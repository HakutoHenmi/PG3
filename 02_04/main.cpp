#include <functional> // std::function
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

// プロトタイプ宣言（std::function を受け取る）
void DelayReveal(std::function<void(int, int)> fn, unsigned int delayMs,
                 int roll, int userGuess);

int main(void) {
  // 乱数シード初期化
  srand((unsigned int)time(NULL));

  int userGuess = 0;

  // ユーザー入力（半=1 / 丁=0）
  printf("予想を入力してください（半=1 / 丁=0）：");

  // 戻り値を受け取って警告を防ぐ
  int readCount = scanf_s("%d", &userGuess);
  if (readCount != 1) {
    printf("入力エラーです。\n");
    return 1;
  }

  // サイコロの出目（1～6）
  int roll = (rand() % 6) + 1;

  // 判定・結果表示（ラムダ式）
  std::function<void(int, int)> showResult = [](int roll, int userGuess) {
    // 1行目：出目表示
    printf("出目は %d でした。\n", roll);

    // 奇数＝1、偶数＝0 で判定（roll%2 は 1 または 0）
    if ((roll % 2) == userGuess) {
      printf("正解\n");
    } else {
      printf("不正解\n");
    }
  };

  // 3秒後に結果表示（DelayReveal にラムダを渡す）
  DelayReveal(showResult, 3000, roll, userGuess);

  return 0;
}

// 遅延実行関数（std::function で受け取る）
void DelayReveal(std::function<void(int, int)> fn, unsigned int delayMs,
                 int roll, int userGuess) {
  Sleep(delayMs);
  fn(roll, userGuess);
}
