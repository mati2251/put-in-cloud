#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

std::mutex print_mutex;

class NaiveBarrier {
public:
  NaiveBarrier(int num_threads) : n(num_threads), count(0) {}

  void wait() {
    int current = count.fetch_add(1, std::memory_order_acq_rel);
    while (count.load(std::memory_order_acquire) < n) {
      std::this_thread::yield();
    }
    if (current == n - 1) {
      {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "All threads reached the barrier.\n";
      }
      count.store(0, std::memory_order_relaxed);
    }
  }

private:
  const int n;
  std::atomic<int> count;
};

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " <number_of_threads>\n";
    return 1;
  }
  int num_threads = std::stoi(argv[1]);
  NaiveBarrier barrier(num_threads);

  auto task = [&](int id) {
    for (int i = 0; i < 5; ++i) {
      {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Thread " << id << " before barrier\n";
      }

      int delay = rand() % 100;
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      barrier.wait();

      {
        std::lock_guard<std::mutex> lock(print_mutex);
        std::cout << "Thread " << id << " after barrier\n";
      }
    }
  };

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i)
    threads.emplace_back(task, i);

  for (auto &t : threads)
    t.join();

  return 0;
}
