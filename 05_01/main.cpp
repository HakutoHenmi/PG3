#include <Novice.h>
#include <cstring> // memcpy
#include <memory>
#include <vector>

const char kWindowTitle[] = "LE2B_22_ヘンミ_ハクト";

//==================================================
// Receiver
//==================================================
struct Player {
  int x = 640;
  int y = 360;
  int size = 32;

  void Move(int dx, int dy) {
    x += dx;
    y += dy;

    // 画面外に出ないように簡易クランプ
    if (x < 0)
      x = 0;
    if (x > 1280 - size)
      x = 1280 - size;
    if (y < 0)
      y = 0;
    if (y > 720 - size)
      y = 720 - size;
  }
};

//==================================================
// Command
//==================================================
class ICommand {
public:
  virtual ~ICommand() {}
  virtual void Execute(Player &player) = 0;
  virtual void Undo(Player &player) = 0;
};

// 移動コマンド
class MoveCommand : public ICommand {
public:
  MoveCommand(int dx, int dy) : dx_(dx), dy_(dy) {}

  void Execute(Player &player) override { player.Move(dx_, dy_); }

  void Undo(Player &player) override { player.Move(-dx_, -dy_); }

private:
  int dx_;
  int dy_;
};

//==================================================
// Invoker
//==================================================
class CommandManager {
public:
  void ExecuteCommand(std::unique_ptr<ICommand> cmd, Player &player) {

    if (cursor_ < (int)history_.size()) {
      history_.erase(history_.begin() + cursor_, history_.end());
    }

    cmd->Execute(player);
    history_.push_back(std::move(cmd));
    cursor_ = (int)history_.size();
  }

  void Undo(Player &player) {
    if (cursor_ <= 0)
      return;
    cursor_--;
    history_[cursor_]->Undo(player);
  }

  void Redo(Player &player) {
    if (cursor_ >= (int)history_.size())
      return;
    history_[cursor_]->Execute(player);
    cursor_++;
  }

  int GetHistoryCount() const { return (int)history_.size(); }
  int GetCursor() const { return cursor_; } // 現在位置

private:
  std::vector<std::unique_ptr<ICommand>> history_;
  int cursor_ = 0;
};

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  // ライブラリの初期化
  Novice::Initialize(kWindowTitle, 1280, 720);

  // キー入力結果を受け取る箱
  char keys[256] = {0};
  char preKeys[256] = {0};

  // ゲーム用データ
  Player player;
  CommandManager cmdMgr;

  const int step = 16; // 1回の移動量

  // ウィンドウの×ボタンが押されるまでループ
  while (Novice::ProcessMessage() == 0) {
    // フレームの開始
    Novice::BeginFrame();

    // キー入力を受け取る
    memcpy(preKeys, keys, 256);
    Novice::GetHitKeyStateAll(keys);

    auto Trigger = [&](int dik) -> bool {
      return (preKeys[dik] == 0 && keys[dik] != 0);
    };

    ///
    /// ↓更新処理ここから
    ///

    // コマンド生成 → 実行 → 履歴に保持
    if (Trigger(DIK_W)) {
      cmdMgr.ExecuteCommand(std::make_unique<MoveCommand>(0, -step), player);
    }
    if (Trigger(DIK_S)) {
      cmdMgr.ExecuteCommand(std::make_unique<MoveCommand>(0, step), player);
    }
    if (Trigger(DIK_A)) {
      cmdMgr.ExecuteCommand(std::make_unique<MoveCommand>(-step, 0), player);
    }
    if (Trigger(DIK_D)) {
      cmdMgr.ExecuteCommand(std::make_unique<MoveCommand>(step, 0), player);
    }

    // Undo / Redo
    if (Trigger(DIK_Z)) {
      cmdMgr.Undo(player);
    }
    if (Trigger(DIK_Y)) {
      cmdMgr.Redo(player);
    }

    ///
    /// ↑更新処理ここまで
    ///

    ///
    /// ↓描画処理ここから
    ///

    // 背景っぽい枠
    Novice::DrawBox(0, 0, 1280, 720, 0.0f, 0x4A78A0FF, kFillModeSolid);

    // プレイヤー（白い四角）
    Novice::DrawBox(player.x, player.y, player.size, player.size, 0.0f,
                    0xFFFFFFFF, kFillModeSolid);

    // 説明表示
    Novice::ScreenPrintf(20, 20, "05_01 Command Pattern (Novice)");
    Novice::ScreenPrintf(20, 45,
                         "W/A/S/D : Move (Create Command -> Execute -> Store)");
    Novice::ScreenPrintf(20, 70, "Z : Undo   Y : Redo");
    Novice::ScreenPrintf(20, 95, "History: %d   Cursor: %d",
                         cmdMgr.GetHistoryCount(), cmdMgr.GetCursor());

    ///
    /// ↑描画処理ここまで
    ///

    // フレームの終了
    Novice::EndFrame();

    // ESCキーが押されたらループを抜ける
    if (preKeys[DIK_ESCAPE] == 0 && keys[DIK_ESCAPE] != 0) {
      break;
    }
  }

  // ライブラリの終了
  Novice::Finalize();
  return 0;
}
