// Complexitate: O(B D^2) per interogare, unde B=10, D=18.
#include <algorithm>
#include <stdio.h>

const int MAX_DIGITS = 19;

typedef unsigned long long u64;
typedef unsigned __int128 u128;

u64 p10[MAX_DIGITS];

struct number {
  u64 x;
  unsigned char len;

  number() { }

  number(u64 x) {
    this->x = x;

    len = 0;
    while (x) {
      len++;
      x /= 10;
    }
  }

  void append(int digit) {
    x = x * 10 + digit;
    len++;
  }

  void times10() {
    x *= 10;
    len++;
  }

  void divide10() {
    x /= 10;
    len--;
  }

  int get_kth_digit_from_left(int k) {
    return x / p10[len - 1 - k] % 10;
  }

  // Returnează true dacă și numai dacă aparținem zonei lui z. Exemplu: 523
  // este în zona lui 5235235235 pentru că, dacă îl extindem ciclic pe 523 la
  // 10 cifre, obținem 5235235235. Putem răspunde în O(1) dacă reformulăm
  // întrebarea: verifică dacă primele 7 cifre ale lui 5235235235 corespund cu
  // ultimele 7 cifre.
  bool in_zone_of(number z) {
    return z.x / p10[len] == z.x % p10[z.len - len];
  }

  // 44644 vine înaintea lui 446444, dar 442444 vine înaintea lui 44244.
  bool before(number other) {
    u128 me_first = (u128)x * p10[other.len] + other.x;
    u128 other_first = (u128)other.x * p10[len] + x;
    return (me_first > other_first) ||
      ((me_first == other_first) && (len > other.len));
  }
};

number a, b;

void compute_powers() {
  p10[0] = 1;
  for (int i = 1; i < MAX_DIGITS; i++) {
    p10[i] = p10[i - 1] * 10;
  }
}

u64 min(u64 x, u64 y) {
  return (x < y) ? x : y;
}

// Returnează lungimea totală a prefixelor lui p din zona lui p. De exemplu,
// pentru p=2323, returnează 2 (pentru numărul 23).
u64 len_prefix_in_prefix(number p, number b) {
  int count = 0;

  number prefix = p;
  do {
    prefix.divide10();
  } while (prefix.x > b.x);

  while (prefix.len) {
    if (prefix.in_zone_of(p)) {
      count += prefix.len;
    }
    prefix.divide10();
  }

  return count;
}

// Returnează suma lungimilor zonelor x ≤ b care încep cu prefix. Include și
// prefixele lui prefix din aceeași zonă.
u64 len_prefix_up_to(number prefix, number b) {
  u64 result = len_prefix_in_prefix(prefix, b);
  u64 p = 1;

  while (prefix.x <= b.x) {
    // prefix = 123, b = 523847 --> 1000 variante de lungime 6, dar
    // prefix = 523, b = 523847 -->  848 variante de lungime 6
    result += min(p, b.x - prefix.x + 1) * prefix.len;
    prefix.times10();
    p *= 10;
  }

  return result;
}

// Returnează suma lungimilor zonelor a ≤ x ≤ b care încep cu prefix.
u64 len_prefix(number prefix) {
  return len_prefix_up_to(prefix, b) - len_prefix_up_to(prefix, a);
}

// Returnează a k-a cifră a concatenării numerelor din v.
int get_digit_from_set(number* v, u64 k) {
  int i = 0;
  while (k >= v[i].len) {
    k -= v[i].len;
    i++;
  }

  return v[i].get_kth_digit_from_left(k);
}

// Dată fiind o zonă ca {232323, 2323, 23}, returnează a k-a cifră a
// concatenării. Ordinea poate fi complexă. De exemplu, pentru n=894848948,
// prefixul 89484894 îl precede pe n, dar prefixul 89484 îl urmează. De aceea,
// sortăm numerele naiv.
int get_digit_from_zone(number n, u64 k) {
  number v[MAX_DIGITS];
  number prefix = n;
  int num_numbers = 0;

  // Colectează prefixele.
  do {
    if (prefix.in_zone_of(n)) {
      v[num_numbers++] = prefix;
    }
    prefix.divide10();
  } while (prefix.x > a.x);

  std::sort(v, v + num_numbers, [](number x, number y) {
    return x.before(y);
  });

  return get_digit_from_set(v, k);
}

// Umple o zonă de b.len cifre. Procedează de la stînga la dreapta. Primele
// întrebări sînt: care este lungimea totală a zonelor care încep cu 9, 8, 7
// ș.a.m.d.? Aceasta identifică prima cifră. Dacă, de exemplu, prima cifră
// este 5, următoarele întrebări sînt: care este lungimea totală a zonelor
// care încep cu 59, 58, 57 ș.a.m.d.?
int get_digit_at(u64 pos) {
  number prefix(0);
  while (prefix.x * 10 <= b.x) {
    prefix.append(9);
    u64 count;
    while ((count = len_prefix(prefix)) <= pos) {
      pos -= count;
      prefix.x--;
    }
  }

  return get_digit_from_zone(prefix, pos);
}

int main() {
  compute_powers();

  int num_queries;
  u64 _a, _b;

  scanf("%llu %llu %d", &_a, &_b, &num_queries);
  _a--; // operăm pe (a, b]
  a = number(_a);
  b = number(_b);

  while (num_queries--) {
    u64 pos;
    scanf("%llu", &pos);
    putchar('0' + get_digit_at(pos));
  }
  putchar('\n');

  return 0;
}
