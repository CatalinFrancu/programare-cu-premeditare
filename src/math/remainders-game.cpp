// Reimplementată după https://codeforces.com/contest/687/submission/18789803
#include <stdio.h>

int gcd(int a, int b) {
  while (b) {
    int tmp = a;
    a = b;
    b = tmp % b;
  }
  return a;
}

int lcm(int a, int b) {
  return a / gcd(a, b) * b;
}

int main() {
  int n, k, c;
  int minimax = 1;

  scanf("%d %d", &n, &k);
  while (n--) {
    scanf("%d", &c);
    minimax = lcm(minimax, gcd(k, c));
  }

  printf((minimax == k) ? "Yes\n" : "No\n");

  return 0;
}
