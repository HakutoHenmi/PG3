#include <stdio.h>

// ------------------------------
// 抽象クラス
// ------------------------------
class IShape {
public:
  // 面積を返す
  virtual double Size() const = 0;

  // 図形を表示する
  virtual void Draw() const = 0;

  virtual ~IShape() {}
};

// ------------------------------
// Circle（円）
// ------------------------------
class Circle : public IShape {
public:
  explicit Circle(double r) : m_radius(r) {}

  double Size() const override {
    const double PI = 3.14159265358979323846;
    return PI * m_radius * m_radius;
  }

  void Draw() const override { printf("Circle (radius=%.2f)\n", m_radius); }

private:
  double m_radius;
};

// ------------------------------
// Rectangle（長方形）
// ------------------------------
class Rectangle : public IShape {
public:
  Rectangle(double w, double h) : m_width(w), m_height(h) {}

  double Size() const override { return m_width * m_height; }

  void Draw() const override {
    printf("Rectangle (width=%.2f, height=%.2f)\n", m_width, m_height);
  }

private:
  double m_width;
  double m_height;
};

int main(void) {
  // インスタンス生成
  Circle c(3.0);
  Rectangle r(4.0, 2.5);

  // 抽象クラス（IShape）でまとめて扱う
  IShape *shapes[] = {&c, &r};

  // 実行
  for (int i = 0; i < 2; ++i) {
    shapes[i]->Draw();
    printf("  area(size) = %.4f\n\n", shapes[i]->Size());
  }

  return 0;
}
