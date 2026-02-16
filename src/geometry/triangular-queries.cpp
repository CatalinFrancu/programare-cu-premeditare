#include <algorithm>
#include <stdio.h>

const int MAX_N = 300000;
const int MAX_Q = 200000;
const int MAX_COORD = 300000;

// Ordinea este importanta.  Punctul trebuie adăugat *după* ce procesăm orice
// bază care îl conține, ca sa nu îl scădem. În schimb, punctul trebuie
// adăugat *înainte* să procesăm orice vîrf de triunghi cu care se suprapune,
// ca să îl luăm în calcul.
const int T_BASE = 0;
const int T_POINT = 1;
const int T_TIP = 2;

struct point {
  int x, y;
};

struct triangle {
  int x, y, d;
  int answer;
};

struct event {
  int diag;
  int obj_id;
  unsigned char type;
};

struct fenwick {
  int v[MAX_COORD + 1];

  void inc(int pos) {
    do {
      v[pos]++;
      pos += pos & -pos;
    } while (pos <= MAX_COORD);
  }

  int partial_sum(int pos) {
    int sum = 0;
    while (pos) {
      sum += v[pos];
      pos &= pos - 1;
    }
    return sum;
  }
};

point p[MAX_N];
triangle t[MAX_Q];
event e[MAX_N + 2 * MAX_Q];
fenwick fx, fy;
int num_points, num_triangles, num_events;
int points_seen;

void read_data() {
  scanf("%d %d", &num_points, &num_triangles);
  for (int i = 0; i < num_points; i++) {
    scanf("%d %d", &p[i].x, &p[i].y);
  }
  for (int i = 0; i < num_triangles; i++) {
    scanf("%d %d %d", &t[i].x, &t[i].y, &t[i].d);
  }
}

void create_events() {
  for (int i = 0; i < num_points; i++) {
    e[num_events++] = {
      .diag = p[i].x + p[i].y,
      .obj_id = i,
      .type = T_POINT,
    };
  }
  for (int i = 0; i < num_triangles; i++) {
    e[num_events++] = {
      .diag = t[i].x + t[i].y + t[i].d,
      .obj_id = i,
      .type = T_BASE,
    };
    e[num_events++] = {
      .diag = t[i].x + t[i].y,
      .obj_id = i,
      .type = T_TIP,
    };
  }
}

void sort_events() {
  std::sort(e, e + num_events, [](event a, event b) {
    return (a.diag > b.diag) ||
      ((a.diag == b.diag) && (a.type < b.type));
  });
}

void base_event(int id) {
  t[id].answer = points_seen
    - fx.partial_sum(t[id].x - 1)
    - fy.partial_sum(t[id].y - 1);
}

void point_event(int id) {
  fx.inc(p[id].x);
  fy.inc(p[id].y);
  points_seen++;
}

void tip_event(int id) {
  int total = points_seen
    - fx.partial_sum(t[id].x - 1)
    - fy.partial_sum(t[id].y - 1);
  t[id].answer = total - t[id].answer;
}

void process_events() {
  for (int i = 0; i < num_events; i++) {
    switch (e[i].type) {
      case T_BASE: base_event(e[i].obj_id); break;
      case T_POINT: point_event(e[i].obj_id); break;
      case T_TIP: tip_event(e[i].obj_id); break;
    }
  }
}

void write_answers() {
  for (int i = 0; i < num_triangles; i++) {
    printf("%d\n", t[i].answer);
  }
}

int main() {
  read_data();
  create_events();
  sort_events();
  process_events();
  write_answers();

  return 0;
}
