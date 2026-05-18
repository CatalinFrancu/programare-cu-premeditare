#include <stdio.h>

const int SIGMA = 27;
const int MAX_LEFT = 200;
const int MAX_RIGHT = 531'441; // 27^4
const int MAX_EDGES = 17'901 * MAX_LEFT; // C(26,4)+C(26,3)+C(26,2)+C(26,1)
const int MAX_LENGTH = 10;

struct edge {
  int v, next;
};

struct left_node {
  int adj;
  int pair; // nenecesar pentru Kuhn, dar simplifică recuperarea soluției
  char word[MAX_LENGTH + 1];
  bool vis;
};

edge e[MAX_EDGES + 1];
left_node l[MAX_LEFT + 1];
unsigned char r[MAX_RIGHT + 1];
int n;

void read_data() {
  FILE* f = fopen("input.txt", "r");
  fscanf(f, "%d", &n);
  for (int i = 1; i <= n; i++) {
    fscanf(f, "%s", l[i].word);
  }
  fclose(f);
}

void add_edge(int u, int v) {
  static int ptr = 1;
  e[ptr] = { v, l[u].adj };
  l[u].adj = ptr++;
}

void build_graph() {
  for (int u = 1; u <= n; u++) {
    char* s = l[u].word;
    for (int a = 0; s[a] != '\0'; a++) {
      int code1 = s[a] - 'a' + 1;
      add_edge(u, code1);
      for (int b = a + 1; s[b] != '\0'; b++) {
        int code2 = code1 * SIGMA + s[b] - 'a' + 1;
        add_edge(u, code2);
        for (int c = b + 1; s[c] != '\0'; c++) {
          int code3 = code2 * SIGMA + s[c] - 'a' + 1;
          add_edge(u, code3);
          for (int d = c + 1; s[d] != '\0'; d++) {
            int code4 = code3 * SIGMA + s[d] - 'a' + 1;
            add_edge(u, code4);
          }
        }
      }
    }
  }
}

void reset_visited() {
  for (int u = 1; u <= n; u++) {
    l[u].vis = false;
  }
}

bool kuhn_dfs(int u) {
  if (l[u].vis) {
    return false;
  }
  l[u].vis = true;

  for (int ptr = l[u].adj; ptr; ptr = e[ptr].next) {
    int v = e[ptr].v;
    if (!r[v] || kuhn_dfs(r[v])) {
      l[u].pair = v;
      r[v] = u;
      return true;
    }
  }

  return false;
}

bool kuhn() {
  int u = 1;
  while ((u <= n) && kuhn_dfs(u)) {
    reset_visited();
    u++;
  }

  return (u > n);
}

void decode(int code, char* dst) {
  int len = 0;
  while (code) {
    dst[len++] = code % SIGMA + 'a' - 1;
    code /= SIGMA;
  }
  dst[len] = '\0';
  for (int i = 0, j = len - 1; i < j; i++, j--) {
    char tmp = dst[i];
    dst[i] = dst[j];
    dst[j] = tmp;
  }
}

void recover_solution(FILE* f) {
  char s[MAX_LENGTH + 1];
  for (int u = 1; u <= n; u++) {
    decode(l[u].pair, s);
    fprintf(f, "%s\n", s);
  }
}

int main() {
  read_data();
  build_graph();
  bool has_solution = kuhn();

  FILE* f = fopen("output.txt", "w");
  if (has_solution) {
    recover_solution(f);
  } else {
    fprintf(f, "-1\n");
  }
  fclose(f);

  return 0;
}
