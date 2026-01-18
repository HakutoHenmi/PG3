#include <atomic>
#include <fstream>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

// 2Dマップ
using Map = std::vector<std::vector<int>>;

static char TileToChar(int id) {
  // マップチップ表現
  switch (id) {
  case 0:
    return '.'; // 床
  case 1:
    return '#'; // 壁
  case 2:
    return '~'; // 水
  case 3:
    return '+'; // 何か
  default:
    return '?'; // 未定義
  }
}

// CSV読み込み
static bool LoadCsvMap(const std::string &path, Map &outMap,
                       std::string &outError) {
  std::ifstream ifs(path);
  if (!ifs) {
    outError = "CSV file open failed: " + path;
    return false;
  }

  Map temp;
  std::string line;

  while (std::getline(ifs, line)) {
    if (line.empty())
      continue;

    std::vector<int> row;
    std::stringstream ss(line);
    std::string cell;

    while (std::getline(ss, cell, ',')) {
      // 空白を許容
      // 例: "1, 2,3"
      try {
        int v = std::stoi(cell);
        row.push_back(v);
      } catch (...) {
        outError = "CSV parse error (not int): " + cell;
        return false;
      }
    }

    if (!row.empty()) {
      temp.push_back(std::move(row));
    }
  }

  if (temp.empty()) {
    outError = "CSV is empty.";
    return false;
  }

  outMap = std::move(temp);
  return true;
}

int main() {
  // 共有データ
  Map mapData;
  std::mutex mapMutex;

  std::atomic<bool> loaded{false};
  std::atomic<bool> failed{false};
  std::atomic<bool> running{true};
  std::string errorMsg;

  const std::string csvPath = "map.csv";

  // ----------------------------
  // バックグラウンドスレッドで読み込み
  // ----------------------------
  std::thread loader([&]() {
    Map temp;
    std::string err;
    bool ok = LoadCsvMap(csvPath, temp, err);

    {
      std::lock_guard<std::mutex> lock(mapMutex);
      if (ok) {
        mapData = std::move(temp);
      } else {
        errorMsg = err;
      }
    }

    if (ok) {
      loaded.store(true, std::memory_order_release);
    } else {
      failed.store(true, std::memory_order_release);
    }
  });

  // ----------------------------
  // メインスレッド：読み込み中表示
  // ----------------------------
  const char spinner[] = {'|', '/', '-', '\\'};
  int sp = 0;

  while (running.load()) {
    if (loaded.load(std::memory_order_acquire)) {
      break;
    }
    if (failed.load(std::memory_order_acquire)) {
      break;
    }

    // 読み込み中の表示
    std::cout << "\rLoading CSV in background... " << spinner[sp % 4]
              << std::flush;
    sp++;

    // CPUを譲る
    std::this_thread::yield();
  }

  std::cout << "\n";

  // スレッド待機
  loader.join();

  // ----------------------------
  // 結果表示
  // ----------------------------
  if (failed.load(std::memory_order_acquire)) {
    std::lock_guard<std::mutex> lock(mapMutex);
    std::cout << "Load failed.\n";
    std::cout << errorMsg << "\n";
    std::cout << "Example map.csv:\n";
    std::cout << "0,0,0,1,1\n0,2,0,0,1\n0,0,3,0,0\n";
    return 1;
  }

  // マップチップとして表示
  Map copy;
  {
    std::lock_guard<std::mutex> lock(mapMutex);
    copy = mapData;
  }

  std::cout << "Load complete! Display map chips:\n\n";

  for (const auto &row : copy) {
    for (int v : row) {
      std::cout << TileToChar(v);
    }
    std::cout << "\n";
  }

  return 0;
}
