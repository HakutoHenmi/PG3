// main.cpp
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <regex>
#include <string>
#include <vector>

// ==============================
//  空白
// ==============================
static inline void TrimInPlace(std::string &s) {
  auto issp = [](unsigned char c) { return std::isspace(c); };
  while (!s.empty() && issp(s.front()))
    s.erase(s.begin());
  while (!s.empty() && issp(s.back()))
    s.pop_back();
}

// ==============================
//  学籍番号キー抽出
// ==============================
struct StuKey {
  int year = 0; // kの後ろ3桁
  int num = 0;  // gの後ろ4桁
  bool valid = false;
};

static StuKey ParseKey(const std::string &mail) {
  static const std::regex re(R"(k(\d{3})g(\d{4})@)", std::regex::icase);
  std::smatch m;
  StuKey key;
  if (std::regex_search(mail, m, re) && m.size() >= 3) {
    key.year = std::stoi(m[1].str());
    key.num = std::stoi(m[2].str());
    key.valid = true;
  }
  return key;
}

// ==============================
//  比較関数：学籍番号 昇順
// ==============================
static bool LessByStuId(const std::string &a, const std::string &b) {
  StuKey ka = ParseKey(a), kb = ParseKey(b);
  if (ka.valid != kb.valid)
    return ka.valid && !kb.valid;
  if (!ka.valid)
    return a < b;
  if (ka.year != kb.year)
    return ka.year < kb.year;
  if (ka.num != kb.num)
    return ka.num < kb.num;
  return a < b;
}

// ==============================
//  ファイル読み込み
//   - 区切り: , / 改行 / 空白 / ;
// ==============================
static bool LoadLinks(const std::string &path, std::vector<std::string> &out) {
  std::ifstream ifs(path);
  if (!ifs)
    return false;

  std::string token, all;
  // 文字単位で読み、区切りごとにトークン確定
  char ch;
  auto flush_token = [&]() {
    TrimInPlace(token);
    if (!token.empty())
      out.emplace_back(token);
    token.clear();
  };

  while (ifs.get(ch)) {
    const bool is_delim = ch == ',' || ch == ';' || ch == '\n' || ch == '\r' ||
                          std::isspace(static_cast<unsigned char>(ch));
    if (is_delim) {
      flush_token();
    } else {
      token.push_back(ch);
    }
  }
  flush_token();

  return !out.empty();
}

// ==============================
//  表示ユーティリティ
// ==============================
static void PrintList(const std::vector<std::string> &v, const char *title) {
  std::cout << "===== " << title << " =====\n";
  int i = 1;
  for (const auto &s : v) {
    const auto k = ParseKey(s);
    if (k.valid) {
      std::cout << i++ << ". " << s << "   (k" << k.year << ", g" << k.num
                << ")\n";
    } else {
      std::cout << i++ << ". " << s << "   (no student key)\n";
    }
  }
  std::cout << "\n";
}

int main(int argc, char *argv[]) {
  // 入力ファイル名
  const std::string path = (argc >= 2) ? argv[1] : "PG3_2025_01_02.txt";

  // 読み込み
  std::vector<std::string> links;
  if (!LoadLinks(path, links)) {
    std::cerr << "[Error] ファイルを開けないか、データが空です: " << path
              << "\n"
              << "        例) " << (argc >= 1 ? argv[0] : "app")
              << " PG3_2025_01_02.txt\n";
    return 1;
  }

  // 生データ表示
  PrintList(links, "Loaded (raw)");

  // 学籍番号で昇順ソート
  std::sort(links.begin(), links.end(), LessByStuId);
  PrintList(links, "Sorted by student-id (k### -> g####)");

  // 最終表
  std::cout << "No, student-id, email\n";
  int no = 1;
  for (const auto &s : links) {
    const auto k = ParseKey(s);
    if (k.valid) {
      std::cout << no++ << ", k" << k.year << "g" << k.num << ", " << s << "\n";
    } else {
      std::cout << no++ << ", (no-key), " << s << "\n";
    }
  }
  return 0;
}
