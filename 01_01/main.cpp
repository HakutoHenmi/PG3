// main.cpp

#include <cstring> // strcmp
#include <iomanip>
#include <iostream>
#include <list>

// ==== 文字列ユーティリティ：std::string を使わない ====
static bool StrEq(const char *a, const char *b) {
  return std::strcmp(a, b) == 0;
}

// リストから一致する駅名を1件削除
static void RemoveByName(std::list<const char *> &L, const char *name) {
  for (auto it = L.begin(); it != L.end(); ++it) {
    if (StrEq(*it, name)) {
      L.erase(it);
      return;
    }
  }
}

// 指定駅 afterName の直後に newName を挿入
static void InsertAfter(std::list<const char *> &L, const char *afterName,
                        const char *newName) {
  for (auto it = L.begin(); it != L.end(); ++it) {
    if (StrEq(*it, afterName)) {
      L.insert(std::next(it), newName);
      return;
    }
  }
  L.push_back(newName);
}

// 1行の矢印並び表示
static void PrintLineup(const std::list<const char *> &L, const char *title) {
  std::cout << "===== " << title << " =====\n";
  size_t i = 0, n = L.size();
  for (const auto &s : L) {
    std::cout << s;
    if (++i < n)
      std::cout << "  ->  ";
  }
  std::cout << "\n\n";
}

// 表形式表示
static void PrintTable(const std::list<const char *> &L, const char *title) {
  std::cout << "===== " << title << " =====\n";
  std::cout << "----------------------\n";
  int no = 1;
  for (const auto &s : L) {
    std::cout << std::left << std::setw(4) << no++ << s << "\n";
  }
  std::cout << "\n";
}

// ==== 年代別データ ====
// 2019年(高輪ゲートウェイ直前)：Nishi-Nippori(1971開業)は含める
static std::list<const char *> Make2019() {
  const char *seq[] = {
      "Tokyo",     "Kanda",      "Akihabara",    "Okachimachi",
      "Ueno",      "Uguisudani", "Nippori",      "Nishi-Nippori",
      "Tabata",    "Komagome",   "Sugamo",       "Otsuka",
      "Ikebukuro", "Mejiro",     "Takadanobaba", "Shin-Okubo",
      "Shinjuku",  "Yoyogi",     "Harajuku",     "Shibuya",
      "Ebisu",     "Meguro",     "Gotanda",      "Osaki",
      "Shinagawa", "Tamachi",    "Hamamatsucho", "Shimbashi",
      "Yurakucho"};
  std::list<const char *> L;
  for (const char *s : seq)
    L.push_back(s);
  return L;
}

// 1970: Nishi-Nippori(1971) before opening -> removed from 2019
static std::list<const char *> Make1970() {
  auto L = Make2019();
  RemoveByName(L, "Nishi-Nippori");
  return L;
}

// 2022年：Takanawa Gatewa(2020)を Shinagawa と Tamachi の間へ挿入
static std::list<const char *> Make2022() {
  auto L = Make2019();
  InsertAfter(L, "Shinagawa", "Takanawa Gateway"); // 品川の直後に挿入
  return L;
}

int main() {
  // === 年代別リスト作成 ===
  auto L1970 = Make1970();
  auto L2019 = Make2019();
  auto L2022 = Make2022();

  // === 駅の並び ===
  PrintLineup(L1970, "1970 lineup (before Nishi-Nippori)");
  PrintLineup(L2019, "2019 lineup");
  PrintLineup(L2022, "2022 lineup (with Takanawa Gateway)");

  // === 表形式 ===
  PrintTable(L1970, "1970");
  PrintTable(L2019, "2019");
  PrintTable(L2022, "2022");

  // === 駅数まとめ ===
  std::cout << "Stations count: "
            << "1970=" << L1970.size() << ", "
            << "2019=" << L2019.size() << ", "
            << "2022=" << L2022.size()
            << "  (2019->2022: +1 by Takanawa Gateway)\n";

  return 0;
}
