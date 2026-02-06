#include <algorithm>
#include <math.h>
#include <stdio.h>

const int MAX_N = 100;
const int MAX_EDGES = MAX_N * (MAX_N - 1) / 2;

struct point {
  int x, y;
};

struct triangle {
  point a, b, c;
};

struct edge {
  int u, v;
  double d;
};

triangle t[MAX_N];
edge e[MAX_EDGES];
int comp[MAX_N];
int n, num_edges, k, task, num_selected_edges, num_connected_pairs;
double total_length;

void read_data() {
  FILE* f = fopen("elicoptere.in", "r");
  fscanf(f, "%d %d %d", &task, &n, &k);
  for (int i = 0; i < n; i++) {
    fscanf(f, "%d %d %d %d %d %d",
           &t[i].a.x, &t[i].a.y, &t[i].b.x, &t[i].b.y, &t[i].c.x, &t[i].c.y);
  }
  fclose(f);
}

double min(double a, double b) {
  return (a < b) ? a : b;
}

double min3(double a, double b, double c) {
  return min(min(a, b), c);
}

double dist_horiz(point p, point a, point b) {
  if ((a.y == p.y) && (p.y == b.y)) {
    // a, b, p coliniare pe orizontală
    return min(fabs(p.x - a.x), fabs(p.x - b.x));
  } else if ((long long)(p.y - a.y) * (p.y - b.y) <= 0) {
    // p.y se află între a.y și b.y
    double x = a.x + (double)(p.y - a.y) * (b.x - a.x) / (b.y - a.y);
    return fabs(p.x - x);
  } else {
    // orizontala din p *nu* întîlnește ab; returnează infinit
    return k + 1;
  }
}

double dist_point_segment(point p, point a, point b) {
  // Pentru a afla distanța pe verticală, oglindim punctele față de diagonală.
  return min(dist_horiz(p, a, b),
             dist_horiz({p.y, p.x}, {a.y, a.x}, {b.y, b.x}));
}

double dist_point_triangle(point p, triangle t) {
  return min3(dist_point_segment(p, t.a, t.b),
              dist_point_segment(p, t.b, t.c),
              dist_point_segment(p, t.c, t.a));
}

double dist_triangle_triangle(triangle& g, triangle& h) {
  return min(min3(dist_point_triangle(g.a, h),
                  dist_point_triangle(g.b, h),
                  dist_point_triangle(g.c, h)),
             min3(dist_point_triangle(h.a, g),
                  dist_point_triangle(h.b, g),
                  dist_point_triangle(h.c, g)));
}

void create_edges() {
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      double d = dist_triangle_triangle(t[i], t[j]);
      if (d <= k) {
        e[num_edges++] = { i, j, d };
      }
    }
  }
}

void init_comp() {
  for (int i = 0; i < n; i++) {
    comp[i] = i;
  }
}

void unite(int u, int v) {
  for (int i = 0; i < n; i++) {
    if (comp[i] == u) {
      comp[i] = v;
    }
  }
}

void kruskal() {
  std::sort(e, e + num_edges, [](edge a, edge b) {
    return a.d < b.d;
  });

  init_comp();

  for (int i = 0; i < num_edges; i++) {
    if (comp[e[i].u] != comp[e[i].v]) {
      unite(comp[e[i].u], comp[e[i].v]);
      num_selected_edges++;
      total_length += e[i].d;
    }
  }
}

void write_answer() {
  FILE* f = fopen("elicoptere.out", "w");
  if (task == 1) {
    fprintf(f, "%d\n", num_selected_edges);
  } else if (task == 2) {
    fprintf(f, "%d\n", num_connected_pairs);
  } else {
    fprintf(f, "%0.6lf\n", total_length);
  }
  fclose(f);
}

void count_connected_pairs() {
  for (int i = 0; i < n; i++) {
    for (int j = i + 1; j < n; j++) {
      num_connected_pairs += (comp[i] == comp[j]);
    }
  }
}

int main() {
  read_data();
  create_edges();
  kruskal();
  count_connected_pairs();
  write_answer();
}
