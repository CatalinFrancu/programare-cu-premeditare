#include <stdio.h>
#include <unordered_set>

const int MAX_NODES = 100'000;
const int MAX_PATH = 1'800; // determinat experimental

typedef std::unordered_set<int> hash_set;

// Un buffer circular de n elemente care operează într-o zonă de memorie
// alocată extern.
struct circ_buf {
  int* v;
  int n;
  int offset; // Conținutul real al vectorului este v[offset]...v[offset-1].
  int num_nonzero;

  void init(int* _v, int _n) {
    v = _v;
    n = _n;
    offset = 0;
    num_nonzero = 0;

    for (int i = 0; i < n; i++) {
      v[i] = 0;
    }
  }

  bool is_empty() {
    return num_nonzero == 0;
  }

  int get(int pos) {
    return v[(pos + offset) % n];
  }

  void set(int pos, int val) {
    pos = (pos + offset) % n;
    num_nonzero -= (v[pos] != 0);
    v[pos] = val;
    num_nonzero += (v[pos] != 0);
  }

  void add(int pos, int delta) {
    set(pos, get(pos) + delta);
  }

  // Shiftează (conceptual) toate elementele spre stînga. Inserează un zero la
  // coadă. Returnează elementul care a ieșit din buffer.
  int shift_left() {
    int leftmost = v[offset];
    num_nonzero -= (leftmost != 0);
    v[offset] = 0;
    offset = (offset + 1) % n;
    return leftmost;
  }

  int shift_right() {
    offset = (offset + n - 1) % n;
    int rightmost = v[offset];
    num_nonzero -= (rightmost != 0);
    v[offset] = 0;
    return rightmost;
  }

  // Mută naiv elementele din stînga lui pos cu 1 mai la dreapta. Mută
  // eficient elementele din dreapta lui pos cu 1 mai la stînga. Returnează
  // valoarea inițială de pe poziția pos.
  int attract_left(int pos) {
    if (pos == 0) {
      return shift_left();
    }

    int extracted = get(pos);

    add(pos + 1, get(pos - 1));
    for (int i = pos; i >= 2; i--) {
      set(i, get(i - 2));
    }
    set(1, 0);
    shift_left();

    return extracted;
  }

  int attract_right(int pos) {
    if (pos == n - 1) {
      return shift_right();
    }

    int extracted = get(pos);

    add(pos - 1, get(pos + 1));
    for (int i = pos; i + 2 < n; i++) {
      set(i, get(i + 2));
    }
    set(n - 2, 0);
    shift_right();

    return extracted;
  }

  int attract(int pos) {
    return (2 * pos < n)
      ? attract_left(pos)
      : attract_right(pos);
  }
};

struct cell {
  int v, next;
};

struct node {
  int adj;
  int parent;
  int heavy;   // fiul cu cel mai mare subarbore
  int head;    // capătul lanțului nostru
  int pos;     // momentul descoperirii în dfs
  int tmp_pop; // populația din nod, doar temporar pînă construim HLD

  circ_buf pop; // populația pe lanț, folosită doar în capetele de lanț
};

// Nu putem muta instantaneu caii de pe un lanț L1 pe altul L2, căci dacă încă
// nu l-am procesat pe L2, vom muta ulterior caii încă odată. Deci ținem o
// listă de postprocesări de făcut după ce procesăm toate lanțurile.
struct postprocess {
  int node, pop;
};

cell list[2 * MAX_NODES];
int circ_buf_space[MAX_NODES];
node nd[MAX_NODES + 1];
hash_set nonempty_paths; // Doar prin aceste lanțuri vom itera.
postprocess postproc[MAX_NODES];
int n, num_queries, num_postproc;

void add_edge(int u, int v) {
  static int ptr = 1;
  list[ptr] = { v, nd[u].adj };
  nd[u].adj = ptr++;
}

void read_tree() {
  scanf("%d %d", &n, &num_queries);

  for (int u = 1; u <= n; u++) {
    scanf("%d", &nd[u].tmp_pop);
  }

  for (int i = 0; i < n - 1; i++) {
    int u, v;
    scanf("%d %d", &u, &v);
    add_edge(u, v);
    add_edge(v, u);
  }
}

int heavy_dfs(int u) {
  int my_size = 1, max_child_size = 0;

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent) {

      nd[v].parent = u;
      int child_size = heavy_dfs(v);
      my_size += child_size;
      if (child_size > max_child_size) {
        max_child_size = child_size;
        nd[u].heavy = v;
      }

    }
  }

  return my_size;
}

// Descompunere heavy-light cu o mică modificare: impunem o limită de
// MAX_PATH pe lungimea oricărui lanț.
void decompose_dfs(int u, int head) {
  static int time = 0;

  nd[u].head = head;
  nd[u].pos = time++;
  int path_size = nd[u].pos - nd[head].pos + 1;
  bool can_fit_heavy = (path_size < MAX_PATH);

  if (nd[u].heavy && can_fit_heavy) {
    decompose_dfs(nd[u].heavy, head);
  } else {
    // Aici se termină lanțul head-u.
    int* start_pos = circ_buf_space + nd[head].pos;
    nd[head].pop.init(start_pos, path_size);
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    int v = list[ptr].v;
    if (v != nd[u].parent && ((v != nd[u].heavy) || !can_fit_heavy)) {
      decompose_dfs(v, v); // la v începe un lanț nou
    }
  }
}

void populate_circular_buffers() {
  for (int u = 1; u <= n; u++) {
    int h = nd[u].head;
    int pos = nd[u].pos - nd[h].pos;
    nd[h].pop.add(pos, nd[u].tmp_pop);
  }

  for (int u = 1; u <= n; u++) {
    if ((nd[u].head == u) && !nd[u].pop.is_empty()) {
      nonempty_paths.insert(u);
    }
  }
}

void migrate_paths_to_root(int u, hash_set& paths_to_root) {
  int prev_h = 0;
  while (u) {
    int h = nd[u].head;
    if (!nd[h].pop.is_empty()) {
      int pos = nd[u].pos - nd[h].pos;
      int extracted = nd[h].pop.attract(pos);
      if (prev_h && extracted) {
        // Caii din centrul migrației coboară la începutul următorului lanț.
        postproc[num_postproc++] = { prev_h, extracted };
      } else {
        // Caii din centrul migrației nu pleacă nicăieri.
        nd[h].pop.add(pos, extracted);
      }
    }

    paths_to_root.insert(h);
    u = nd[h].parent;
    prev_h = h;
  }
}

// Migrează caii de pe celelalte lanțuri nevide. Evită lanțurile deja migrate
// în migrate_paths_to_root.
void migrate_other_paths(int u, hash_set paths_to_root) {
  for (int h: nonempty_paths) {
    if (!paths_to_root.contains(h)) {
      int extracted = nd[h].pop.shift_left(); // spre rădăcină
      if (extracted) {
        postproc[num_postproc++] = { nd[h].parent, extracted };
      }
    }
  }
}

void post_migration() {
  // Ștergem din set în timp ce iterăm prin set.
  for (auto it = nonempty_paths.begin(); it != nonempty_paths.end(); ) {
    if (nd[*it].pop.is_empty()) {
      it = nonempty_paths.erase(it);
    } else {
      it++;
    }
  }

  while (num_postproc) {
    postprocess& p = postproc[--num_postproc];
    int h = nd[p.node].head;
    int pos = nd[p.node].pos - nd[h].pos;
    nd[h].pop.add(pos, p.pop);
    nonempty_paths.insert(h);
  }
}

void migrate(int u) {
  hash_set paths_to_root;
  migrate_paths_to_root(u, paths_to_root);
  migrate_other_paths(u, paths_to_root);
  post_migration();
}

int query(int u) {
  int h = nd[u].head;
  int pos = nd[u].pos - nd[h].pos;
  return nd[h].pop.get(pos);
}

void process_ops() {
  char type1, type2;
  int u;

  while (num_queries--) {
    scanf(" %c%c %d", &type1, &type2, &u);
    if (type1 == 'C') {
      migrate(u);
    } else {
      int claimed_pop;
      scanf("%d", &claimed_pop);
      printf("%d\n", claimed_pop == query(u));
    }
  }
}

int main() {
  read_tree();
  heavy_dfs(1);
  decompose_dfs(1, 1);
  populate_circular_buffers();
  process_ops();

  return 0;
}
