package oneProducerOneConsumer;

public class Buffer {
    private int a = -1;

    void put(int value) throws InterruptedException {
        synchronized (this) {
            if (a != -1) {
                wait();
            }
            a = value;
            notify();
        }

    }

    int get() throws InterruptedException {
        synchronized (this) {
            if (a == -1) {
                wait();
            }
            notify();
            int b = a;
            a = -1;
            return b;
        }
    }
}
