// Complexitate: O(N + Q).
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_Q = 1'000'000;
const int MAX_DEQUES = MAX_N / 2; // deoarece K >= 2

char q[MAX_Q + 1];
int n, k, d, num_outer, num_queries;

int buf[2 * MAX_N];
int buf_ptr; // primul element disponibil în buf[]

// Un vector circular de exact end-start întregi, alocat în buf[].
struct deque {
  int start;
  int end;
  int head; // poziția primului element

  // Inițializează vectorul cu progresia first + i*step (size termeni).
  void init_progression(int size, int first, int step) {
    start = head = buf_ptr;
    buf_ptr += size;
    end = buf_ptr;

    for (int i = 0; i < size; i++) {
      buf[start + i] = first + i * step;
    }
  }

  // Adaugă x la început. Șterge și returnează ultimul element.
  int shift(int x) {
    head = (head == start) ? (end - 1) : (head - 1);
    int result = buf[head];
    buf[head] = x;
    return result;
  }

  int get(int pos) {
    pos += head;
    if (pos >= end) {
      pos -= (end - start);
    }
    return buf[pos];
  }
};

struct circuit {
  deque mesh[MAX_DEQUES];
  int mesh_head; // indicele primei liste din rețea
  int last_node;
  deque outer;

  void rotate(circuit& other) {
    // Rotește rețeaua.
    mesh_head = mesh_head ? (mesh_head - 1) : (d - 1);

    // Mută ultimul nod la începutul lanțului exterior. Obține de la acesta
    // noul prim nod.
    int first_node = outer.shift(last_node);

    // Mută primul nod la începutul primei liste din rețea. Obține de la
    // aceasta noul ultim nod.
    last_node = mesh[mesh_head].shift(first_node);

    // Notifică celălalt circuit că prima sa listă este totuna cu prima
    // noastră listă.
    other.mesh[other.mesh_head] = mesh[mesh_head];
    other.last_node = last_node;
  }

  // Enumeră conținutul: listele din rețea interclasate, apoi lanțul exterior.
  long long hash_code() {
    long long result = 0, pos = 0;

    int h = mesh_head;
    for (int i = 0; i < k - 1; i++) {
      for (int j = 0; j < d; j++) {
        result += pos++ * mesh[h].get(i);
        h = (h + 1) % d;
      }
    }
    result += pos++ * last_node;

    for (int i = 0; i < num_outer; i++) {
      result += pos++ * outer.get(i);
    }

    return result;
  }
};

circuit a, b;

void make_circuits() {
  scanf("%d %d %d %d %s", &n, &k, &d, &num_queries, q);

  num_outer = n - (k - 1) * d - 1;

  for (int i = 0; i < d; i++) {
    a.mesh[i].init_progression(k - 1, i, d);
  }
  a.last_node = (k - 1) * d;
  a.outer.init_progression(num_outer, (k - 1) * d + 1, 1);

  b.mesh[0] = a.mesh[0];
  for (int i = 1; i < d; i++) {
    b.mesh[i].init_progression(k - 1, n + i - 1, d - 1);
  }
  b.last_node = a.last_node;
  b.outer.init_progression(num_outer, n + (k - 1) * (d - 1), 1);
}

void make_moves() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i] == 'R') {
      a.rotate(b);
    } else {
      b.rotate(a);
    }
  }
}

void write_hash_codes() {
  printf("%lld\n", a.hash_code());
  printf("%lld\n", b.hash_code());
}

int main() {
  make_circuits();
  make_moves();
  write_hash_codes();

  return 0;
}
