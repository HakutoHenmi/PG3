#include <stdio.h>
#include <type_traits> // std::common_type_t

template <class T1, class T2> class MinClass {
public:
  // 2つの引数の小さい方を返す
  std::common_type_t<T1, T2> Min(T1 a, T2 b) const {
    using R = std::common_type_t<T1, T2>;
    R ra = static_cast<R>(a);
    R rb = static_cast<R>(b);
    return (ra < rb) ? ra : rb;
  }
};

int main(void) {

  MinClass<int, int> m_ii;
  MinClass<float, float> m_ff;
  MinClass<double, double> m_dd;

  // float-double）
  MinClass<int, float> m_if;
  MinClass<int, double> m_id;
  MinClass<float, double> m_fd;

  // 呼び出し
  printf("Min<int, int>       : %g\n", (double)m_ii.Min(10, 3));
  printf("Min<float, float>   : %g\n", (double)m_ff.Min(2.5f, 7.25f));
  printf("Min<double, double> : %g\n", (double)m_dd.Min(9.0, 4.125));

  printf("Min<int, float>     : %g\n", (double)m_if.Min(8, 3.75f));
  printf("Min<int, double>    : %g\n", (double)m_id.Min(6, 2.125));
  printf("Min<float, double>  : %g\n", (double)m_fd.Min(5.5f, 1.25));

  return 0;
}
