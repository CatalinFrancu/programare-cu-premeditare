#include <stdio.h>

const int MAX_N = 127;

bool a[MAX_N + 1][MAX_N + 1];
bool vis[MAX_N + 1];
unsigned char rpair[MAX_N + 1], cpair[MAX_N + 1];
unsigned char sol[MAX_N + 1];
int n, num_moves;

void read_data() {
  FILE* f = fopen("revolutie.in", "r");
  fscanf(f, "%d ", &n);
  for (int r = 1; r <= n; r++) {
    for (int c = 1; c <= n; c++) {
      a[r][c] = (fgetc(f) == '1');
      fgetc(f);
    }
  }
  fclose(f);
}

void reset_visited() {
  for (int row = 1; row <= n; row++) {
    vis[row] = false;
  }
}

bool dfs(int row) {
  if (vis[row]) {
    return false;
  }
  vis[row] = true;

  for (int col = 1; col <= n; col++) {
    if (a[row][col] && (!cpair[col] || dfs(cpair[col]))) {
      rpair[row] = col;
      cpair[col] = row;
      return true;
    }
  }

  return false;
}

int kuhn() {
  int size = 0;
  for (int row = 1; row <= n; row++) {
    reset_visited();
    size += dfs(row);
  }
  return size;
}

void collect_moves() {
  for (int col = 1; col <= n; col++) {
    if (cpair[col] != col) {
      num_moves++;
      int r = cpair[col];
      sol[col] = r;
      rpair[r] = rpair[col];
      cpair[rpair[r]] = r;
    }
  }
}

void recover_solution(int max_matching) {
  FILE* f = fopen("revolutie.out", "w");
  if (max_matching < n) {
    fprintf(f, "-1\n");
  } else {
    collect_moves();
    fprintf(f, "%d\n", num_moves);
    for (int c = 1; c <= n; c++) {
      if (sol[c]) {
        fprintf(f, "L %d %d\n", c, sol[c]);
      }
    }
  }
  fclose(f);
}

int main() {
  read_data();
  int max_matching = kuhn();
  recover_solution(max_matching);
  return 0;
}
