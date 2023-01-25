public class Ex1 extends Thread {
    int id;

    public Ex1(int id) {
        this.id = id;
    }

    public void run() {
        System.out.println("Hello world from thread " + id);
    }

    static class Main {

        public static void main(String[] args) {
            int cores = Runtime.getRuntime().availableProcessors();
            Thread[] t = new Thread[cores];

            for (int i = 0; i < cores; i++) {
                t[i] = new Ex1(i);
                t[i].start();
            }

            for (int i = 0; i < cores; i++) {
                try {
                    t[i].join();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
        }
    }
}
