#include <Novice.h>
#include <cstdio>  // sprintf_s
#include <cstring> // memcpy

const char kWindowTitle[] = "LE2B_22_ヘンミ_ハクト";

//==================================================
// 入力管理（押した瞬間を取りやすくする）
//==================================================
class InputManager {
public:
  void Update(const char *keys, const char *preKeys) {
    keys_ = keys;
    preKeys_ = preKeys;
  }

  bool Trigger(int dik) const {
    return (preKeys_[dik] == 0 && keys_[dik] != 0);
  }

  bool Press(int dik) const { return (keys_[dik] != 0); }

private:
  const char *keys_ = nullptr;
  const char *preKeys_ = nullptr;
};

//==================================================
// 共有データ（シーン間で渡したい値）
//==================================================
struct SharedData {
  int lastScore = 0;
};

//==================================================
// ステート（シーン）基底クラス（State Pattern）
//==================================================
enum class SceneID { Title, Stage, Result };

class IScene {
public:
  virtual ~IScene() {}
  virtual SceneID Update(const InputManager &input, SharedData &shared) = 0;
  virtual void Draw(const SharedData &shared) = 0;
};

//==================================================
// TitleScene
//==================================================
class TitleScene : public IScene {
public:
  SceneID Update(const InputManager &input, SharedData & /*shared*/) override {
    // SPACE でステージへ
    if (input.Trigger(DIK_SPACE)) {
      return SceneID::Stage;
    }
    return SceneID::Title;
  }

  void Draw(const SharedData & /*shared*/) override {
    Novice::ScreenPrintf(40, 40, "=== TITLE ===");
    Novice::ScreenPrintf(40, 70, "Press SPACE to Start");
    Novice::ScreenPrintf(40, 100, "ESC : Exit");
  }
};

//==================================================
// StageScene（簡易シューティング：移動＋弾、スコア）
//==================================================
class StageScene : public IScene {
public:
  StageScene() { Reset(); }

  SceneID Update(const InputManager &input, SharedData &shared) override {
    // Player move
    if (input.Press(DIK_LEFT))
      px_ -= speed_;
    if (input.Press(DIK_RIGHT))
      px_ += speed_;
    if (input.Press(DIK_UP))
      py_ -= speed_;
    if (input.Press(DIK_DOWN))
      py_ += speed_;

    // clamp
    if (px_ < 0)
      px_ = 0;
    if (px_ > 1280 - pSize_)
      px_ = 1280 - pSize_;
    if (py_ < 0)
      py_ = 0;
    if (py_ > 720 - pSize_)
      py_ = 720 - pSize_;

    // Shot (SPACE trigger)
    if (input.Trigger(DIK_SPACE)) {
      Fire();
    }

    // Update bullets
    for (int i = 0; i < kMaxBullets; ++i) {
      if (!bullets_[i].alive)
        continue;
      bullets_[i].y -= bulletSpeed_;
      if (bullets_[i].y < -20) {
        bullets_[i].alive = false;
      }
    }

    // Simple target (moving)
    targetX_ += targetVX_;
    if (targetX_ < 0 || targetX_ > 1280 - targetW_) {
      targetVX_ *= -1;
    }

    // Hit check (AABB)
    for (int i = 0; i < kMaxBullets; ++i) {
      if (!bullets_[i].alive)
        continue;

      int bx = bullets_[i].x;
      int by = bullets_[i].y;

      bool hit = bx < targetX_ + targetW_ && bx + bSize_ > targetX_ &&
                 by < targetY_ + targetH_ && by + bSize_ > targetY_;

      if (hit) {
        bullets_[i].alive = false;
        score_ += 10;
      }
    }

    // 制限時間（例：10秒）で結果へ
    frame_++;
    if (frame_ >= limitFrames_) {
      shared.lastScore = score_;
      return SceneID::Result;
    }

    // ENTER で強制的に結果へ（確認しやすく）
    if (input.Trigger(DIK_RETURN)) {
      shared.lastScore = score_;
      return SceneID::Result;
    }

    return SceneID::Stage;
  }

  void Draw(const SharedData & /*shared*/) override {
    Novice::ScreenPrintf(40, 40, "=== STAGE ===");
    Novice::ScreenPrintf(40, 70,
                         "Arrow Keys: Move  SPACE: Shot  ENTER: Result");

    // score & timer
    int remain = (limitFrames_ - frame_) / 60;
    Novice::ScreenPrintf(40, 100, "Score: %d", score_);
    Novice::ScreenPrintf(40, 130, "Time : %d", remain);

    // player
    Novice::DrawBox(px_, py_, pSize_, pSize_, 0.0f, 0x00FF00FF, kFillModeSolid);

    // target
    Novice::DrawBox(targetX_, targetY_, targetW_, targetH_, 0.0f, 0xFF0000FF,
                    kFillModeSolid);

    // bullets
    for (int i = 0; i < kMaxBullets; ++i) {
      if (!bullets_[i].alive)
        continue;
      Novice::DrawBox(bullets_[i].x, bullets_[i].y, bSize_, bSize_, 0.0f,
                      0xFFFFFFFF, kFillModeSolid);
    }
  }

private:
  struct Bullet {
    int x = 0;
    int y = 0;
    bool alive = false;
  };

  static const int kMaxBullets = 32;

  // player
  int px_ = 0;
  int py_ = 0;
  int pSize_ = 32;
  int speed_ = 6;

  // bullet
  Bullet bullets_[kMaxBullets]{};
  int bSize_ = 8;
  int bulletSpeed_ = 10;

  // target
  int targetX_ = 0;
  int targetY_ = 160;
  int targetW_ = 80;
  int targetH_ = 40;
  int targetVX_ = 5;

  // score & timer
  int score_ = 0;
  int frame_ = 0;
  int limitFrames_ = 60 * 10; // 10 seconds

  void Reset() {
    px_ = 1280 / 2 - pSize_ / 2;
    py_ = 720 - 80;

    for (int i = 0; i < kMaxBullets; ++i) {
      bullets_[i].alive = false;
    }

    targetX_ = 100;
    targetY_ = 160;
    targetVX_ = 5;

    score_ = 0;
    frame_ = 0;
  }

  void Fire() {
    for (int i = 0; i < kMaxBullets; ++i) {
      if (bullets_[i].alive)
        continue;
      bullets_[i].alive = true;
      bullets_[i].x = px_ + pSize_ / 2 - bSize_ / 2;
      bullets_[i].y = py_ - bSize_;
      break;
    }
  }
};

//==================================================
// ResultScene
//==================================================
class ResultScene : public IScene {
public:
  SceneID Update(const InputManager &input, SharedData & /*shared*/) override {
    // SPACE でタイトルへ
    if (input.Trigger(DIK_SPACE)) {
      return SceneID::Title;
    }
    return SceneID::Result;
  }

  void Draw(const SharedData &shared) override {
    Novice::ScreenPrintf(40, 40, "=== RESULT ===");
    Novice::ScreenPrintf(40, 80, "Score: %d", shared.lastScore);
    Novice::ScreenPrintf(40, 120, "Press SPACE to Title");
  }
};

//==================================================
// SceneManager（ステートの保持と切替）
//==================================================
class SceneManager {
public:
  SceneManager() { Change(SceneID::Title); }

  void Update(const InputManager &input, SharedData &shared) {
    SceneID next = scene_->Update(input, shared);
    if (next != current_) {
      Change(next);
    }
  }

  void Draw(const SharedData &shared) { scene_->Draw(shared); }

private:
  SceneID current_ = SceneID::Title;
  IScene *scene_ = nullptr;

  // 実体を保持
  TitleScene title_;
  StageScene stage_;
  ResultScene result_;

  void Change(SceneID id) {
    current_ = id;
    switch (id) {
    case SceneID::Title:
      scene_ = &title_;
      break;
    case SceneID::Stage:
      scene_ = &stage_;
      break;
    case SceneID::Result:
      scene_ = &result_;
      break;
    }
  }
};

//==================================================
// WinMain
//==================================================
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  Novice::Initialize(kWindowTitle, 1280, 720);

  char keys[256] = {0};
  char preKeys[256] = {0};

  InputManager input;
  SharedData shared;
  SceneManager sceneManager;

  while (Novice::ProcessMessage() == 0) {
    Novice::BeginFrame();

    // input
    memcpy(preKeys, keys, 256);
    Novice::GetHitKeyStateAll(keys);
    input.Update(keys, preKeys);

    ///
    /// ↓更新処理ここから
    ///
    sceneManager.Update(input, shared);
    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///
    sceneManager.Draw(shared);
    ///
    /// ↑描画処理ここまで
    ///

    Novice::EndFrame();

    if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
      break;
    }
  }

  Novice::Finalize();
  return 0;
}
