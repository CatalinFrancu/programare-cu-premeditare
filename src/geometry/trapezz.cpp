#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

const int MAX_POINTS = 1'000;
const int MAX_SEGMENTS = MAX_POINTS * (MAX_POINTS - 1) / 2;

struct point {
  int x, y;
};

int gcd(int a, int b) {
  while (b) {
    int tmp = a % b;
    a = b;
    b = tmp;
  }
  return a;
}

struct line {
  int a, b, c;

  void normalize() {
    int g = gcd(abs(c), gcd(abs(a), abs(b)));
    if ((a < 0) || ((a == 0) & (b < 0))) {
      g = -g;
    }
    a /= g;
    b /= g;
    c /= g;
  }

  bool operator ==(line& other) {
    return (a == other.a) && (b == other.b) && (c == other.c);
  }
};

// Pentru segmente cu o mediatoare comună, teoretic trebuie să cunoaștem
// ecuațiile dreptelor. Dar nu ne trebuie ecuația completă -- știm deja că au
// aceleași valori a și b, deoarece sînt paralele. De aceea, stocăm doar
// valorile c, care denotă pozițiile segmentelor pe mediatoare.
struct segment {
  int len2; // lungimea la pătrat
  line bis; // mediatoarea
  int pos;  // poziția pe mediatoare

  void from_points(point p, point q) {
    len2 = (q.x - p.x) * (q.x - p.x) + (q.y - p.y) * (q.y - p.y);
    bis = {
      .a = q.x - p.x,
      .b = q.y - p.y,
      .c = (q.x * q.x - p.x * p.x + q.y * q.y - p.y * p.y) / 2,
    };
    bis.normalize();

    line l = {
      .a = q.y - p.y,
      .b = p.x - q.x,
      .c = p.y * (q.x - p.x) - p.x * (q.y - p.y),
    };
    l.normalize();
    pos = l.c;
  }
};

point p[MAX_POINTS];
segment s[MAX_SEGMENTS + 1];
int num_points, num_segments;

void read_data() {
  FILE* f = fopen("trapezz.in", "r");
  fscanf(f, "%d", &num_points);
  for (int i = 0; i < num_points; i++) {
    fscanf(f, "%d %d", &p[i].x, &p[i].y);
    p[i].x *= 2;
    p[i].y *= 2; // astfel ca toate mijloacele să aibă coordonate întregi
  }
  fclose(f);
}

void create_segments() {
  for (int i = 0; i < num_points - 1; i++) {
    for (int j = i + 1; j < num_points; j++) {
      s[num_segments++].from_points(p[i], p[j]);
    }
  }
}

void sort_by_bisectors() {
  std::sort(s, s + num_segments, [](segment& s, segment& t) {
    if (s.bis.a != t.bis.a) {
      return s.bis.a < t.bis.a;
    } else if (s.bis.b != t.bis.b) {
      return s.bis.b < t.bis.b;
    } else if (s.bis.c != t.bis.c) {
      return s.bis.c < t.bis.c;
    } else {
      return s.pos < t.pos;
    }
  });
}

void sort_by_length(int start, int end) {
  std::sort(s + start, s + end, [](segment& s, segment& t) {
    return s.len2 < t.len2;
  });
}

// TODO: Codul următoarelor două funcții este aproape duplicat.
int count_collinear_pairs(int start, int end) {
  int result = 0, i = start;
  while (i < end) {
    int j = i + 1;
    while ((j < end) && (s[i].pos == s[j].pos)) {
      j++;
    }
    result += (j - i) * (j - i - 1) / 2;
    i = j;
  }
  return result;
}

int count_same_length_pairs(int start, int end) {
  int result = 0, i = start;
  while (i < end) {
    int j = i + 1;
    while ((j < end) && (s[i].len2 == s[j].len2)) {
      j++;
    }
    result += (j - i) * (j - i - 1) / 2;
    i = j;
  }
  return result;
}

// Acest grup de segmente arată ca o frigăruie. Toate au o mediatoare comună
// și sînt sortate după poziția pe această mediatoare.
int count_bisector(int start, int end) {
  int result = (end - start) * (end - start - 1) / 2;
  result -= count_collinear_pairs(start, end);
  sort_by_length(start, end);
  result -= count_same_length_pairs(start, end);
  // Notă: nu este nevoie de pinex. Două segmente nu pot fi și coliniare, și
  // de aceeași lungime. Atunci ar fi confundate, or enunțul promite că
  // punctele sînt distincte.
  return result;
}

int count_trapezoids() {
  s[num_segments].bis = { 0, 0, 0 }; // santinelă

  int i = 0, result = 0;
  while (i < num_segments) {
    int j = i + 1;
    while (s[i].bis == s[j].bis) {
      j++;
    }
    result += count_bisector(i, j);
    i = j;
  }
  return result;
}

void write_result(int result) {
  FILE* f = fopen("trapezz.out", "w");
  fprintf(f, "%d\n", result);
  fclose(f);
}

int main() {
  read_data();
  create_segments();
  sort_by_bisectors();
  int result = count_trapezoids();
  write_result(result);

  return 0;
}
