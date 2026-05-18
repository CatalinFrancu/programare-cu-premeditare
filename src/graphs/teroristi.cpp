// Metodă: Construim un graf cu sursă, destinație și două coloane de 26 de
// noduri și respectiv 26*27/2 noduri. Rulăm algoritmul lui Dinic.
#include <queue>
#include <stdio.h>
#include <vector>

const int MAX_WORD = 1'000'000;

const int SIGMA = 26;
const int NUM_DIGRAMS = SIGMA * (SIGMA + 1) / 2;
const int MAX_NODES = 1 + SIGMA + NUM_DIGRAMS + 1;
const int MAX_EDGES = SIGMA + SIGMA * SIGMA + NUM_DIGRAMS;
const int NONE = -1;
const int INFINITY = 1'000'000'001;

// Pentru fiecare digramă între 0 și 26*27/2-1, stochează frecvența sa și o
// listă de poziții pe care apare în fișierul de intrare.
struct digrams {
  int freq[NUM_DIGRAMS];
  std::vector<int> pos[NUM_DIGRAMS];
  short code[SIGMA][SIGMA];

  void init() {
    int k = 0;
    for (int i = 0; i < SIGMA; i++) {
      for (int j = i; j < SIGMA; j++) {
        code[i][j] = code[j][i] = k++;
      }
    }
  }

  void push(char c1, char c2, int line) {
    int k = code[c1 - 'a'][c2 - 'a'];
    freq[k]++;
    pos[k].push_back(line);
  }

  int pop(int code) {
    int result = pos[code].back();
    pos[code].pop_back();
    return result;
  }
};

struct edge {
  int cap;
  short v, next;
};

struct node {
  short adj;
  short ptr;
  short dist;

  bool is_reachable() {
    return dist != NONE;
  }
};

int min(int x, int y) {
  return (x < y) ? x : y;
}

struct graph {
  edge e[2 * MAX_EDGES];
  node n[MAX_NODES + 1];
  int num_nodes, source, sink;

  void add_edge(short u, short v, int cap) {
    static int pos = 0;

    e[pos] = { cap, v, n[u].adj };
    n[u].adj = pos++;
  }

  void add_edge_pair(short u, short v, int cap) {
    add_edge(u, v, cap);
    add_edge(v, u, 0);
  }

  void init(int* letter_freq, digrams dig) {
    num_nodes = SIGMA + NUM_DIGRAMS + 2;
    source = num_nodes - 2;
    sink = num_nodes - 1;

    for (int u = 0; u < num_nodes; u++) {
      n[u].adj = NONE;
    }

    for (int i = 0; i < SIGMA; i++) {
      add_edge_pair(source, i, letter_freq[i]);
      for (int j = 0; j < SIGMA; j++) {
        int code = dig.code[i][j];
        add_edge_pair(i, code + SIGMA, INFINITY);
      }
    }

    for (int i = 0; i < NUM_DIGRAMS; i++) {
      add_edge_pair(i + SIGMA, sink, dig.freq[i]);
    }
  }

  bool bfs_reachable() {
    for (int u = 0; u < num_nodes; u++) {
      n[u].dist = NONE;
    }
    n[source].dist = 0;

    std::queue<short> q;
    q.push(source);

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      for (int pos = n[u].adj; pos != NONE; pos = e[pos].next) {
        int v = e[pos].v;
        if (!n[v].is_reachable() && e[pos].cap) {
          n[v].dist = n[u].dist + 1;
          q.push(v);
        }
      }
    }

    return n[sink].is_reachable();
  }

  void reset_dfs_pointers() {
    for (int u = 0; u < num_nodes; u++) {
      n[u].ptr = n[u].adj;
    }
  }

  int dfs(int u, int incoming_flow) {
    if (u == sink) {
      return incoming_flow;
    }

    int result = 0;
    while (!result && (n[u].ptr != NONE)) {
      int p = n[u].ptr;
      int v = e[p].v, cap = e[p].cap;
      if (cap && (n[v].dist == n[u].dist + 1)) {
        result = dfs(v, min(cap, incoming_flow));
        e[p].cap -= result;
        e[p ^ 1].cap += result;
      }
      // Rămîi pe aceeași muchie dacă o mai putem folosi.
      if (!result) {
        n[u].ptr = e[p].next;
      }
    }

    return result;
  }

  void dinic() {
    while (bfs_reachable()) {
      reset_dfs_pointers();
      while (dfs(source, INFINITY)) {
      }
    }
  }

  int get_digram(char c) {
    // Caută o muchie reziduală dinspre dreapta (dinspre digrame). Scoate o
    // unitate de flux de pe ea și returnează codul digramei.
    int u = c - 'a';
    short& p = n[u].ptr;
    while (!(p ^ 1) || !e[p ^ 1].cap) {
      p = e[p].next;
    }
    e[p ^ 1].cap--;
    return e[p].v - SIGMA;
  }
};

graph g;
char word[MAX_WORD + 1];
digrams dig;
int letter_freq[SIGMA];

void read_data() {
  int ignored, num_pairs;

  FILE* f = fopen("teroristi.in", "r");
  fscanf(f, "%d %d %s ", &ignored, &num_pairs, word);

  for (int line = 1; line <= num_pairs; line++) {
    char c1, c2;
    fscanf(f, "%c %c ", &c1, &c2);
    dig.push(c1, c2, line);
  }
  fclose(f);
}

void compute_letter_frequencies() {
  for (char* s = word; *s; s++) {
    letter_freq[*s - 'a']++;
  }
}

void recover_solution() {
  g.reset_dfs_pointers();
  FILE* f = fopen("teroristi.out", "w");
  for (char* s = word; *s; s++) {
    int code = g.get_digram(*s);
    int line = dig.pop(code);
    fprintf(f, "%d ", line);
  }
  fprintf(f, "\n");
  fclose(f);
}

int main() {
  dig.init();
  read_data();
  compute_letter_frequencies();
  g.init(letter_freq, dig);
  g.dinic();
  recover_solution();

  return 0;
}
