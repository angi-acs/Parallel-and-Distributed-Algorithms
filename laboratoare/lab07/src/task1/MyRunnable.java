package task1;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    ExecutorService tpe;
    ArrayList<Integer> partialPath;
    AtomicInteger inQueue;
    int destination;

    public MyRunnable(ExecutorService tpe, ArrayList<Integer> partialPath,
                      int destination, AtomicInteger inQueue) {
        this.tpe = tpe;
        this.partialPath = partialPath;
        this.inQueue = inQueue;
        this.destination = destination;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
        }

        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                inQueue.incrementAndGet();
                tpe.submit(new task1.MyRunnable(tpe, newPartialPath,3, inQueue));
            }
        }
        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
