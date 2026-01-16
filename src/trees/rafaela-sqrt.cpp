// Metodă: descompunere în radical după interogări.
#include <algorithm>
#include <stdio.h>
#include <unordered_set>
#include <vector>

const int MAX_NODES = 200'000;
const int MAX_QUERIES = 200'000;
const int BLOCK_SIZE = 3'000;

struct query {
  int id;             // ordinea de la intrare
  int outside_delta;  // modificări în afara subarborelui
  int child_delta;    // modificări la fiul curent
  int max_touched;    // maximul dintre fiii anteriori

  query(int id) {
    this->id = id;
    outside_delta = 0;
    child_delta = 0;
    max_touched = 0;
  }
};

struct node {
  std::vector<int> adj;
  std::vector<query> q; // interogări (din blocul curent)
  int parent;
  int tin, tout;        // timpii de intrare/ieșire din DFS
  int pop, spop;        // populația din nod, respectiv din subarbore
};

struct update {
  int id, u, delta;
};

node nd[MAX_NODES + 1];
update updates[BLOCK_SIZE];
int answer[MAX_QUERIES + 1];
std::unordered_set<int> nodes_with_queries;
int n, num_ops, num_updates;
FILE* fin;

int min(int x, int y) {
  return (x < y) ? x : y;
}

int max(int x, int y) {
  return (x > y) ? x : y;
}

void read_data() {
  fscanf(fin, "%d %d", &n, &num_ops);

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    fscanf(fin, "%d %d", &u, &v);
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }

  for (int u = 1; u <= n; u++) {
    fscanf(fin, "%d", &nd[u].pop);
  }
}

// Calculează timpii DFS și părinții
void initial_dfs(int u) {
  static int time = 0;
  nd[u].tin = time++;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      nd[v].parent = u;
      initial_dfs(v);
    }
  }

  nd[u].tout = time - 1;
}

void read_ops(int start, int end) {
  char type;
  int u, delta;

  num_updates = 0;
  for (int id = start; id < end; id++) {
    fscanf(fin, " %c", &type);
    if (type == 'U') {
      fscanf(fin, "%d %d ", &delta, &u);
      updates[num_updates++] = { id, u, delta };
      // vor intra în vigoare în următorul bloc
      nd[u].pop += delta;
    } else {
      fscanf(fin, "%d ", &u);
      nd[u].q.push_back(query(id));
      nodes_with_queries.insert(u);
    }
  }
}

void sort_updates_by_dfs_time() {
  std::sort(updates, updates + num_updates, [](update& a, update& b) {
    return nd[a.u].tin < nd[b.u].tin;
  });
}

void dfs(int u) {
  nd[u].spop = nd[u].pop;

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      dfs(v);
      nd[u].spop += nd[v].spop;
    }
  }
}

// Notifică-l pe u că există actualizarea upd în afara subarborelui său.
void process_outside_update(int u, update& upd) {
  for (query& q: nd[u].q) {
    if (upd.id < q.id) {
      q.outside_delta += upd.delta;
    }
  }
}

// Notifică-l pe u că există actualizarea upd în fiul său curent.
void process_child_update(int u, update& upd) {
  for (query& q: nd[u].q) {
    if (upd.id < q.id) {
      q.child_delta += upd.delta;
    }
  }
}

int merge_init(int u) {
  int i = 0;

  while ((i < num_updates) && (nd[updates[i].u].tin < nd[u].tin)) {
    process_outside_update(u, updates[i++]);
  }

  while ((i < num_updates) && (updates[i].u == u)) {
    // Doar le sărim. Populația din u nu influențează interogările din u.
    i++;
  }

  return i;
}

int merge_advance(int u, int v, int i) {
  while ((i < num_updates) && (nd[updates[i].u].tout <= nd[v].tout)) {
    process_child_update(u, updates[i++]);
  }

  for (query& q: nd[u].q) {
    q.max_touched = max(q.max_touched, nd[v].spop + q.child_delta);
    q.child_delta = 0;
  }

  return i;
}

void merge_finish(int u, int i) {
  while (i < num_updates) {
    process_outside_update(u, updates[i++]);
  }
}

void answer_queries(int u, int max_untouched) {
  for (query& q: nd[u].q) {
    // fluxul pe muchia dinspre părinte
    answer[q.id] = nd[1].spop + q.outside_delta - nd[u].spop;

    // maximul fiilor afectați de actualizări
    answer[q.id] = max(answer[q.id], q.max_touched);

    // maximul fiilor neafectați de actualizări
    answer[q.id] = max(answer[q.id], max_untouched);
  }

  nd[u].q.clear();
}

void process_queries(int u) {
  int max_untouched = 0;
  int i = merge_init(u);

  for (int v: nd[u].adj) {
    if (v != nd[u].parent) {
      int old_i = i;
      i = merge_advance(u, v, i);

      if (i == old_i) {
        max_untouched = max(max_untouched, nd[v].spop);
      }
    }
  }

  merge_finish(u, i);
  answer_queries(u, max_untouched);
}

void process_nodes_with_queries() {
  for (int u: nodes_with_queries) {
    process_queries(u);
  }
  nodes_with_queries.clear();
}

void process_ops() {
  for (int start = 0; start < num_ops; start += BLOCK_SIZE) {
    int end = min(start + BLOCK_SIZE, num_ops);
    dfs(1);
    read_ops(start, end);
    sort_updates_by_dfs_time();
    process_nodes_with_queries();
  }
}

void write_answers() {
  FILE* f = fopen("rafaela.out", "w");

  for (int i = 0; i < num_ops; i++) {
    if (answer[i]) {
      fprintf(f, "%d\n", answer[i]);
    }
  }

  fclose(f);
}

int main() {
  fin = fopen("rafaela.in", "r");

  read_data();
  initial_dfs(1);
  process_ops();

  fclose(fin);

  write_answers();

  return 0;
}
