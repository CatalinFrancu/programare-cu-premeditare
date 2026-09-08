// Complexitate: O((n + q) log n).
#include <map>
#include <stdio.h>
#include <stdlib.h>

typedef unsigned long long u64;

const int MAX_N = 50'000;
const int BASE = 60'133;

struct student {
  int score;
  int time;

  friend bool operator< (student a, student b) {
    return (a.score > b.score) ||
      ((a.score == b.score) & (a.time < b.time));
  }
};

student s[MAX_N];
u64 power[MAX_N + 1];
int n, num_ops;

struct node {
  int id, pri;
  int cnt;
  u64 hash_code;
  int l, r;
};

struct treap {
  node v[MAX_N + 1];
  int n, root;

  void init(int _n) {
    n = 1; // Începem de la 1 deoarece 0 înseamnă NULL.

    for (int i = 0; i < _n; i++) {
      v[n] = { .id = i, .pri = rand(), .l = 0, .r = 0 };
      insert(root, n++);
    }
  }

  void update_node(int t) {
    if (t) {
      node &l = v[v[t].l];
      node &r = v[v[t].r]; // syntactic sugar
      v[t].cnt = 1 + l.cnt + r.cnt;
      v[t].hash_code =
        l.hash_code * power[1 + r.cnt] +
        v[t].id * power[r.cnt] +
        r.hash_code;
    }
  }

  // Sparge subarborele lui t în studenți ≤ id și > id. Pune rădăcinile în l
  // și r.
  void split(int t, int id, int& l, int& r) {
    if (!t) {
      l = r = 0;
    } else if (s[id] < s[v[t].id]) {
      split(v[t].l, id, l, v[t].l);
      r = t;
      update_node(t);
    } else {
      split(v[t].r, id, v[t].r, r);
      l = t;
      update_node(t);
    }
  }

  // Unifică subarborii lui l și r și pune rădăcina rezultată în t.
  void merge(int& t, int l, int r) {
    if (!l) {
      t = r;
    } else if (!r) {
      t = l;
    } else if (v[l].pri > v[r].pri) {
      merge(v[l].r, v[l].r, r);
      t = l;
    } else {
      merge(v[r].l, l, v[r].l);
      t = r;
    }
    update_node(t);
  }

  // Inserează nodul elemn în subarborele lui t și pune noua rădăcină în t.
  void insert(int& t, int elem) {
    if (!t) {
      t = elem;
    } else if (v[elem].pri > v[t].pri) {
      split(t, v[elem].id, v[elem].l, v[elem].r);
      t = elem;
    } else {
      insert(s[v[elem].id] < s[v[t].id] ? v[t].l : v[t].r, elem);
    }
    update_node(t);
  }

  // Șterge studentul id din subarborele lui t și pune noua rădăcină în t.
  // Returnează indicele nodului șters.
  int erase(int& t, int id) {
    int result;

    if (v[t].id == id) {
      result = t;
      merge(t, v[t].l, v[t].r);
    } else {
      result = erase(s[id] < s[v[t].id] ? v[t].l : v[t].r, id);
    }

    update_node(t);
    return result;
  }

  void update_score(int id, int score, int time) {
    int z = erase(root, id);
    s[id] = { score, time };
    v[z].l = v[z].r = 0;
    insert(root, z);
  }

  u64 get_hash_code() {
    return v[root].hash_code;
  }
};

treap t;
FILE *fin, *fout;

void read_scores() {
  fscanf(fin, "%d %d", &n, &num_ops);
  for (int i = 0; i < n; i++) {
    fscanf(fin, "%d", &s[i].score);
  }
}

void compute_powers() {
  power[0] = 1;
  for (int i = 1; i <= n; i++) {
    power[i] = power[i - 1] * BASE;
  }
}

void process_ops() {
  t.init(n);

  std::map<u64, int> seen;
  seen[t.get_hash_code()] = 1;

  for (int time = 1; time <= num_ops; time++) {
    int i, score;
    fscanf(fin, "%d %d", &i, &score);
    i--;
    t.update_score(i, score, time);
    fprintf(fout, "%d\n", seen[t.get_hash_code()]);
    seen[t.get_hash_code()]++;
  }
}

int main() {
  fin = fopen("clasament.in", "r");
  fout = fopen("clasament.out", "w");

  read_scores();
  compute_powers();
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
