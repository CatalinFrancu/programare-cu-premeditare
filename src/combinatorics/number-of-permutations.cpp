#include <algorithm>
#include <stdio.h>

const int MAX_N = 300'000;
const int MOD = 998'244'353;

struct pair {
  int a, b;
};

typedef bool (*cmpFunc)(pair, pair);
typedef bool (*equalsFunc)(pair, pair);

pair v[MAX_N + 1];
int fact[MAX_N + 1];
int n;

bool cmpA(pair p, pair q) {
  return p.a < q.a;
}

bool cmpB(pair p, pair q) {
  return p.b < q.b;
}

bool cmpBoth(pair p, pair q) {
  return (p.a < q.a) || ((p.a == q.a) && (p.b < q.b));
}

bool equalsA(pair p, pair q) {
  return p.a == q.a;
}

bool equalsB(pair p, pair q) {
  return p.b == q.b;
}

bool equalsBoth(pair p, pair q) {
  return (p.a == q.a) && (p.b == q.b);
}

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &v[i].a, &v[i].b);
  }
  v[n].a = v[n].b = n + 1; // santinele infinite
}

void compute_fact() {
  fact[0] = 1;
  for (int i = 1; i <= n; i++) {
    fact[i] = (long long)i * fact[i - 1] % MOD;
  }
}

int count(cmpFunc cmp, equalsFunc equals) {
  std::sort(v, v + n, cmp);

  long long result = 1;
  int run = 1;
  for (int i = 1; i <= n; i++) {
    if (equals(v[i], v[i - 1])) {
      run++;
    } else {
      result = result * fact[run] % MOD;
      run = 1;
    }
  }

  return result;
}

bool is_sorted_by_b() {
  for (int i = 1; i < n; i++) {
    if (v[i].b < v[i - 1].b) {
      return false;
    }
  }
  return true;
}

// Dintre cele n! permutări, scade-le pe cele sortate după a sau după b și
// adună-le la loc pe cele sortate după a și după b.
int inclusion_exclusion() {
  long long result = (long long)fact[n]
    + (MOD - count(cmpA, equalsA))
    + (MOD - count(cmpB, equalsB));

  int both = count(cmpBoth, equalsBoth);
  if (is_sorted_by_b()) {
    result += both;
  }
  return result % MOD;
}

int main() {
  read_data();
  compute_fact();
  int result = inclusion_exclusion();
  printf("%d\n", result);

  return 0;
}
