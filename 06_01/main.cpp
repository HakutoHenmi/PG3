#include <atomic>
#include <iostream>
#include <thread>

int main() {
  std::atomic<int> turn{1}; // 1→2→3 の順で実行させる

  auto worker = [&](int id) {
    // sleep / wait を使わず、順番になるまで回す
    while (turn.load(std::memory_order_acquire) != id) {
      std::this_thread::yield();
    }

    std::cout << "thread" << id << std::endl;

    turn.store(id + 1, std::memory_order_release);
  };

  std::thread t1(worker, 1);
  std::thread t2(worker, 2);
  std::thread t3(worker, 3);

  t1.join();
  t2.join();
  t3.join();

  return 0;
}
