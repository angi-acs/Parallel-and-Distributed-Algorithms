package multipleProducersMultipleConsumers;

/**
 * DO NOT MODIFY
 */
public class Producer implements Runnable {
    private final Buffer buffer;
    private final int id;

    public Producer(Buffer buffer, int id) {
        this.buffer = buffer;
        this.id = id;
    }

    @Override
    public void run() {
        for (int i = 0; i < Main.N; i++) {
            try {
                buffer.put(i);
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        System.out.println("Producer " + id + " finished Correctly");
    }

}
