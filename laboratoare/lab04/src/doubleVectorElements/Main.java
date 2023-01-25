package doubleVectorElements;

public class Main {

    public static void main(String[] args) {
        int N = 100000013;
        int[] v = new int[N];
        int P = 4; // the program should work for any P <= N

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        // Parallelize me using P threads
//        for (int i = 0; i < N; i++) {
//            v[i] = v[i] * 2;
//        }

        Thread[] t = new Thread[P];

        for (int i = 0; i < P; i++) {
            t[i] = new Task(i, N, P, v);
            t[i].start();
        }

        for (int i = 0; i < P; i++) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

            for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }

    static class Task extends Thread {
        int id, N, P;
        int[] v;

        public Task(int id, int N, int P, int[] v) {
            this.id = id;
            this.N = N;
            this.P = P;
            this.v = v;
        }

        public void run() {
            int start = id * N / P;
            int end =  (id+1) * N / P;
            if (N < end) {
                end = N;
            }

            for (int i = start; i < end; i++) {
                v[i] *= 2;
            }
        }
    }

}
