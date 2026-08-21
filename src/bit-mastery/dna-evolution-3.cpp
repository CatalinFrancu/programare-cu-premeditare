#include <stdio.h>
#include <string.h>

const int MAX_LEN = 100'000;
const int BLOCK_SIZE = 1'112;
const int MAX_BLOCKS = (MAX_LEN - 1) / BLOCK_SIZE + 1;
const int LATIN_SIGMA = 26;
const int SIGMA = 4;
const int MAX_Q_LEN = 10;
const int OP_UPDATE = 1;
const int OP_QUERY = 2;

// Un vector de doi biți pe element.
struct packed_array {
  unsigned v[MAX_LEN / 16];

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

packed_array s;
char next[MAX_Q_LEN][MAX_Q_LEN + 1];
char next_bl[MAX_Q_LEN][MAX_Q_LEN + 1];
char letter_map[LATIN_SIGMA];
int n;

struct block_info {
  int start;
  short cnt[SIGMA][MAX_Q_LEN * (MAX_Q_LEN + 1) / 2];

  void init(int start, int end) {
    this->start = start;
    for (int pos = start; pos < end; pos++) {
      update(s.get(pos), pos, +1);
    }
  }

  void update(int c, int pos, int val) {
    for (int mod = 1; mod <= MAX_Q_LEN; mod++) {
      int gauss = mod * (mod - 1) / 2;
      cnt[c][gauss + (pos - start) % mod] += val;
    }
  }

  int query(char* q, int qlen, int rem) {
    int gauss = qlen * (qlen - 1) / 2;
    int result = 0;
    for (int i = 0; i < qlen; i++) {
      // Numără aparițiile lui q[rem] la poziții egale cu i modulo qlen.
      result += cnt[(int)q[rem]][gauss + i];
      rem = next[rem][qlen];
    }
    return result;
  }
};

block_info block[MAX_BLOCKS];

void precompute_letter_map() {
  letter_map['A' - 'A'] = 0;
  letter_map['C' - 'A'] = 1;
  letter_map['G' - 'A'] = 2;
  letter_map['T' - 'A'] = 3;
}

int translate_char(char c) {
  return letter_map[c - 'A'];
}

void translate_string(char* s) {
  for (char* c = s; *c; c++) {
    *c = translate_char(*c);
  }
}

void read_string() {
  char c;
  while ((c = getchar()) != '\n') {
    s.set(n++, translate_char(c));
  }
}

void precompute_next() {
  for (int mod = 1; mod <= MAX_Q_LEN; mod++) {
    for (int i = 0; i < mod; i++) {
      next[i][mod] = (i + 1) % mod;
      next_bl[i][mod] = (i + BLOCK_SIZE) % mod;
    }
  }
}

int min(int x, int y) {
  return (x < y) ? x : y;
}

void init_blocks() {
  int num_blocks = (n - 1) / BLOCK_SIZE + 1;
  for (int b = 0; b < num_blocks; b++) {
    int start = b * BLOCK_SIZE;
    int end = min(start + BLOCK_SIZE, n);
    block[b].init(start, end);
  }
}

void update(int pos, int c) {
  int b = pos / BLOCK_SIZE;
  block[b].update(s.get(pos), pos, -1);
  s.set(pos, c);
  block[b].update(c, pos, +1);
}

int naive_count(int l, int r, char* q, int qlen, int& rem) {
  int result = 0;
  while (l < r) {
    result += (s.get(l) == q[rem]);
    rem = next[rem][qlen];
    l++;
  }
  return result;
}

int block_sum(int bl, int br, char* q, int qlen, int& rem) {
  int result = 0;
  while (bl < br) {
    result += block[bl].query(q, qlen, rem);
    rem = next_bl[rem][qlen];
    bl++;
  }
  return result;
}

int query(int l, int r, char* q, int qlen) { // [l inclusiv, r exclusiv)
  int bl = l / BLOCK_SIZE;
  int br = r / BLOCK_SIZE;
  int rem = 0;

  if (bl == br) {
    return naive_count(l, r, q, qlen, rem);
  } else {
    int result = naive_count(l, (bl + 1) * BLOCK_SIZE, q, qlen, rem);
    result += block_sum(bl + 1, br, q, qlen, rem);
    result += naive_count(br * BLOCK_SIZE, r, q, qlen, rem);
    return result;
  }
}

void process_ops() {
  int num_ops, type;
  scanf("%d", &num_ops);
  while (num_ops--) {
    scanf("%d", &type);
    if (type == OP_UPDATE) {
      int x;
      char c;
      scanf("%d %c", &x, &c);
      update(x - 1, translate_char(c));
    } else {
      int l, r, qlen;
      char q[MAX_Q_LEN + 1];
      scanf("%d %d %s", &l, &r, q);
      qlen = strlen(q);
      translate_string(q);
      int answer = query(l - 1, r, q, qlen);
      printf("%d\n", answer);
    }
  }
}

int main() {
  precompute_letter_map();
  precompute_next();
  read_string();
  init_blocks();
  process_ops();

  return 0;
}
