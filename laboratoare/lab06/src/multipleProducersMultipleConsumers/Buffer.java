package multipleProducersMultipleConsumers;

import java.util.concurrent.ArrayBlockingQueue;

public class Buffer {
	int value;
	int capacity = 10;
	ArrayBlockingQueue<Integer> queue = new ArrayBlockingQueue<Integer>(capacity);

	void put(int value) throws InterruptedException {
		queue.put(value);
		//this.value = value;
	}

	int get() throws InterruptedException {
		return queue.take();
		//return value;
	}
}
