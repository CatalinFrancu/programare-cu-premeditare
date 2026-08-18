#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int MAX_T = 100'000;

bool used[MAX_T + 1];
int n, q, max_p, max_t;

void read_command_line_args(int argc, char** argv) {
  if (argc != 5) {
    fprintf(stderr, "Usage: gen <n> <q> <max_p> <max_t>\n");
    exit(1);
  }

  n = atoi(argv[1]);
  q = atoi(argv[2]);
  max_p = atoi(argv[3]);
  max_t = atoi(argv[4]);
}

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

int rand2(int min, int max) {
  return min + rand() % (max - min + 1);
}

void generate_p() {
  printf("%d %d\n", n, q);
  for (int i = 0; i < n; i++) {
    printf("%d ", rand2(1, max_p));
  }
  printf("\n");
}

void generate_t() {
  for (int i = 0; i < n; i++) {
    int t;
    do {
      t = rand2(1, max_t);
    } while (used[t]);
    printf("%d ", t);
    used[t] = true;
  }
  printf("\n");
}

void generate_queries() {
  for (int i = 0; i < q; i++) {
    int l = rand2(1, n);
    int r = rand2(1, n);
    if (l > r) {
      int tmp = l;
      l = r;
      r = tmp;
    }
    printf("%d %d\n", l, r);
  }
}

int main(int argc, char** argv) {
  read_command_line_args(argc, argv);
  init_rng();

  generate_p();
  generate_t();
  generate_queries();

  return 0;
}
