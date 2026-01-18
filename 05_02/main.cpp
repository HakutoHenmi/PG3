#include <Novice.h>
#include <cstring> // memcpy
#include <memory>
#include <vector>

const char kWindowTitle[] = "LE2B_22_ヘンミ_ハクト";

//==================================================
// キーのトリガー
//==================================================
static bool Trigger(const char *preKeys, const char *keys, int dik) {
  return (preKeys[dik] == 0 && keys[dik] != 0);
}

//==================================================
// データ
//==================================================
struct Unit {
  int x;
  int y;
  int size;
};

struct Selector {
  int x;
  int y;
  int size;
};

//==================================================
// Command Pattern
//==================================================
class ICommand {
public:
  virtual ~ICommand() {}
  virtual void Execute(Unit &unit) = 0;
  virtual void Undo(Unit &unit) = 0;
};

class MoveCommand : public ICommand {
public:
  MoveCommand(int dx, int dy) : dx_(dx), dy_(dy) {}

  void Execute(Unit &unit) override {
    unit.x += dx_;
    unit.y += dy_;
  }
  void Undo(Unit &unit) override {
    unit.x -= dx_;
    unit.y -= dy_;
  }

private:
  int dx_;
  int dy_;
};

// Undo/Redo 管理
class CommandHistory {
public:
  void ExecuteCommand(std::unique_ptr<ICommand> cmd, Unit &unit) {

    if (cursor_ < (int)history_.size()) {
      history_.erase(history_.begin() + cursor_, history_.end());
    }
    cmd->Execute(unit);
    history_.push_back(std::move(cmd));
    cursor_ = (int)history_.size();
  }

  void Undo(Unit &unit) {
    if (cursor_ <= 0)
      return;
    cursor_--;
    history_[cursor_]->Undo(unit);
  }

  void Redo(Unit &unit) {
    if (cursor_ >= (int)history_.size())
      return;
    history_[cursor_]->Execute(unit);
    cursor_++;
  }

  int HistoryCount() const { return (int)history_.size(); }
  int Cursor() const { return cursor_; }

private:
  std::vector<std::unique_ptr<ICommand>> history_;
  int cursor_ = 0;
};

//==================================================
// 画面＆グリッド設定
//==================================================
static const int kScreenW = 1280;
static const int kScreenH = 720;
static const int kGrid = 32;

// グリッドにスナップ
static int Snap(int v) {
  // v を 32刻みに丸め
  return (v / kGrid) * kGrid;
}

// 画面外に出ないように
static void ClampToScreen(int &x, int &y, int size) {
  if (x < 0)
    x = 0;
  if (y < 0)
    y = 0;
  if (x > kScreenW - size)
    x = kScreenW - size;
  if (y > kScreenH - size)
    y = kScreenH - size;
}

// セレクタ位置にユニットがあるか
static int FindUnitOnSelector(const std::vector<Unit> &units,
                              const Selector &sel) {
  for (int i = 0; i < (int)units.size(); ++i) {
    if (units[i].x == sel.x && units[i].y == sel.y) {
      return i;
    }
  }
  return -1;
}

//==================================================
// モード
//==================================================
enum class Mode { Selector, Unit };

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {

  Novice::Initialize(kWindowTitle, kScreenW, kScreenH);

  char keys[256] = {0};
  char preKeys[256] = {0};

  // ----------------------------
  // 初期配置
  // ----------------------------
  std::vector<Unit> units;
  units.push_back({Snap(320), Snap(160), 28});
  units.push_back({Snap(704), Snap(160), 28});
  units.push_back({Snap(928), Snap(288), 28});
  units.push_back({Snap(736), Snap(416), 28});
  units.push_back({Snap(608), Snap(544), 28});

  // セレクタ
  Selector selector{units[0].x, units[0].y, 32};

  // モード
  Mode mode = Mode::Selector;

  // 選択中ユニット
  int selectedIndex = 0;

  // コマンド履歴
  CommandHistory history;

  // 入力移動量
  const int step = kGrid;

  while (Novice::ProcessMessage() == 0) {
    Novice::BeginFrame();

    memcpy(preKeys, keys, 256);
    Novice::GetHitKeyStateAll(keys);

    // ----------------------------
    // 入力：方向
    // ----------------------------
    int dx = 0;
    int dy = 0;

    if (Trigger(preKeys, keys, DIK_W) || Trigger(preKeys, keys, DIK_UP))
      dy = -step;
    if (Trigger(preKeys, keys, DIK_S) || Trigger(preKeys, keys, DIK_DOWN))
      dy = step;
    if (Trigger(preKeys, keys, DIK_A) || Trigger(preKeys, keys, DIK_LEFT))
      dx = -step;
    if (Trigger(preKeys, keys, DIK_D) || Trigger(preKeys, keys, DIK_RIGHT))
      dx = step;

    // ----------------------------
    // 更新処理
    // ----------------------------
    // Space：モード切替
    if (Trigger(preKeys, keys, DIK_SPACE)) {
      if (mode == Mode::Selector) {
        // セレクタ位置にユニットがあれば、それを選択して Unit Mode へ
        int hit = FindUnitOnSelector(units, selector);
        if (hit != -1) {
          selectedIndex = hit;
          mode = Mode::Unit;
          // Unit Mode
          // に入った瞬間の見た目が分かるように、セレクタは選択ユニットに合わせる
          selector.x = units[selectedIndex].x;
          selector.y = units[selectedIndex].y;
        }
      } else {
        // Unit Mode → Selector Mode
        mode = Mode::Selector;
        // セレクタは選択ユニット位置に残す
        selector.x = units[selectedIndex].x;
        selector.y = units[selectedIndex].y;
      }
    }

    if (mode == Mode::Selector) {
      // セレクタだけ動かす
      if (dx != 0 || dy != 0) {
        selector.x += dx;
        selector.y += dy;
        selector.x = Snap(selector.x);
        selector.y = Snap(selector.y);
        ClampToScreen(selector.x, selector.y, selector.size);
      }

    } else {
      // Unit Mode：選択ユニットをコマンドで動かす
      if (dx != 0 || dy != 0) {
        auto cmd = std::make_unique<MoveCommand>(dx, dy);
        history.ExecuteCommand(std::move(cmd), units[selectedIndex]);

        // セレクタもユニットに追従
        selector.x = units[selectedIndex].x;
        selector.y = units[selectedIndex].y;
      }

      // Undo / Redo
      if (Trigger(preKeys, keys, DIK_Z)) {
        history.Undo(units[selectedIndex]);
        selector.x = units[selectedIndex].x;
        selector.y = units[selectedIndex].y;
      }
      if (Trigger(preKeys, keys, DIK_Y)) {
        history.Redo(units[selectedIndex]);
        selector.x = units[selectedIndex].x;
        selector.y = units[selectedIndex].y;
      }
    }

    // 移動後のクランプ
    for (auto &u : units) {
      ClampToScreen(u.x, u.y, u.size);
      u.x = Snap(u.x);
      u.y = Snap(u.y);
    }

    // ----------------------------
    // 描画処理
    // ----------------------------
    // 背景
    Novice::DrawBox(0, 0, kScreenW, kScreenH, 0.0f, 0x4A78A0FF, kFillModeSolid);

    // グリッド線
    for (int x = 0; x <= kScreenW; x += kGrid) {
      Novice::DrawLine(x, 0, x, kScreenH, 0xBFD9F0FF);
    }
    for (int y = 0; y <= kScreenH; y += kGrid) {
      Novice::DrawLine(0, y, kScreenW, y, 0xBFD9F0FF);
    }

    // ユニット
    for (int i = 0; i < (int)units.size(); ++i) {
      const Unit &u = units[i];
      Novice::DrawBox(u.x + 2, u.y + 2, u.size, u.size, 0.0f, 0xFFFFFFFF,
                      kFillModeSolid);
    }

    // セレクタ（赤枠）
    int sx = selector.x;
    int sy = selector.y;
    int ss = selector.size;

    // 上下
    Novice::DrawBox(sx, sy, ss, 2, 0.0f, 0xFF3333FF, kFillModeSolid);
    Novice::DrawBox(sx, sy + ss - 2, ss, 2, 0.0f, 0xFF3333FF, kFillModeSolid);
    // 左右
    Novice::DrawBox(sx, sy, 2, ss, 0.0f, 0xFF3333FF, kFillModeSolid);
    Novice::DrawBox(sx + ss - 2, sy, 2, ss, 0.0f, 0xFF3333FF, kFillModeSolid);

    // 画面下の黒帯
    const int barH = 90;
    Novice::DrawBox(0, kScreenH - barH, kScreenW, barH, 0.0f, 0x000000CC,
                    kFillModeSolid);

    // 説明文
    Novice::ScreenPrintf(
        20, kScreenH - 80,
        "WASD||arrow keys: move / space key: change unit mode");

    if (mode == Mode::Selector) {
      Novice::ScreenPrintf(
          20, kScreenH - 55,
          "In Selector Mode, you cannot use the 'Undo' action.");
    } else {
      Novice::ScreenPrintf(20, kScreenH - 55,
                           "Unit Mode: Z=Undo  Y=Redo   history=%d  cursor=%d",
                           history.HistoryCount(), history.Cursor());
    }

    // 現在モード表示
    Novice::ScreenPrintf(20, kScreenH - 30, "Mode: %s",
                         (mode == Mode::Selector) ? "Selector" : "Unit");

    Novice::EndFrame();

    if (Trigger(preKeys, keys, DIK_ESCAPE)) {
      break;
    }
  }

  Novice::Finalize();
  return 0;
}
