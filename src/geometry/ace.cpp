#include <stdio.h>

const int MAX_ROWS = 1'000;

short a[MAX_ROWS][MAX_ROWS];
int task, rows, cols, result;

void read_matrix() {
  FILE* f = fopen("ace.in", "r");
  fscanf(f, "%d %d %d", &task, &rows, &cols);

  for (int r = rows - 1; r >= 0; r--) {
    for (int c = cols - 1; c >= 0; c--) {
      fscanf(f, "%hd", &a[r][c]);
    }
  }

  fclose(f);
}

// Numără de la (dr,dc) și avansînd cu (dr,dc). Pentru distanța pe orizontală
// ar trebui să folosim numărul iterației, dar orice combinație liniară de r
// și c este bună. Deci folosim (r+c) ca să eliminăm o variabilă.
void count_direction(int dr, int dc) {
  int max_h = 0, max_g = 1;

  for (int r = dr, c = dc;
       r < rows && c < cols;
       r += dr, c += dc) {
    if (a[r][c] * max_g > max_h * (r + c)) {
      result++;
      max_h = a[r][c];
      max_g = r + c;
    }
    a[r][c] = 0;
  }
}

void count_visible() {
  if (task == 1) {
    count_direction(0, 1);
    count_direction(1, 0);
  } else {
    for (int dr = 0; dr < rows; dr++) {
      for (int dc = 0; dc < cols; dc++) {
        if (a[dr][dc]) {
          count_direction(dr, dc);
        }
      }
    }
  }
}

void write_result() {
  FILE* f = fopen("ace.out", "w");
  fprintf(f, "%d\n", result);
  fclose(f);
}

int main() {
  read_matrix();
  count_visible();
  write_result();
  return 0;
}
