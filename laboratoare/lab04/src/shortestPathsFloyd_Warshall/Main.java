package shortestPathsFloyd_Warshall;

public class Main {

    public static void main(String[] args) {
        int M = 9;
        int[][] graph = {{0, 1, M, M, M},
                {1, 0, 1, M, M},
                {M, 1, 0, 1, 1},
                {M, M, 1, 0, M},
                {M, M, 1, M, 0}};

        // Parallelize me (You might want to keep the original code in order to compare)
//        for (int k = 0; k < 5; k++) {
//            for (int i = 0; i < 5; i++) {
//                for (int j = 0; j < 5; j++) {
//                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
//                }
//            }
//        }

        int cores = Runtime.getRuntime().availableProcessors();
        Thread[] t = new Thread[cores];

        for (int i = 0; i < cores; i++) {
            t[i] = new Task(i, 5, cores, graph);
            t[i].start();
        }

        for (int i = 0; i < cores; i++) {
            try {
                t[i].join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }
    }

    static class Task extends Thread {
        int id, N, P;
        int[][] graph;

        public Task(int id, int N, int P, int[][] graph) {
            this.id = id;
            this.N = N;
            this.P = P;
            this.graph = graph;
        }

        public void run() {
            int start = id * N / P;
            int end =  (id+1) * N / P;
            if (N < end) {
                end = N;
            }

            for (int k = 0; k < 5; k++) {
                for (int i = 0; i < 5; i++) {
                    for (int j = start; j < end; j++) {
                        graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
                    }
                }
            }
        }
    }
}
