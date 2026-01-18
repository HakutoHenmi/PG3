#include <stdio.h>
#include <windows.h>

class Enemy {
public:
  Enemy() : m_state(&Enemy::Approach), m_finished(false) {}

  void Update() {
    // 現在の状態（メンバ関数ポインタ）を実行
    (this->*m_state)();
  }

  bool IsFinished() const { return m_finished; }

private:
  // メンバ関数ポインタ型
  using StateFunc = void (Enemy::*)();

  // 現在の状態
  StateFunc m_state;

  // 離脱が終わったら終了
  bool m_finished;

private:
  void Approach() {
    printf("【接近】敵が接近しています...\n");
    Sleep(1000);

    // 次の状態へ
    m_state = &Enemy::Shoot;
  }

  void Shoot() {
    printf("【射撃】敵が射撃しました！\n");
    Sleep(1000);

    // 次の状態へ
    m_state = &Enemy::Retreat;
  }

  void Retreat() {
    printf("【離脱】敵が離脱しました。\n");
    Sleep(1000);

    // 終了
    m_finished = true;
  }
};

int main(void) {
  Enemy enemy;

  // 状態遷移（接近 → 射撃 → 離脱）を順に実行
  while (!enemy.IsFinished()) {
    enemy.Update();
  }

  printf("処理を終了します。\n");
  return 0;
}
