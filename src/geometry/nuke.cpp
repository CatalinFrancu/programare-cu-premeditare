#include <algorithm>
#include <map>
#include <stdio.h>

const int MAX_POINTS = 200'000;
const int MAX_CIRCLES = 100'000;
const int MAX_EVENTS = MAX_POINTS + 2 * MAX_CIRCLES;

const int T_BEGIN_CIRCLE = 0;
const int T_POINT = 1;
const int T_END_CIRCLE = 2;

struct point {
  int x, y;
};

struct circle {
  int x, y, r, pop;

  void update_pop(point p) {
    long long dx2 = (long long)(p.x - x) * (p.x - x);
    long long dy2 = (long long)(p.y - y) * (p.y - y);
    long long r2 = (long long) r * r;
    pop += (dx2 + dy2 <= r2);
  }
};

struct event {
  int x, type, obj_id;
};

point p[MAX_POINTS];
circle c[MAX_CIRCLES];
event evt[MAX_EVENTS];
std::map<int, int> circle_ys;
int n, m, num_evt;

void read_data() {
  FILE* f = fopen("nuke.in", "r");
  fscanf(f, "%d %d", &n, &m);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
  }
  for (int i = 0; i < m; i++) {
    fscanf(f, "%d %d %d", &c[i].x, &c[i].y, &c[i].r);
  }
  fclose(f);
}

void create_events() {
  for (int i = 0; i < n; i++) {
    evt[num_evt++] = { p[i].x, T_POINT, i };
  }
  for (int i = 0; i < m; i++) {
    evt[num_evt++] = { c[i].x - c[i].r, T_BEGIN_CIRCLE, i };
    evt[num_evt++] = { c[i].x + c[i].r, T_END_CIRCLE, i };
  }
}

void sort_events() {
  std::sort(evt, evt + num_evt, [](event& a, event& b) {
    return (a.x < b.x) ||
      ((a.x == b.x) && (a.type < b.type));
  });
}

void begin_circle(int id) {
  circle_ys[c[id].y] = id;
}

void end_circle(int id) {
  circle_ys.erase(c[id].y);
}

void process_point(point p) {
  auto it = circle_ys.lower_bound(p.y);
  if (it != circle_ys.end()) {
    c[it->second].update_pop(p);
  }
  if (it != circle_ys.begin()) {
    c[(--it)->second].update_pop(p);
  }
}

void sweep_line() {
  for (int i = 0; i < num_evt; i++) {
    int id = evt[i].obj_id;
    switch (evt[i].type) {
      case T_BEGIN_CIRCLE: begin_circle(id); break;
      case T_END_CIRCLE: end_circle(id); break;
      case T_POINT: process_point(p[id]); break;
    }
  }
}

void write_answers() {
  FILE* f = fopen("nuke.out", "w");
  for (int i = 0; i < m; i++) {
    fprintf(f, "%d\n", c[i].pop);
  }
  fclose(f);
}

int main() {
  read_data();
  create_events();
  sort_events();
  sweep_line();
  write_answers();

  return 0;
}
