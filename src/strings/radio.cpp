#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_M = 2'500;
const int BASE = 113;    // baza pentru hash
const int MOD = 1 << 20; // modulul pentru hash
const int MAX_COLLISIONS = 9; // determinat experimental
const int NONE = -1;

int n, m, l, k;
char s[MAX_N + 1], p[MAX_M + 1]; // șirul și cuvîntul curent
int min_match; // lungimea minimă garantată de căutat exact
int cur_word;    // operația curentă
long long base_pow; // BASE^{min_match - 1} % MOD, pentru Rabin-Karp
int cnt[MOD];       // numărul de apariții ale fiecărei valori hash
int where[MOD][MAX_COLLISIONS]; // pozițiile acestor apariții
short tried[MOD]; // cache cu pozițiile deja încercate pentru cuvîntul curent
FILE *fin, *fout;

// ⌈a/b⌉
int ceil(int a, int b) {
  return (a - 1) / b + 1;
}

void read_string() {
  fscanf(fin, "%d %d %d %d %s ", &n, &m, &l, &k, s);
  min_match = ceil(l - k, k + 1);
}

// Rabin-Karp (hash în fereastră glisantă) pe s.
void hash_string() {
  base_pow = 1;
  for (int i = 0; i < min_match - 1; i++) {
    base_pow = base_pow * BASE % MOD;
  }

  long long hash = 0;
  for (int i = 0; i < min_match - 1; i++) {
    hash = (hash * BASE + (s[i] - 'a')) % MOD;
  }
  for (int start = 0, end = min_match - 1; end < n; start++, end++) {
    hash = (hash * BASE + (s[end] - 'a')) % MOD;
    where[hash][cnt[hash]++] = start;
    hash = (hash + MOD - base_pow * (s[start] - 'a') % MOD) % MOD;
  }
}

// Numără naiv diferențele între s[start...] și p.
bool at_most_k_mismatches(int start) {
  if (tried[start] == cur_word) {
    return false;
  }
  tried[start] = cur_word;

  int num_diffs = 0;
  int i = start;
  while ((num_diffs <= k) && (i < start + l)) {
    num_diffs += (s[i] != p[i - start]);
    i++;
  }
  return (num_diffs <= k);
}

// Încearcă să suprapună p[pstart..pstart+min_match-1] peste s.
bool try_offset(int pstart, int hash) {
  for (int i = 0; i < cnt[hash]; i++) {
    int sstart = where[hash][i];
    // sstart trebuie să fie suficient de departe de marginile lui s pentru ca
    // p să poată fi suprapus corect (reminder: la sstart începe doar
    // fragmentul p[pstart...pstart-min_match+1]).
    if ((sstart >= pstart) &&
        (sstart <= n - l + pstart) &&
        at_most_k_mismatches(sstart - pstart)) {
      return true;
    }
  }

  return false;
}

// Rabin-Karp peste cuvîntul curent. Caută în s fiecare fereastră a lui p de
// mărime min_match.
bool query() {
  long long hash = 0;
  for (int i = 0; i < min_match - 1; i++) {
    hash = (hash * BASE + (p[i] - 'a')) % MOD;
  }

  bool found = false;
  int start = 0, end = min_match - 1;
  while (!found && (end < l)) {
    hash = (hash * BASE + (p[end] - 'a')) % MOD;
    found = try_offset(start, hash);
    hash = (hash + MOD - base_pow * (p[start] - 'a') % MOD) % MOD;
    start++;
    end++;
  }

  return found;
}

void process_queries() {
  for (cur_word = 1; cur_word <= m; cur_word++) {
    fscanf(fin, "%s ", p);
    fprintf(fout, "%d\n", query());
  }
}

int main() {
  fin = fopen("radio.in", "r");
  fout = fopen("radio.out", "w");

  read_string();
  hash_string();
  process_queries();

  fclose(fin);
  fclose(fout);
  return 0;
}
