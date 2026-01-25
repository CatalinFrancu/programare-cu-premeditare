#include <stdio.h>

const int MAX_N = 300'000;
const int MOD = 998'244'353;

char s[MAX_N];
int n, num_ops;
long long answer;

void extended_euclid_iterative(int a, int b, int& d, int& x, int& y) {
  x = 1;
  y = 0;
  int xp = 0, yp = 1;
  while (b) {
    int q = a / b;
    int tmp = b; b = a - q * b; a = tmp;
    tmp = xp; xp = x - q * xp; x = tmp;
    tmp = yp; yp = y - q * yp; y = tmp;
  }
  d = a;
}

int inverse(int x) {
  int y, k, d;
  extended_euclid_iterative(x, MOD, d, y, k);
  return (y >= 0) ? y : (y + MOD);
}

void read_data() {
  scanf("%d %d %s", &n, &num_ops, s);
}

void compute_first_answer() {
  answer = 1;
  for (int i = 1; i < n - 1; i++) {
    if (s[i] == '?') {
      answer = answer * i % MOD;
    }
  }
}

void update_string() {
  int pos;
  char c;
  scanf("%d %c", &pos, &c);
  pos--;

  if (s[pos] != c) { // optimizare de viteză
    if ((pos > 0) && (s[pos] == '?')) {
      answer = answer * inverse(pos) % MOD;
    }
    s[pos] = c;
    if ((pos > 0) && (s[pos] == '?')) {
      answer = answer * pos % MOD;
    }
  }
}

void write_answer() {
  int real_answer = (s[0] == '?') ? 0 : answer;
  printf("%d\n", real_answer);
}

int main() {
  read_data();
  compute_first_answer();
  write_answer();

  while (num_ops--) {
    update_string();
    write_answer();
  }

  return 0;
}
