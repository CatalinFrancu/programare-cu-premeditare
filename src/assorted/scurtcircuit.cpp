#include <stdio.h>
#include <string.h>
#include <vector>

const int MAX_K = 100'000;
const int MAX_LOG = 17;
const int MAX_CELLS = 2'500'000;

int OP_AND = 0;
int OP_OR = 1;
int OP_XOR = 2;

const char OP_STRINGS[3][5] = { "0001", "0111", "0110" };

struct cell {
  int op, pos1, pos2;
};

// Indicii celulelor unde stocăm biții de generare / propagare pentru o zonă.
struct gen_prop {
  int g, p;
};

// O matrice de biți indexată după coloane. mat[col] este coloana de biți de
// pe poziția col (de valoare 2^col).
std::vector<int> mat[MAX_K + MAX_LOG];
std::vector<int> new_mat[MAX_K + MAX_LOG];

// GP[p][i] = valorile generate și propagate de biții i-2^p+1...i
gen_prop GP[MAX_LOG][MAX_K + MAX_LOG];

// cells reține suplimentare generate, fără cele n·k de la intrare.
cell cells[MAX_CELLS];
int n, k, orig_k, pos_zero, num_cells, expected_digits, last_gp_row;

void read_data() {
  scanf("%d %d", &n, &k);
}

void init_bit_matrix() {
  for (int r = 0; r < n; r++) {
    for (int c = 0; c < k; c++) {
      mat[c].push_back(r * k + c);
    }
  }
}

int new_cell(int op, int pos1, int pos2) {
  cells[num_cells] = { op, pos1, pos2 };
  return num_cells++ + n * orig_k;
}

int ceil_log2(int n) {
  return 32 - __builtin_clz(n - 1);
}

void init_cells() {
  orig_k = k;
  expected_digits = k + ceil_log2(n);
  pos_zero = new_cell(OP_XOR, 0, 0); // x^x = 0
}

// Calculează suma a 3 biți. Rezultatul va avea 2 biți. Cel mai semnificativ
// este c_out (transportul), cel mai puțin semnificativ este s. Are adîncime 3
// și folosește 3 celule auxiliare.
//
// s = x xor y xor z
// c_out = x·y + y·z + z·x sau, ca să refolosim celulele, x·y + (x xor y)·z
void compress_32(int x, int y, int z, int* s, int* c_out) {
  int x_xor_y = new_cell(OP_XOR, x, y);
  *s = new_cell(OP_XOR, x_xor_y, z);
  int x_and_y = new_cell(OP_AND, x, y);
  int x_xor_y_and_z = new_cell(OP_AND, x_xor_y, z);
  *c_out = new_cell(OP_OR, x_and_y, x_xor_y_and_z);
}

void compress_column(int c) {
  while (mat[c].size() >= 3) {
    int x = mat[c].back(); mat[c].pop_back();
    int y = mat[c].back(); mat[c].pop_back();
    int z = mat[c].back(); mat[c].pop_back();
    int s, c_out;
    compress_32(x, y, z, &s, &c_out);
    new_mat[c].push_back(s);
    new_mat[c + 1].push_back(c_out);
  }

  for (int x: mat[c]) {
    new_mat[c].push_back(x);
  }
  mat[c].clear();
}

void pad_columns_to_length_2() {
  for (int c = 0; c < k; c++) {
    if (mat[c].size() == 1) {
      mat[c].push_back(pos_zero);
    }
  }
}

// Comprimă cîte 3 biți folosind un compresor 3:2 pînă cînd rămînem cu 3 biți
// pe fiecare coloană.
void compress_to_two_numbers() {
  bool three_or_more;
  do {
    three_or_more = false;
    for (int c = 0; c < k; c++) {
      compress_column(c);
      three_or_more |= (new_mat[c].size() >= 3);
    }
    if (new_mat[k].size()) {
      k++;
    }
    for (int c = 0; c < k; c++) {
      mat[c].swap(new_mat[c]);
    }
  } while (three_or_more);

  pad_columns_to_length_2();
}

// (a.g, a.p) ○ (b.g, b.p) = (b_g + a_g · b_p, a_p · b_p)
// În cuvinte: Intervalul a ∪ b generează transport dacă b generează transport
// sau dacă a generează transport, iar b îl propagă. Intervalul a ∪ b propagă
// transportul dacă și a și b îl propagă.
gen_prop combine(gen_prop a, gen_prop b) {
  gen_prop res;
  int a_gen_b_prop = new_cell(OP_AND, a.g, b.p);
  res.g = new_cell(OP_OR, b.g, a_gen_b_prop);
  res.p = new_cell(OP_AND, a.p, b.p);
  return res;
}

void up_sweep() {
  for (int i = 0; i < k; i++) {
    GP[0][i].g = new_cell(OP_AND, mat[i][0], mat[i][1]);
    GP[0][i].p = new_cell(OP_XOR, mat[i][0], mat[i][1]);
  }

  last_gp_row = 0;
  for (int p = 1, len = 1; len < k; p++, len <<= 1) {
    last_gp_row = p;
    for (int i = 0; i < len; i++) {
      GP[p][i] = GP[p - 1][i];
    }
    for (int i = len; i < k; i++) {
      GP[p][i] = combine(GP[p - 1][i - len], GP[p - 1][i]);
    }
  }
}

void compute_sum() {
  gen_prop* gp = GP[last_gp_row];

  new_cell(OP_XOR, GP[0][0].p, pos_zero);
  for (int i = 1; i < k; i++) {
    new_cell(OP_XOR, GP[0][i].p, gp[i - 1].g);
  }
  new_cell(OP_XOR, gp[k - 1].g, pos_zero);

  for (int i = k + 1; i < expected_digits; i++) {
    new_cell(OP_XOR, pos_zero, pos_zero);
  }
}

// Adună două numere cu un sumator Kogge-Stone.
void add_numbers() {
  up_sweep();
  compute_sum();
}

struct fast_output {
  static const int BUFFER_SIZE = 128 * 1024;
  char num[MAX_CELLS][8];
  char buf[BUFFER_SIZE];
  int ptr;

  void init(int max_val) {
    num[0][0] = '\0';
    for (int i = 1; i <= max_val; i++) {
      strcpy(num[i], num[i / 10]);
      int l = strlen(num[i]);
      num[i][l] = '0' + i % 10;
      num[i][l + 1] = '\0';
    }
    num[0][0] = '0';
    num[0][1] = '\0';

    ptr = 0;
  }

  void flush() {
    fwrite(buf, 1, ptr, stdout);
    ptr = 0;
  }

  void put_char(char c) {
    buf[ptr++] = c;
    if (ptr == BUFFER_SIZE) {
      flush();
    }
  }

  void put_int(int x) {
    put_string(num[x]);
  }

  void put_string(const char* s) {
    for (const char* c = s; *c; c++) {
      put_char(*c);
    }
  }
};

fast_output out;

void write_circuit() {
  out.init(num_cells + n * orig_k);

  out.put_int(num_cells);
  out.put_char('\n');
  for (int i = 0; i < num_cells; i++) {
    out.put_string(OP_STRINGS[cells[i].op]);
    out.put_char(' ');
    out.put_int(cells[i].pos1);
    out.put_char(' ');
    out.put_int(cells[i].pos2);
    out.put_char('\n');
  }

  out.flush();
}

int main() {
  read_data();
  init_bit_matrix();
  init_cells();
  compress_to_two_numbers();
  add_numbers();
  write_circuit();

  return 0;
}
