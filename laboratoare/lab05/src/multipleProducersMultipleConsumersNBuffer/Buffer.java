package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;

public class Buffer {
    
    Queue<Integer> queue;
    int limit;
    
    public Buffer(int size) {
        queue = new LimitedQueue<>(size);
        limit = size;
    }

	public void put(int value) throws InterruptedException {
        synchronized (this) {
            while (queue.size() == limit) {
                wait();
            }
            queue.add(value);
            notifyAll();
        }
	}

	public int get() throws InterruptedException {
        synchronized (this) {
            int a = -1;
            if (queue.isEmpty()) {
                wait();
            }
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
            notifyAll();
            return a;
        }
	}
}
