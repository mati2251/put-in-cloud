import java.lang.Thread;

class CounterImpl implements Counter {
  private int value = 0;

  public CounterImpl() {
    super();
  }

  @Override
  public int increment(int value) throws InterruptedException {
    int oldValue = this.value;
    Thread.sleep(2000);
    this.value = oldValue + value;
    return this.value;
  }

  @Override
  public synchronized int decrement(int value) throws InterruptedException {
    int oldValue = this.value;
    Thread.sleep(2000);
    this.value = oldValue - value;
    return this.value;
  }
}
