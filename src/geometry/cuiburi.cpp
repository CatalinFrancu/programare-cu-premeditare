#include <algorithm>
#include <math.h>
#include <stdio.h>

const int MAX_NESTS = 2'000;
const int T_RECTANGLE = 0;
const int T_CIRCLE = 1;

bool closer(int x1, int y1, int x2, int y2, int dist) {
  return (x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) <= dist * dist;
}

struct nest {
  double area;
  union { short x1; short x; };
  union { short y1; short y; };
  union { short x2; short r; };
  union { short y2; short unused; };
  unsigned char type;

  void read(FILE* f) {
    fscanf(f, "%hhu", &type);
    if (type == T_RECTANGLE) {
      fscanf(f, "%hd %hd %hd %hd", &x1, &y1, &x2, &y2);
    } else {
      fscanf(f, "%hd %hd %hd", &x, &y, &r);
    }

    compute_area();
  }

  void compute_area() {
    if (type == T_RECTANGLE) {
      area = ((int)x2 - x1) * ((int)y2 - y1);
    } else {
      area = M_PI * r * r;
    }
  }

  bool fits_in(nest& other) {
    if (type == T_RECTANGLE) {
      if (other.type == T_RECTANGLE) {
        return
          (x1 >= other.x1) && (y1 >= other.y1) &&
          (x2 <= other.x2) && (y2 <= other.y2);
      } else {
        return
          closer(x1, y1, other.x, other.y, other.r) &&
          closer(x1, y2, other.x, other.y, other.r) &&
          closer(x2, y1, other.x, other.y, other.r) &&
          closer(x2, y2, other.x, other.y, other.r);
      }
    } else {
      if (other.type == T_RECTANGLE) {
        return
          (x >= other.x1 + r) &&
          (x <= other.x2 - r) &&
          (y >= other.y1 + r) &&
          (y <= other.y2 - r);
      } else {
        return closer(x, y, other.x, other.y, other.r - r);
      }
    }
  }
};

nest a[MAX_NESTS];
short len[MAX_NESTS];
int n;

void read_nests() {
  FILE* f = fopen("cuiburi.in", "r");
  fscanf(f, "%d", &n);
  for (int i = 0; i < n; i++) {
    a[i].read(f);
  }
  fclose(f);
}

void sort_nests() {
  std::sort(a, a + n, [](nest& p, nest& q) {
    return p.area < q.area;
  });
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

int find_longest_subsequence() {
  short max_len = 0;

  for (int i = 0; i < n; i++) {
    len[i] = 1;
    for (int j = 0; j < i; j++) {
      if (a[j].fits_in(a[i])) {
        len[i] = max(len[i], 1 + len[j]);
      }
    }

    max_len = max(max_len, len[i]);
  }

  return max_len;
}

void write_result(int result) {
  FILE* f = fopen("cuiburi.out", "w");
  fprintf(f, "%d\n", result);
  fclose(f);
}

int main() {
  read_nests();
  sort_nests();
  int result = find_longest_subsequence();
  write_result(result);

  return 0;
}
