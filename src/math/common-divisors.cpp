#include <stdio.h>

const int MAX_VAL = 1'000'000;

int freq[MAX_VAL + 1];

void read_data() {
  int n, x;

  scanf("%d", &n);
  while (n--) {
    scanf("%d", &x);
    freq[x]++;
  }
}

int count_multiples(int div) {
  int result = 0;

  for (int mult = div; mult <= MAX_VAL; mult += div) {
    result += freq[mult];
  }

  return result;
}

int get_highest_gcd() {
  int div = MAX_VAL;
  while (count_multiples(div) < 2) {
    div--;
  }
  return div;
}

int main() {

  read_data();
  int answer = get_highest_gcd();
  printf("%d\n", answer);

  return 0;
}
