#include <algorithm>
#include <stdio.h>
#include <string.h>

const int MAX_LEN = 300'000;
const int MAX_N = 300'000;
const int MOD = 1'000'000'007;

char buf[MAX_LEN + MAX_N]; // lasă loc pentru terminatorii de șir
char* s[MAX_N];            // cuvintele de la intrare

int f[MAX_LEN + 1];     // frecvențele lungimilor cuvintelor citite
int g[MAX_LEN + 1];     // frecvențele lungimilor cuvintelor de adăugat
int cnt[MAX_LEN + 1];   // numărul de subșiruri distincte de fiecare lungime
int n, sigma, max_extra_len;

void read_data() {
  int ptr = 0, num_extra, len;
  scanf("%d %d %d", &n, &num_extra, &sigma);

  for (int i = 0; i < n; i++) {
    s[i] = buf + ptr;
    scanf("%s", s[i]);
    ptr += strlen(s[i]) + 1; // include \0
  }

  while (num_extra--) {
    scanf("%d", &len);
    g[len]++;
    max_extra_len = std::max(len, max_extra_len);
  }
}

void tsort(int left, int right, int pos) {
  // Optimizare care previne recursivitatea pentru restul șirului.
  if (right - left == 1) {
    while (s[left][pos] != '\0') {
      cnt[++pos]++;
    }
    f[pos]++;
  }

  if (right - left <= 1) {
    return;
  }

  int i = left, lt = left, gt = right;
  char pivot = s[left][pos];

  while ((i < gt) && s[i][pos]) {
    if (s[i][pos] < pivot) {
      std::swap(s[i++], s[lt++]);
    } else if (s[i][pos] > pivot)  {
      std::swap(s[i], s[--gt]);
    } else {
      i++;
    }
  }

  if (i < gt) {
    // Am găsit un șir terminat aici. Restul șirurilor sînt continuări ale
    // sale și nu mai contează în statistici, deci nu mai reapelăm.
    f[pos]++;
  } else {
    cnt[pos + 1]++; // pentru zona din mijloc
    tsort(left, lt, pos);
    tsort(lt, gt, pos + 1);
    tsort(gt, right, pos);
  }
}

int scan_lengths() {
  long long result = 1, sigma_l = 1, c = 0;

  for (int l = 1; l <= max_extra_len; l++) {
    c = (c + f[l - 1] + g[l - 1]) * sigma % MOD;
    sigma_l = sigma_l * sigma % MOD;
    long long legal = sigma_l - c - cnt[l] + MOD;
    for (int j = 0; j < g[l]; j++) {
      result = result * (legal - j) % MOD;
    }
  }

  return result;
}

int main() {
  read_data();
  tsort(0, n, 0);
  int result = scan_lengths();
  printf("%d\n", result);

  return 0;
}
