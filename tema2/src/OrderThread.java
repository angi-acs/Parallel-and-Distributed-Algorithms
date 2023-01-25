import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Future;
import java.util.concurrent.atomic.AtomicInteger;

public class OrderThread extends Thread {
    // For OrderThread
    BufferedReader ordersReader;
    BufferedWriter ordersWriter;

    // For ProductTask
    AtomicInteger productsTotal;
    ExecutorService productsPool;
    String productsIn;
    BufferedWriter productsWriter;

    public OrderThread(BufferedReader ordersReader, BufferedWriter ordersWriter, AtomicInteger productsTotal,
                       ExecutorService productsPool, String productsIn, BufferedWriter productsWriter) {
        this.ordersReader = ordersReader;
        this.ordersWriter = ordersWriter;
        this.productsTotal = productsTotal;
        this.productsPool = productsPool;
        this.productsIn = productsIn;
        this.productsWriter = productsWriter;
    }

    public void run() {
        while (true) {
            try {
                if (!ordersReader.ready()) break;
            } catch (IOException e) {
                e.printStackTrace();
            }
            String line = null;
            try {
                line = ordersReader.readLine();
            } catch (IOException e) {
                e.printStackTrace();
            }
            assert line != null;
            String orderId = getOrderId(line);
            int productsNo = getProductsNo(line);

            // Create product tasks
            List<Boolean> shippedProducts = new ArrayList<>();
            for (int i = 0; i < productsNo; i++) {
                productsTotal.incrementAndGet();
                Future<Boolean> status =
                        productsPool.submit(new ProductTask(productsTotal, productsPool, productsIn,
                                orderId, i, productsWriter));
                try {
                    shippedProducts.add(status.get());
                } catch (InterruptedException | ExecutionException e) {
                    e.printStackTrace();
                }
            }

            // If every product is shipped, then the order is shipped
            if (productsNo != 0 && !shippedProducts.contains(Boolean.FALSE)) {
                try {
                    ordersWriter.write(line + ",shipped\n");
                } catch (IOException e) {
                    e.printStackTrace();
                }
            }

            int left = productsTotal.get();
            if (left == 0) {
                try {
                    productsWriter.close();
                } catch (IOException e) {
                    e.printStackTrace();
                }
                productsPool.shutdown();
            }
        }
    }

    private String getOrderId(String line) {
        String[] lineArray = line.split(",");
        return lineArray[0];
    }

    private int getProductsNo(String line) {
        String[] lineArray = line.split(",");
        return Integer.parseInt(lineArray[1]);
    }
}
