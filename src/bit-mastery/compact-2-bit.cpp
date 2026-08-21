#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int ARRAY_SIZE = 1'000'000;
const int NUM_CHANGES = 100'000;
const int NUM_PASSES = 10'000;

struct change {
  int pos;
  char val, prev_val;
};

struct packed_array {
  unsigned v[ARRAY_SIZE / 16];

  void set(int pos, int val) {
    int word = pos / 16;
    int offset = pos % 16 * 2;
    v[word] = (v[word] & ~(3 << offset)) | (val << offset);
  }

  int get(int pos) {
    int word = pos / 16;
    int offset = pos % 16 * 2;
    return (v[word] >> offset) & 3;
  }
};

change c[NUM_CHANGES];
char v[ARRAY_SIZE];
packed_array packed;
long long t0;

void init_rng() {
  struct timeval time;
  gettimeofday(&time, NULL);
  int seed = time.tv_sec * 1'000'000 + time.tv_usec;
  srand(seed);
}

void mark_time() {
  timeval tv;

  gettimeofday (&tv, NULL);
  t0 = 1000LL * tv.tv_sec + tv.tv_usec / 1000;
}

void report_time(const char* msg) {
  timeval tv;

  gettimeofday (&tv, NULL);
  long long t = 1000LL * tv.tv_sec + tv.tv_usec / 1000;

  int millis = t - t0;
  int ops = NUM_CHANGES * NUM_PASSES * 2; // am măsurat citiri + scrieri

  printf("%s: %lld ms for %d ops (%d Mops/sec)\n", msg, t - t0,
         ops, ops / millis / 1000);
}

void gen_locations() {
  for (int i = 0; i < NUM_CHANGES; i++) {
    c[i].pos = rand() % (ARRAY_SIZE - NUM_PASSES);
    c[i].val = rand() % 4;
  }
}

void run_packed() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = packed.get(pos);
      packed.set(pos, c[i].val);
    }
  }
}

void run_naive() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = v[pos];
      v[pos] = c[i].val;
    }
  }
}

void verify_packed() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (v[i] != packed.get(i)) {
      printf("Error: position %d, naive is %d, packed is %d.\n",
             i, v[i], packed.get(i));
      exit(1);
    }
  }
}

int main(void) {
  init_rng();
  gen_locations();

  mark_time();
  run_naive();
  report_time("naive");

  mark_time();
  run_packed();
  report_time("packed array, 32 bit");
  verify_packed();

  return 0;
}
