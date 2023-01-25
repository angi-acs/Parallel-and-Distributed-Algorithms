import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;
import java.util.concurrent.atomic.AtomicInteger;

public class Tema2 {

    public static void main(String[] args) throws IOException, InterruptedException {
        // Parse arguments
        String path = args[0];
        String ordersIn = path + "/orders.txt";
        String productsIn = path + "/order_products.txt";
        int threadsNo = Integer.parseInt(args[1]);
        Thread[] threads = new Thread[threadsNo];

        // Readers & Writers
        BufferedReader ordersReader = new BufferedReader(new FileReader(ordersIn));
        BufferedWriter ordersWriter = new BufferedWriter(new FileWriter("orders_out.txt"));
        BufferedWriter productsWriter = new BufferedWriter(new FileWriter("order_products_out.txt"));

        // Thread Pool
        AtomicInteger productsTotal = new AtomicInteger(0);
        ExecutorService productsPool = Executors.newFixedThreadPool(threadsNo);

        // Create level 1 threads
        for (int i = 0; i < threadsNo; i++) {
            threads[i] = new OrderThread(ordersReader, ordersWriter, productsTotal, productsPool,
                                            productsIn, productsWriter);
            threads[i].start();
        }

        for (int i = 0; i < threadsNo; i++) {
            threads[i].join();
        }
        ordersWriter.close();
    }
}
