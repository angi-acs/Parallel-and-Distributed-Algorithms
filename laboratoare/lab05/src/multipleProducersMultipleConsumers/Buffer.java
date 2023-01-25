package multipleProducersMultipleConsumers;

public class Buffer {
    private int a = -1;

    void put(int value) throws InterruptedException {
        synchronized (this) {
            while (a != -1) {
                wait();
            }
            a = value;
            notifyAll();
        }

    }

    int get() throws InterruptedException {
        synchronized (this) {
            while (a == -1) {
                wait();
            }
            notifyAll();
            int b = a;
            a = -1;
            return b;
        }
    }
}
