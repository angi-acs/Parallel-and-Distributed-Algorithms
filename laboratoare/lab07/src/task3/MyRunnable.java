package task3;

import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    ExecutorService tpe;
    AtomicInteger inQueue;
    int[] graph;
    int step;

    public MyRunnable(ExecutorService tpe, AtomicInteger inQueue, int[] graph, int step) {
        this.tpe = tpe;
        this.inQueue = inQueue;
        this.graph = graph;
        this.step = step;
    }


    @Override
    public void run() {
        if (Main.N == step) {
            Main.printQueens(graph);
        } else {
            for (int i = 0; i < Main.N; ++i) {
                int[] newGraph = graph.clone();
                newGraph[step] = i;

                if (Main.check(newGraph, step)) {
                    inQueue.incrementAndGet();
                    tpe.submit(new MyRunnable(tpe, inQueue, newGraph, step + 1));
                }
            }
        }
        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
