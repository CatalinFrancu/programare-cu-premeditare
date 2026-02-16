#include <algorithm>
#include <set>
#include <stdio.h>

const int MAX_N = 100'000;

// Returnează true dacă și numai dacă (x1,y1), (x2,y2) și (x3,y3) sînt în
// ordine trigonometrică.
long long orientation(int x1, int y1, int x2, int y2, int x3, int y3) {
  return ((long long)(x2 - x1)) * (y3 - y2)
    - ((long long)(x3 - x2)) * (y2 - y1);
}

struct segment {
  int x1, y1, x2, y2;
  int ind;

  // Returneaza true dacă și numai dacă acest segment este „sub” o.
  bool operator<(segment const& o) const {
    if (x2 < o.x2) {
      // Acest segment *nu* este sub o dacă (x2,y2) cade pe o, adică dacă
      // (o.x1,o.y1), (o.x2,o.y2) și (x2,y2) sînt în ordine
      // antitrigonometrică.
      return orientation(o.x1, o.y1, o.x2, o.y2, x2, y2) < 0;
		} else {
      // Acest segment *este* sub o dacă (o.x2,o.y2) cade pe acest segment,
      // adică dacă (x1,y1), (x2,y2) și (o.x2,o.y2) sînt în ordine
      // trigonometrică.
      return orientation(x1, y1, x2, y2, o.x2, o.y2) > 0;
		}
  }
};

struct event {
  int ind;
  int x, y;
};

segment seg[MAX_N];
event evt[2 * MAX_N];
std::set<segment> set;
int n, bessie, hill_count;

void read_data() {
  scanf("%d", &n);
  for (int i = 0; i < n; i++) {
    scanf("%d %d %d %d", &seg[i].x1, &seg[i].y1, &seg[i].x2, &seg[i].y2);
    seg[i].ind = i;
  }
}

void create_and_sort_events() {
  for (int i = 0; i < n; i++) {
    evt[2 * i] = { i, seg[i].x1, seg[i].y1 };
    evt[2 * i + 1] = { i, seg[i].x2, seg[i].y2 };
  }

  std::sort(evt, evt + 2 * n, [](event& a, event& b) {
    return (a.x < b.x) || ((a.x == b.x) && (a.y < b.y));
  });
}

void process_end(event& e) {
  std::set<segment>::iterator it = set.find(seg[e.ind]);

  if (e.ind == bessie) {
    if (it == set.begin()) {
      bessie = n;
    } else {
      std::set<segment>::iterator prev = std::prev(it);
      bessie = prev->ind;
      hill_count++;
    }
  }

  set.erase(it);
}

void sweep_line() {
  bessie = 0;
  hill_count = 1;

  for (int i = 0; i < 2 * n; i++) {
    event& e = evt[i];
    segment& s = seg[e.ind];
    if (e.x == s.x1) {
      set.insert(s);
    } else {
      process_end(e);
    }
  }
}

int main() {
  read_data();
  create_and_sort_events();
  sweep_line();
  printf("%d\n", hill_count);

  return 0;
}
