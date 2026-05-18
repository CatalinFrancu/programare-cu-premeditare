#include <ctype.h>
#include <stdio.h>
#include <vector>

const int MAX_N = 2'500;
const int MAX_M = 5'000;
const int HASH_BASE = 5'113;

struct fast_input {
  static const int BUF_SIZE = 1 << 20;
  char buf[BUF_SIZE + 1];
  int pos = BUF_SIZE;

  char get_char() {
    if (pos == BUF_SIZE) {
      int read = fread(buf, 1, BUF_SIZE, stdin);
      buf[read] = '\0';
      pos = 0;
    }
    return buf[pos++];
  }

  int read_int() {
    int result = 0;
    char c;
    do {
      c = get_char();
    } while (!isdigit(c));

    do {
      result = 10 * result + c - '0';
      c = get_char();
    } while (isdigit(c));

    return result;
  }
};

// Aceleași date pentru nodurile stîngi și drepte, deoarece deocamdată nu știm
// care este care.
struct node {
  std::vector<short> adj;
  short pair;
  bool is_left;
  bool vis;
};

short p[MAX_N + 1][MAX_M + 1];
short inv[MAX_N + 1][MAX_M + 1]; // permutările inverse
short diff[MAX_N + 1][MAX_N + 1]; // prima diferență
unsigned h[MAX_N + 1][MAX_M + 1];
int ord[MAX_N];
unsigned pow_b[MAX_M + 1];
node nd[MAX_N + 1];
int n, m;

void read_and_hash_data() {
  fast_input f;
  n = f.read_int();
  m = f.read_int();
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      p[i][j] = f.read_int();
      inv[i][p[i][j]] = j;
      h[i][j] = h[i][j - 1] * HASH_BASE + p[i][j];
    }
  }
}

void precompute_powers() {
  pow_b[0] = 1;
  for (int i = 1; i <= m; i++) {
    pow_b[i] = pow_b[i - 1] * HASH_BASE;
  }
}

// Returnează true dacă și numai dacă p[a] este pară.
bool is_even(int a) {
  bool result = true;

  // Numără lungimile ciclurilor. Adaugă m la fiecare element pentru a-l
  // marca.
  for (int i = 0; i < m; i++) {
    if (p[a][i] < m) {
      int j = i, len = 0;
      do {
        p[a][j] += m;
        j = p[a][j] - m;
        len++;
      } while (j != i);
      result ^= !(len & 1); // ciclurile pare schimbă paritatea
    }
  }

  // Refă p[a].
  for (int i = 0; i < m; i++) {
    p[a][i] -= m;
  }

  return result;
}

void compute_parities() {
  for (int i = 1; i <= n; i++) {
    nd[i].is_left = is_even(i);
  }
}

void swap(int i, int j) {
  int tmp = ord[i];
  ord[i] = ord[j];
  ord[j] = tmp;
}

void mark_differences(int first, int left, int right, int last, int pos) {
  for (int i = first; i < left; i++) {
    for (int j = left; j < last; j++) {
      diff[ord[i]][ord[j]] = pos;
      diff[ord[j]][ord[i]] = pos;
    }
  }
  for (int i = left; i < right; i++) {
    for (int j = right; j < last; j++) {
      diff[ord[i]][ord[j]] = pos;
      diff[ord[j]][ord[i]] = pos;
    }
  }
}

// Sortează permutările ord[first..last), care sînt egale pînă la poziția
// pos-1.
void ternary_sort(int first, int last, int pos) {
  if (last - first <= 1) {
    return;
  }

  int i = first, left = first, right = last;
  short pivot = p[ord[first]][pos];

  // Invariant de buclă:
  // * ord[first ... left)     poziția pos este < pivot
  // * ord[left ... i)         poziția pos este == pivot
  // * ord[i]                  permutarea în curs de evaluare
  // * ord(i ... right)        permutări încă neevaluate
  // * ord[right ... last)     poziția pos este > pivot
  while (i < right) {
    short x = p[ord[i]][pos];
    if (x < pivot) {
      swap(i++, left++);
    } else if (x > pivot)  {
      swap(i, --right);
    } else {
      i++;
    }
  }

  mark_differences(first, left, right, last, pos);

  ternary_sort(first, left, pos);
  if (pos < m) {
    ternary_sort(left, right, pos + 1);
  }
  ternary_sort(right, last, pos);
}

void compute_first_diff() {
  for (int i = 0; i < n; i++) {
    ord[i] = i + 1;
  }
  ternary_sort(0, n, 1);
}

unsigned range_hash(int i, int l, int r) {
  return h[i][r] - h[i][l - 1] * pow_b[r - l + 1];
}

// Returnează true dacă și numai dacă p[i][l..r] == p[j][l..r].
bool hash_equal(int i, int j, int l, int r) {
  return range_hash(i, l, r) == range_hash(j, l, r);
}

bool similar_perm(int i, int j) {
  if (nd[i].is_left == nd[j].is_left) {
    return false;
  }

  // Dacă p[i][first] = 4 și p[j][first] = 10, ne întrebăm unde apare 10 în
  // p[i]. Aceste două diferențe trebuie să fie singurele.
  int first = diff[i][j];
  int last = inv[i][p[j][first]];
  return hash_equal(i, j, first + 1, last - 1) &&
    hash_equal(i, j, last + 1, m);
}

void build_graph() {
  for (int i = 1; i < n; i++) {
    for (int j = i + 1; j <= n; j++) {
      if (similar_perm(i, j)) {
        nd[i].adj.push_back(j);
        nd[j].adj.push_back(i);
      }
    }
  }
}

void clear_vis() {
  for (int u = 1; u <= n; u++) {
    nd[u].vis = false;
  }
}

bool dfs(int u) {
  if (nd[u].vis) {
    return false;
  }
  nd[u].vis = true;

  for (int v: nd[u].adj) {
    if (!nd[v].pair || dfs(nd[v].pair)) {
      nd[v].pair = u;
      return true;
    }
  }
  return false;
}

int kuhn() {
  int size = 0;
  for (int u = 1; u <= n; u++) {
    if (nd[u].is_left) {
      clear_vis();
      size += dfs(u);
    }
  }
  return size;
}

int main() {
  read_and_hash_data();
  precompute_powers();
  compute_parities();
  compute_first_diff();
  build_graph();
  int max_matching = kuhn();
  printf("%d\n", n - max_matching);
}
