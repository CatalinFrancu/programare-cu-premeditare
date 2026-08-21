#include <algorithm>
#include <stdio.h>

const int MAX_QUESTS = 25;
const int MAX_SUBSETS = 2'125'764; // 3^13 + 3^12
const long long INFINITY = 300'000'000;
const int POW3[] = {
  1, 3, 9, 27, 81, 243, 729, 2187, 6561, 19683, 59049, 177147,
  531441, 1594323, 43046721,
};

const bool LEFT = false;
const bool RIGHT = true;

struct attitude {
  int a, b, c;
};

struct subset {
  int a, tritmask;
  long long fp; // fingerprint (amprentă)

  void set(int a, int b, int c, int tritmask, bool half) {
    this->a = a;
    this->tritmask = tritmask;

    // Amprenta este combinația lui (b-a) și (c-a), urmată de un bit care
    // indică jumătatea (stînga/dreapta). Pentru a grupa împreună submulțimile
    // care se potrivesc, negăm amprenta jumătății drepte.
    if (half == LEFT) {
      fp = (INFINITY * (b - a + INFINITY) + (c - a + INFINITY)) *  2;
    } else {
      fp = (INFINITY * (a - b + INFINITY) + (a - c + INFINITY)) *  2 + 1;
    }
  }
};

attitude att[MAX_QUESTS];
subset sub[MAX_SUBSETS];
int n, mid, num_subsets;

// O capsulă ca să nu facem variabilele from, to și half globale.
struct dfs_wrapper {
  int from, to;
  bool half;

  void run(int k, int tritmask, int a, int b, int c)  {
    if (k == to) {
      sub[num_subsets++].set(a, b, c, tritmask, half);
      return;
    }

    // a, b și respectiv c stau acasă
    run(k + 1, tritmask, a, b + att[k].b, c + att[k].c);
    run(k + 1, tritmask + POW3[k - from], a + att[k].a, b, c + att[k].c);
    run(k + 1, tritmask + 2 * POW3[k - from], a + att[k].a, b + att[k].b, c);
  }

  void master(int from, int to, int half) {
    this->from = from;
    this->to = to;
    this->half = half;

    run(from, 0, 0, 0, 0);
  }
};

dfs_wrapper dfs_wrap;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d", &att[i].a, &att[i].b, &att[i].c);
  }
  mid = n / 2;
}

void sort_subsets() {
  std::sort(sub, sub + num_subsets, [] (subset& p, subset& q) {
    if (p.fp != q.fp) {
      return p.fp < q.fp;
    } else if (p.fp & 1) {
      // jumătatea dreaptă
      return p.a > q.a;
    } else {
      // jumătatea stîngă
      return p.a < q.a;
    }
  });
}

long long find_match() {
  int best_a = -INFINITY;
  long long best_mask = -1;

  for (int i = 0; i < num_subsets - 1; i++) {
    if ((sub[i].fp != sub[i + 1].fp) &&
        (sub[i].fp / 2 == sub[i + 1].fp / 2) &&
        (sub[i].a + sub[i + 1].a > best_a)) {
      best_a = sub[i].a + sub[i + 1].a;
      best_mask = (long long)sub[i + 1].tritmask * POW3[mid] + sub[i].tritmask;
    }
  }

  return best_mask;
}

void print_solution(long long tritmask) {
  if (tritmask == -1) {
    printf("Impossible\n");
    return;
  }

  for (int i = 0; i < n; i++) {
    switch (tritmask % 3) {
      case 0:
        printf("MW\n");
        break;
      case 1:
        printf("LW\n");
        break;
      default:
        printf("LM\n");
    }
    tritmask /= 3;
  }
}

int main() {
  read_data();

  dfs_wrap.master(0, mid, LEFT);
  dfs_wrap.master(mid, n, RIGHT);
  sort_subsets();
  long long tritmask = find_match();
  print_solution(tritmask);

  return 0;
}
