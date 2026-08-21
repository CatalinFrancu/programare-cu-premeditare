#include <stdio.h>

const int MAX_N = 1 << 18;
const int MAX_SIGMA = 18;

char s[MAX_N + 1];
bool bad[1 << MAX_SIGMA];
int f[MAX_SIGMA]; // frecvențe în fereastra glisantă
int n, sigma, max_word_length;

void read_data() {
  scanf("%d %d %d ", &n, &sigma, &max_word_length);
  for (int i = 0; i < n; i++) {
    s[i] = getchar() - 'A';
  }
}

void reset() {
  for (int i = 0; i < sigma; i++) {
    f[i] = 0;
  }

  for (int i = 0; i < (1 << sigma); i++) {
    bad[i] = false;
  }
}

int complement(int mask) {
  return ((1 << sigma) - 1) ^ mask;
}

void include(int& mask, int letter) {
  if (++f[letter] == 1) {
    mask ^= (1 << letter);
  }
}

void exclude(int& mask, int letter) {
  if (--f[letter] == 0) {
    mask ^= (1 << letter);
  }
}

// Glisează o fereastră de lungime max_word_length. Menține masca ferestrei
// curente. Complementul ferestrei este garantat rău, întrucît fereastra
// (max_word_length caractere consecutive) nu include cazuri.
void init_bad_masks() {
  int mask = 0;

  for (int i = 0; i < max_word_length - 1; i++) {
    include(mask, s[i]);
  }

  for (int i = max_word_length - 1; i < n; i++) {
    include(mask, s[i]);
    bad[complement(mask)] = true;
    exclude(mask, s[i - max_word_length + 1]);
  }

  int end_mask = 1 << s[n - 1];
  bad[complement(end_mask)] = true;
}

// Dacă o mască este rea, atunci toate submăștile ei sînt rele.
void propagate_bad_masks() {
  for (int mask = (1 << sigma) - 1; mask; mask--) {
    if (bad[mask]) {
      for (int bit = 0; bit < sigma; bit++) {
        if (mask & (1 << bit)) {
          bad[mask ^ (1 << bit)] = true;
        }
      }
    }
  }
}

int get_min_pop_good_mask() {
  int min = sigma;

  for (int mask = 0; mask < (1 << sigma); mask++) {
    if (!bad[mask]) {
      int pop = __builtin_popcount(mask);
      if (pop < min) {
        min = pop;
      }
    }
  }

  return min;
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  while(num_tests--) {
    read_data();
    reset();
    init_bad_masks();
    propagate_bad_masks();
    int answer = get_min_pop_good_mask();
    printf("%d\n", answer);
  }

  return 0;
}
