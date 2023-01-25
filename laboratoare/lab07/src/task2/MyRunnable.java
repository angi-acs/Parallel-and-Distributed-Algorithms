package task2;

import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    ExecutorService tpe;
    AtomicInteger inQueue;
    int[] colors;
    int step;

    public MyRunnable(ExecutorService tpe, AtomicInteger inQueue, int[] colors, int step) {
        this.tpe = tpe;
        this.inQueue = inQueue;
        this.colors = colors;
        this.step = step;
    }

    @Override
    public void run() {
        if (step == Main.N) {
            Main.printColors(colors);
        } else {
            for (int i = 0; i < Main.COLORS; i++) {
                int[] newColors = colors.clone();
                newColors[step] = i;
                if (Main.verifyColors(newColors, step)) {
                    inQueue.incrementAndGet();
                    tpe.submit(new MyRunnable(tpe, inQueue, newColors, step + 1));
                }
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
