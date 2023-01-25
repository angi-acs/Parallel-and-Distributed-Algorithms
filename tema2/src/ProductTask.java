import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.util.TreeMap;
import java.util.concurrent.Callable;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class ProductTask implements Callable<Boolean> {
    AtomicInteger productsTotal;
    ExecutorService productsPool;
    String productsIn;
    String orderId;
    int searchedProduct;
    BufferedWriter productsWriter;

    public ProductTask(AtomicInteger productsTotal, ExecutorService productsPool, String productsIn,
                        String orderId, int searchedProduct, BufferedWriter productsWriter) {
        this.productsTotal = productsTotal;
        this.productsPool = productsPool;
        this.productsIn = productsIn;
        this.orderId = orderId;
        this.searchedProduct = searchedProduct;
        this.productsWriter = productsWriter;
    }

    @Override
    public Boolean call() throws Exception {
        BufferedReader productsReader = new BufferedReader(new FileReader(productsIn));
        String line, order;
        int idx = 0;

        while (productsReader.ready()) {
            line = productsReader.readLine();
            order = getOrderId(line);

            if (order.equals(orderId)) {
                if (idx == searchedProduct) {
                    productsWriter.write(line + ",shipped\n");
                    productsTotal.decrementAndGet();
                    return Boolean.TRUE;
                }
                idx++;
            }
        }

        productsTotal.decrementAndGet();
        return Boolean.FALSE;
    }

    private String getOrderId(String line) {
        String[] lineArray = line.split(",");
        return lineArray[0];
    }
}
