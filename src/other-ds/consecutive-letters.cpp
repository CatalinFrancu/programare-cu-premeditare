#include <stdio.h>
#include <string.h>

const int MAX_N = 200'000;
const int MAX_Q = 100'000;
const int T_QUERY = 1;
const int T_UPDATE = 2;

struct disjoint_set_forest {
  int p[MAX_N];
  int len[MAX_N];

  void init(int n) {
    for (int u = 0; u < n; u++) {
      p[u] = u;
      len[u] = 1;
    }
  }

  int get_range_len(int u) {
    return len[find(u)];
  }

  int find(int u) {
    return (p[u] == u)
      ? u
      : (p[u] = find(p[u]));
  }

  void unite(int u, int v) {
    u = find(u);
    v = find(v);
    p[u] = v;
    len[v] += len[u];
  }
};

struct query {
  int type;
  int pos;
  int answer;
  char orig;
};

char s[MAX_N + 1];
query q[MAX_Q];
disjoint_set_forest dsf;
int n, num_queries;

void read_data_and_fast_forward() {
  scanf("%s %d", s, &num_queries);
  n = strlen(s);
  for (int i = 0; i < num_queries; i++) {
    scanf("%d %d", &q[i].type, &q[i].pos);
    if (q[i].type == T_UPDATE) {
      q[i].orig = s[q[i].pos];
      s[q[i].pos] = '#';
    }
  }
}

void init_connections() {
  dsf.init(n);
  for (int i = 1; i < n; i++) {
    if ((s[i] != '#') && (s[i] == s[i - 1])) {
      dsf.unite(i - 1, i);
    }
  }
}

void rewind() {
  for (int i = num_queries - 1; i >= 0; i--) {
    int pos = q[i].pos;
    if (q[i].type == T_UPDATE) {
      s[pos] = q[i].orig;
      if (pos && (s[pos] == s[pos - 1])) {
        dsf.unite(pos - 1, pos);
      }
      if ((pos < n) && (s[pos] == s[pos + 1])) {
        dsf.unite(pos, pos + 1);
      }
    } else {
      q[i].answer = dsf.get_range_len(pos);
    }
  }
}

void write_answers() {
  for (int i = 0; i < num_queries; i++) {
    if (q[i].type == T_QUERY) {
      printf("%d\n", q[i].answer);
    }
  }
}

int main() {
  int num_tests;

  scanf("%d", &num_tests);
  for (int t = 1; t <= num_tests; t++) {
    printf("Case %d:\n", t);
    read_data_and_fast_forward();
    init_connections();
    rewind();
    write_answers();
  }

  return 0;
}
