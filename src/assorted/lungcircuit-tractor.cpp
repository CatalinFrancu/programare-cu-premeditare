// Complexitate: O((N + Q) log Q).
#include <stdio.h>

const int MAX_N = 1'000'000;
const int MAX_Q = 1'000'000;
const int MAX_LOG = 20;
const bool LEFT = false;
const bool RIGHT = true;

// moment[i] reține informații despre situația înainte de procesarea mutării
// #i (0 ≤ i < q).
struct moment {
  // mutarea (LEFT/RIGHT)
  bool move;

  // A cîta mutare de tipul său (R/A) este aceasta, numărînd de la sfîrșit?
  // Dacă mutarea este R, garantează că occurrence[LEFT][ord] = acest moment.
  // Similar pentru A.
  int ord;

  // Momentul următoarei mutări opuse (R dacă mutarea curentă este A și
  // viceversa).
  int next_opp;

  // Considerînd bila aflată în nodul de sus, la ce moment va încheia ea
  // ultima tură completă și va reveni în nodul de sus? Stochează chiar
  // momentul curent dacă bila nu poate descrie nicio tură completă.
  int full_laps;
};

int n, k, d, q;
moment m[MAX_Q + 1];
int sol[2][MAX_N];

// pozițiile mutărilor R/A, descrescător de la q-1 la 0.
int occurrence[2][MAX_Q];

// st[i][t] = momentul la care ajungem dacă parcurgem 2^i intervale începînd
// de la momentul t.
int st[MAX_LOG][MAX_Q + 1];

int min(int x, int y) {
  return (x < y) ? x : y;
}

void read_data() {
  scanf("%d %d %d %d ", &n, &k, &d, &q);
  for (int i = 0; i < q; i++) {
    m[i].move = (getchar() == 'R') ? LEFT : RIGHT;
  }
}

void make_occurrences() {
  m[q].next_opp = q;
  int next[2] = { q, q };
  int ptr[2] = { 0, 0 };

  for (int i = q - 1; i >= 0; i--) {
    bool side = m[i].move;
    m[i].ord = ptr[side];
    m[i].next_opp = next[!side];
    occurrence[side][ptr[side]++] = i;
    next[side] = i;
  }
}

void make_sparse_table() {
  for (int t = 0; t < q; t++) {
    if (m[t].ord < d - 1) {
      st[0][t] = 0;
    } else {
      bool side = m[t].move;
      st[0][t] = 1 + occurrence[side][m[t].ord - d + 1];
    }
  }
  st[0][q] = 0;

  for (int i = 1; i < MAX_LOG; i++) {
    for (int t = 0; t < q; t++) {
      int mid = st[i - 1][t];
      st[i][t] = mid ? st[i - 1][mid] : 0;
    }
  }
}

// Informații despre o bilă aflată pe circuitul side la poziția pos după
// primele time mutări.
struct state {
  int time;
  bool side;
  int pos;

  bool is_node() {
    return (pos <= (k - 1) * d) && !(pos % d);
  }

  bool is_between_nodes() {
    return (pos < (k - 1) * d) && (pos % d);
  }

  // Face cel mult atîția pași pe o porțiune care poate începe dintr-un nod,
  // dar nu conține alte noduri.
  void move_at_most(int steps) {
    if (is_node()) {
      // Nodurile sînt pe ambele circuite. Partea pe care o avem notată nu
      // este relevantă. Alege o parte în funcție de mutarea dată.
      side = m[time].move;
    } else if (side != m[time].move) {
      time = m[time].next_opp; // Sari peste mutările circuitului opus.
    }

    if (time < q) {
      steps = min(steps, m[time].ord + 1);
      pos = (pos + steps) % n;
      int last_ord = m[time].ord - (steps - 1);
      time = 1 + occurrence[side][last_ord];
    }
  }

  void move_to_next_node() {
    if (is_between_nodes()) {
      move_at_most(d - pos % d);
    }
  }

  void move_to_last_node() {
    if (is_node()) {
      int intervals = (k - 1) - (pos / d);
      for (int i = MAX_LOG - 1; i >= 0; i--) {
        if (((1 << i) <= intervals) && st[i][time]) {
          time = st[i][time];
          pos += d * (1 << i);
          intervals -= (1 << i);
        }
      }
    }
  }

  void move_to_first_node() {
    if (pos >= (k - 1) * d) {
      move_at_most(n - pos);
    }
  }

  void move_full_laps() {
    if (pos == 0) {
      time = m[time].full_laps;
    }
  }

  void mark_solution(int ball_number) {
    sol[side][pos] = ball_number;
    if (is_node()) {
      sol[!side][pos] = ball_number;
    }
  }

  void compute_trajectory(int ball_number) {
    move_to_next_node();
    move_to_last_node();
    move_to_first_node();
    move_full_laps();
    move_to_last_node();
    move_at_most(q); // Știm că restul drumului nu va mai întîlni noduri.
    mark_solution(ball_number);
  }
};

void compute_full_laps() {
  m[q].full_laps = q;

  for (int t = q - 1; t >= 0; t--) {
    state s = { t, LEFT, 0 };
    s.move_to_last_node();
    s.move_to_first_node();

    m[t].full_laps = (s.pos == 0) && (s.time > t)
      ? m[s.time].full_laps
      : t;      // Odată plecați din t, nu mai revenim niciodată.
  }
}

void compute_trajectories() {
  // În stînga, bilele [0, n) stau pe pozițiile [0, n).
  for (int i = 0; i < n; i++) {
    state s = { 0, LEFT, i };
    s.compute_trajectory(i);
  }

  // În dreapta, bilele [n, 2n-k) sar peste noduri.
  int cnt = n;
  for (int i = 0; i < n; i++) {
    state s = { 0, RIGHT, i };
    if (!s.is_node()) {
      s.compute_trajectory(cnt++);
    }
  }
}

void write_solution() {
  long long hash_left = 0, hash_right = 0;
  for (long long i = 0; i < n; i++) {
    hash_left += i * sol[LEFT][i];
    hash_right += i * sol[RIGHT][i];
  }

  printf("%lld\n", hash_left);
  printf("%lld\n", hash_right);
}

int main() {
  read_data();
  make_occurrences();
  make_sparse_table();
  compute_full_laps();
  compute_trajectories();
  write_solution();

  return 0;
}
