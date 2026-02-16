#include <stdio.h>

const int MAX_WIZARDS = 200'000;

struct wizard {
  int x, e;
  int p, q;
};

wizard w[MAX_WIZARDS];
int st[MAX_WIZARDS], ss;
int n;

void read_input_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d", &w[i].x, &w[i].e);
  }
}

// Test standard de orientare trigonometrică.
bool is_reflex(int i, int j, int x, int e) {
  return
    (long long)(w[i].x - x) * (w[j].e - e) >
    (long long)(w[j].x - x) * (w[i].e - e);
}

// Elimină vrăjitorul anterior din stivă cîtă vreme vrăjitorul curent (A) este
// mai sus sau (B) face un unghi reflex cu cei doi vrăjitori anteriori.
void evict(int x, int e) {
  while ((ss && (e >= w[st[ss - 1]].e)) ||
         ((ss > 1) && is_reflex(st[ss - 2], st[ss - 1], x, e))) {
    ss--;
  }
}

// Vrăjitorul me are un nou mentor c. Vezi dacă este mai bun decît vechiul
// mentor.
void compare_mentors(int me, int c) {
  // Dacă q nu există, atunci evident alege p/q din c. Altfel compară vechea
  // și noua pantă.
  if (!w[me].q ||
      ((long long)w[c].e * w[me].q >
       (long long)w[me].p * (w[me].x - w[c].x))) {
    w[me].p = w[c].e;
    w[me].q = w[me].x - w[c].x;
  }
}

void iterate_wizards() {
  ss = 0; // mărimea stivei
  st[ss++] = 0;

  for (int i = 1; i < n; i++) {
    // Mai întîi privește de la nivelul solului și alege-ți un mentor.
    evict(w[i].x, 0);
    compare_mentors(i, st[ss - 1]);

    // Apoi privește mai de sus și continuă să elimini.
    evict(w[i].x, w[i].e);
    st[ss++] = i;
  }
}

// Oglindește coordonatele x. Astfel scăpăm de nevoia de a copia algoritmul cu
// stivă de la stînga la dreapta și de la dreapta la stînga.
void reverse_wizards() {
  int min_coord = w[0].x, max_coord = w[n - 1].x;

  for (int i = 0, j = n - 1; i < j; i++, j--) {
    wizard tmp = w[i];
    w[i] = w[j];
    w[j] = tmp;
  }

  for (int i = 0; i < n; i++) {
    w[i].x = min_coord + max_coord - w[i].x;
  }
}

void find_mentors() {
  iterate_wizards();
  reverse_wizards();
  iterate_wizards();
  reverse_wizards();
}

int gcd(int x, int y) {
  while (y) {
    int tmp = x;
    x = y;
    y = tmp % y;
  }
  return x;
}

void write_reduced_fractions() {
  for (int i = 0; i < n; i++) {
    int d = gcd(w[i].p, w[i].q);
    printf("%d %d\n", w[i].p / d, w[i].q / d);
  }
}

int main() {
  read_input_data();
  find_mentors();
  write_reduced_fractions();

  return 0;
}
