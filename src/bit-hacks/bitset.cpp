// Pregătește un vector de <poziție, valoare>. La fiecare trecere, shiftează
// pozițiile cu 1. Notează valorile anterioare. La final algoritmului, compară
// abordarea naivă cu abordarea compactată.
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

const int ARRAY_SIZE = 1'000'000;
const int NUM_CHANGES = 100'000;
const int NUM_PASSES = 10'000;

struct change {
  int pos;
  bool val, prev_val;
};

struct packed_32 {
  unsigned v[ARRAY_SIZE / 32];

  void set_branching(int pos, bool val) {
    unsigned& c = v[pos >> 5];
    unsigned mask = 1 << (pos & 31);

    if (val) {
      c |= mask;
    } else {
      c &= ~mask;
    }
  }

  void set(int pos, bool val) {
    unsigned& c = v[pos >> 5];
    int shift = pos & 31;

    // Prima jumătate curăță bitul, iar a doua îl scrie.
    c = (c & ~(1 << shift)) | (val << shift);
  }

  bool get(int pos) {
    return (v[pos >> 5] >> (pos & 31)) & 1;
  }
};

struct packed_8 {
  unsigned char v[ARRAY_SIZE / 8];

  void set(int pos, bool val) {
    unsigned char& c = v[pos >> 3];
    int shift = pos & 7;

    // Prima jumătate curăță bitul, iar a doua îl scrie.
    c = (c & ~(1 << shift)) | (val << shift);
  }

  bool get(int pos) {
    return (v[pos >> 3] >> (pos & 7)) & 1;
  }
};

change c[NUM_CHANGES];
bool v[ARRAY_SIZE];
std::bitset<ARRAY_SIZE> bs;
packed_32 p32;
packed_8 p8;
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
    c[i].val = rand() % 2;
  }
}

void run_p32_branching() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = p32.get(pos);
      p32.set_branching(pos, c[i].val);
    }
  }
}

void run_p32() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = p32.get(pos);
      p32.set(pos, c[i].val);
    }
  }
}

void run_p8() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = p8.get(pos);
      p8.set(pos, c[i].val);
    }
  }
}

void run_bitset() {
  for (int pass = 0; pass < NUM_PASSES; pass++) {
    for (int i = 0; i < NUM_CHANGES; i++) {
      int pos = c[i].pos + pass;
      c[i].prev_val = bs[pos];
      bs[pos] = c[i].val;
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

void verify_p32() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (v[i] != p32.get(i)) {
      printf("Error: position %d, naive is %d, p32 is %d.\n",
             i, v[i], p32.get(i));
      exit(1);
    }
  }
}

void verify_p8() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (v[i] != p8.get(i)) {
      printf("Error: position %d, naive is %d, p8 is %d.\n",
             i, v[i], p8.get(i));
      exit(1);
    }
  }
}

void verify_bitset() {
  for (int i = 0; i < ARRAY_SIZE; i++) {
    if (v[i] != bs[i]) {
      printf("Error: position %d, naive is %d, bitset is %d.\n",
             i, v[i], (int)bs[i]);
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
  run_p32_branching();
  report_time("packed array, 32 bit, branching");
  verify_p32();

  mark_time();
  run_p32();
  report_time("packed array, 32 bit");
  verify_p32();

  mark_time();
  run_p8();
  report_time("packed array, 8 bit");
  verify_p8();

  mark_time();
  run_bitset();
  report_time("STL bitset");
  verify_bitset();

  return 0;
}
