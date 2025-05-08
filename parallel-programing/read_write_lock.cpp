#include <algorithm>
#include <atomic>
#include <chrono>
#include <iostream>
#include <mutex>
#include <random>
#include <thread>
#include <vector>

class rwlock {
public:
  std::atomic<int> n{0};

  static constexpr int aw = 1;
  static constexpr int rc = 2;

  static constexpr int base_delay_ms = 1;
  static constexpr int max_delay_ms = 1000;
  static constexpr int delay_multiplier = 2;

  void writer_acquire() {
    int delay = base_delay_ms;
    int zero = 0;

    while (!n.compare_exchange_strong(zero, aw, std::memory_order_seq_cst)) {
      zero = 0;
      std::this_thread::sleep_for(std::chrono::milliseconds(delay));
      delay = std::min(delay * delay_multiplier, max_delay_ms);
    }
  }

  void writer_release() { n.fetch_add(-aw, std::memory_order_acq_rel); }

  void reader_acquire() {
    n.fetch_add(rc, std::memory_order_acquire);
    while ((n.load(std::memory_order_seq_cst) & aw) == aw) {
      std::this_thread::yield();
    }
    std::atomic_thread_fence(std::memory_order_acquire);
  }

  void reader_release() { n.fetch_add(-rc, std::memory_order_acquire); }

  int reader_count() {
    int count = n.load(std::memory_order_seq_cst);
    return (count & ~aw) / rc;
  }

  int writer_count() {
    int count = n.load(std::memory_order_seq_cst);
    return (count & aw) / aw;
  }
};

rwlock lock_instance;
int shared_data = 0;
std::mutex print_mutex;

void random_delay(int max_ms = 100) {
  thread_local static std::mt19937 gen{std::random_device{}()};
  std::uniform_int_distribution<> distrib(0, max_ms);
  int delay = distrib(gen);
  if (delay > 0) {
    std::this_thread::sleep_for(std::chrono::milliseconds(delay));
  }
}

void reader_thread_func(int id, int cycles = 5) {
  random_delay(200);
  for (int i = 0; i < cycles; ++i) {
    lock_instance.reader_acquire();
    {
      std::lock_guard<std::mutex> guard(print_mutex);
      std::cout << "Reader " << id << ": " << shared_data
                << " (active readers: " << lock_instance.reader_count()
                << ", active writers: " << lock_instance.writer_count() << ")"
                << std::endl;
      if (lock_instance.writer_count() > 0) {
        std::cerr << "READER ERROR" << std::endl;
      }
    }
    random_delay(50);
    lock_instance.reader_release();
    random_delay(50);
  }
}

void writer_thread_func(int id, int cycles = 3) {
  // random_delay(50);
  for (int i = 0; i < cycles; ++i) {
    lock_instance.writer_acquire();
    shared_data++;
    {
      std::lock_guard<std::mutex> guard(print_mutex);
      std::cout << "Writer " << id << ": " << shared_data
                << " (active readers: " << lock_instance.reader_count() << ")"
                << std::endl;
      if (lock_instance.reader_count() > 0) {
        std::cerr << "WRITER ERROR" << std::endl;
      }
    }
    random_delay(50);
    lock_instance.writer_release();
    random_delay(50);
  }
}

int main(int argc, char *argv[]) {
  int writer_count = 5;
  int reader_count = 5;
  int writer_cycles = 3;
  int reader_cycles = 5;
  if (argc > 4) {
    writer_count = std::stoi(argv[1]);
    reader_count = std::stoi(argv[2]);
    writer_cycles = std::stoi(argv[3]);
    reader_cycles = std::stoi(argv[4]);
  }
  std::vector<std::thread> threads;

  for (int i = 0; i < writer_count; ++i) {
    threads.emplace_back(writer_thread_func, i + 1, writer_cycles);
  }

  for (int i = 0; i < reader_count; ++i) {
    threads.emplace_back(reader_thread_func, i + 1, reader_cycles);
  }

  for (auto &t : threads) {
    t.join();
  }

  {
    std::lock_guard<std::mutex> guard(print_mutex);
    std::cout << "End value: " << shared_data << std::endl;
  }

  return 0;
}
