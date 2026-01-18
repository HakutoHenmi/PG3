#include <stdio.h>

// 基底クラス
class Animal {
public:
  // 仮想関数
  virtual void Speak() { printf("動物が鳴きます。\n"); }

  // 仮想デストラクタ
  virtual ~Animal() {}
};

// 派生クラス：犬
class Dog : public Animal {
public:
  void Speak() override { printf("犬：ワンワン\n"); }
};

// 派生クラス：猫
class Cat : public Animal {
public:
  void Speak() override { printf("猫：ニャー\n"); }
};

int main(void) {
  // 基底クラスのポインタで派生クラスのインスタンスを扱う
  Animal *a1 = new Dog();
  Animal *a2 = new Cat();

  a1->Speak();
  a2->Speak();

  // 後始末
  delete a1;
  delete a2;

  return 0;
}
