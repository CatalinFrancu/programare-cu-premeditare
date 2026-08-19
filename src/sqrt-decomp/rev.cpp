#include <stdio.h>
#include <stdlib.h>

const int MAX_N = 100'000;
const int OP_BLOCK_SIZE = 150;

// Direcția este de la stînga la dreapta sau de la dreapta la stînga după cum
// start ≶ end.
struct block {
  int start, end;

  int len() {
    return 1 + abs(end - start);
  }

  int dir() {
    return (start < end) ? +1 : -1;
  }

  int get_index_at(int pos) {
    return start + dir() * pos;
  }

  void reverse() {
    int tmp = start;
    start = end;
    end = tmp;
  }
};

struct block_list {
  block v[2 * OP_BLOCK_SIZE + 1];
  int cnt;

  // Cînd defragmentăm structura, citim dintr-un vector și scriem în celălalt.
  int base_array[2][MAX_N + 1];
  int row;

  void init(int n) {
    for (int i = 0; i < n; i++) {
      base_array[0][i] = i + 1;
    }
    row = 0;
    v[0] = { 0, n - 1 };
    cnt = 1;
  }

  void defrag() {
    int ptr = 0;
    for (int i = 0; i < cnt; i++) {
      int dir = v[i].dir();
      for (int pos = v[i].start; pos != v[i].end + dir; pos += dir) {
        base_array[1 - row][ptr++] = base_array[row][pos];
      }
    }
    row = 1 - row;
    v[0] = { 0, ptr - 1 };
    cnt = 1;
  }

  void get_block_and_real_index(int pos, int& b, int& ind) {
    b = 0;
    while (v[b].len() <= pos) {
      pos -= v[b++].len();
    }

    ind = v[b].get_index_at(pos);
  }

  int get_element_at(int pos) {
    int b, ind;
    get_block_and_real_index(pos, b, ind);
    return base_array[row][ind];
  }

  void split_block(int b, int last_left) {
    for (int i = cnt - 1; i >= b; i--) {
      v[i + 1] = v[i];
    }
    v[b + 1].start = last_left + v[b].dir();
    v[b].end = last_left;
    cnt++;
  }

  void reverse_blocks(int l, int r) {
    for (int i = l; i <= r; i++) {
      v[i].reverse();
    }

    while (l < r) {
      block tmp = v[l];
      v[l] = v[r];
      v[r] = tmp;
      l++;
      r--;
    }
  }

  void reverse(int l, int r) {
    int bl, indl, br, indr;
    get_block_and_real_index(l, bl, indl);
    get_block_and_real_index(r, br, indr);

    if (indr != v[br].end) {
      split_block(br, indr);
    }
    if (indl != v[bl].start) {
      split_block(bl, indl - v[bl].dir());
      // Inserarea unui nou bloc împinge bl și br la dreapta.
      bl++;
      br++;
    }
    reverse_blocks(bl, br);
  }
};

block_list blist;
FILE *fin, *fout;

void init_perm() {
  int n;
  fscanf(fin, "%d", &n);
  blist.init(n);
}

void process_ops() {
  int num_ops, l, r, pos;
  fscanf(fin, "%d", &num_ops);

  while (num_ops--) {
    fscanf(fin, "%d %d %d", &l, &r, &pos);
    blist.reverse(l - 1, r - 1);
    if (num_ops % OP_BLOCK_SIZE == 0) {
      blist.defrag();
    }
    int val = blist.get_element_at(pos - 1);
    fprintf(fout, "%d\n", val);
  }
}

int main() {
  fin = fopen("rev.in", "r");
  fout = fopen("rev.out", "w");

  init_perm();
  process_ops();

  fclose(fin);
  fclose(fout);

  return 0;
}
