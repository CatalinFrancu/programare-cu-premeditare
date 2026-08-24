#include <stdio.h>

const int MAX_VAL = 100'000;
const int MOD = 1'000'000'007;
const int MAX_ROOT = 512;
const int ROOT_BITS = 9;

int spf[MAX_VAL + 1];
int mobius[MAX_VAL + 1];
int freq[MAX_VAL + 1];

// pow2: 2^0, 2^1, 2^2, ..., 2^511
// pow2_large: 2^0, 2^512, 2^1024, 2^1536, ...
int pow2[MAX_ROOT], pow2_large[MAX_ROOT];

void read_data() {
  int n, x;
  scanf("%d", &n);
  while (n--) {
    scanf("%d", &x);
    freq[x]++;
  }
}

void precompute_powers_of_2() {
  pow2[0] = 1;
  for (int i = 1; i < MAX_ROOT; i++) {
    pow2[i] = pow2[i - 1] * 2 % MOD;
  }

  pow2_large[0] = 1;
  pow2_large[1] = (2 * pow2[MAX_ROOT - 1]) % MOD;
  for (int i = 2; i < MAX_ROOT; i++) {
    pow2_large[i] = (long long)pow2_large[i - 1] * pow2_large[1] % MOD;
  }
}

int power_of_2(int e) {
  long long large = pow2_large[e >> ROOT_BITS];
  long long small = pow2[e & (MAX_ROOT - 1)];
  return large * small % MOD;
}

void compute_smallest_factors() {
  spf[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (!spf[i])  {
      for (int j = i; j <= MAX_VAL; j += i) {
        if (!spf[j]) {
          spf[j] = i;
        }
      }
    }
  }
}

void compute_mobius() {
  mobius[1] = 1;
  for (int i = 2; i <= MAX_VAL; i++) {
    if (spf[i] == spf[i / spf[i]]) {
      mobius[i] = 0;
    } else {
      mobius[i] = -mobius[i / spf[i]];
    }
  }
}

int count_multiples(int d) {
  int result = 0;
  for (int i = d; i <= MAX_VAL; i += d) {
    result += freq[i];
  }
  return result;
}

int count_coprime_subsets() {
  long long result = 0;
  for (int d = 1; d <= MAX_VAL; d++) {
    int num_mult = count_multiples(d);
    result += MOD + mobius[d] * (power_of_2(num_mult) - 1);
  }
  return result % MOD;
}

void write_answer(int answer) {
  printf("%d\n", answer);
}

int main() {
  read_data();
  precompute_powers_of_2();
  compute_smallest_factors();
  compute_mobius();
  int answer = count_coprime_subsets();
  write_answer(answer);

  return 0;
}
