// Complexitate: O(n log n).
#include <algorithm>
#include <stdio.h>
#include <vector>

const int MAX_NODES = 200'000;

struct edge {
  int v, color;
};

struct node {
  std::vector<edge> adj;
  int size;
  int height;
};

int max(int x, int y) {
  return (x > y) ? x : y;
}

int k, m, longest_path;

struct cost {
  // Pentru o lungime fixată, vom prefera lanțurile care necesită numărul
  // minim de schimbări, iar la egalitate pe cele care încep cu un prefix cît
  // mai scurt. Acest criteriu este optim. Nu avem niciun motiv să preferăm un
  // lanț cu mai multe schimbări și prefix mai scurt. Preferăm să facem acea
  // schimbare suplimentară doar dacă este nevoie, la combinarea a două
  // lanțuri.
  int changes, prefix;

  void optimize(cost other) {
    if ((other.changes < changes) ||
        ((other.changes == changes) && (other.prefix < prefix))) {
      *this = other;
    }
  }
};

struct costs {
  // v[len] = Costul minim pentru a ajunge la lungimea len.
  std::vector<cost> v;

  // Lungimea curentă a vectorului de costuri. Artificiu necesar deoarece
  // trebuie să ne încadrăm în O(max_depth) pentru fiecare subarbore, nu avem
  // voie O(n). Din același motiv nu putem inițializa vectorul în O(n).
  int max_len;

  costs() {
    v.reserve(MAX_NODES);
  }

  void swap(costs& other) {
    v.swap(other.v);
    std::swap(max_len, other.max_len);
  }

  void reset() {
    max_len = 0;
    v[0] = { 0, 0 };
  }

  void optimize(int len, cost c) {
    if (len > max_len) {
      v[len] = c;
      max_len = len;
    } else {
      v[len].optimize(c);
    }
  }

  // Combină doi vectori de costuri care pornesc pe muchii de culori opuse.
  // Folosind tehnica celor doi pointeri, caută lungimea maximă dintre două
  // elemente al căror cost însumat este cel mult m.
  void combine_opposite(costs& other) {
    int i = 0, j = other.max_len;
    while ((i <= max_len) && (j >= 0)) {
      if (v[i].changes + other.v[j].changes <= m) {
        longest_path = max(longest_path, i + j);
        i++;
      } else {
        j--;
      }
    }
  }

  // Față de combine_opposite(), aici adăugăm 1 la cost dacă suma prefixelor
  // depășește k.
  void combine_same(costs& other) {
    int i = 0, j = other.max_len;
    while ((i <= max_len) && (j >= 0)) {
      int sum_costs =
        v[i].changes +
        other.v[j].changes +
        (v[i].prefix + other.v[j].prefix > k);

      if (sum_costs <= m) {
        longest_path = max(longest_path, i + j);
        i++;
      } else {
        j--;
      }
    }
  }

  void absorb(costs& other) {
    // Fiii sînt sortați după înălțimea absolută, NU după înălțimea fezabilă
    // cu limita de modificări.
    if (other.max_len > max_len) {
      swap(other);
    }
    for (int i = 0; i <= other.max_len; i++) {
      v[i].optimize(other.v[i]);
    }
  }
};

// c[0/1] = Costurile optime pentru fiii explorați anterior care pornesc pe
// muchii 0, respectiv 1. cur_cost = costurile în construcție pentru fiul
// curent.
costs c[2], cur_cost;

// Combină fiul curent, care pornește pe culoarea cur_color, cu informațiile
// existente din c[0] și c[1]. Apoi absoarbe informațiile de aceeași culoare
// cu fiul curent.
void combine_and_absorb_costs(int cur_color) {
  c[cur_color].combine_same(cur_cost);
  c[1 - cur_color].combine_opposite(cur_cost);
  cur_cost.absorb(c[cur_color]);
  cur_cost.swap(c[cur_color]);
  cur_cost.reset();
}

struct tree {
  node a[MAX_NODES + 1];
  int n;

  // Informații despre prefixul căii curente în DFS-ul din centroidul
  // curent. Declarate în afara recursivității ca să reducem numărul de
  // parametri ai lui cost_dfs().
  int prefix_len, prefix_color;

  void add_edge(int u, int v, int color) {
    a[u].adj.push_back({v, color});
    a[v].adj.push_back({u, color});
  }

  // Calculează înălțimile nodurilor (raportate la frunze).
  void height_dfs(int u, int parent) {
    a[u].height = 0;
    for (edge e: a[u].adj) {
      if (e.v != parent) {
        height_dfs(e.v, u);
        a[u].height = max(a[u].height, 1 + a[e.v].height);
      }
    }
  }

  // Notează faptul că din fiul curent există o cale de lungime length care
  // face changes schimbări. Încorporează și informația despre prefixul căii.
  void collect_length(int length, int changes) {
    changes += prefix_len / (k + 1);
    if (changes <= m) {
      int rem = prefix_len % (k + 1);
      cur_cost.optimize(length + prefix_len, { changes, rem });
    }
  }

  // Vizitează nodul u. Pînă la el am făcut changes schimbări. Calea curentă
  // se termină cu un sufix de lungime suf_len și culoare suf_color.
  void cost_dfs(int u, int parent, int depth, int changes,
                int suf_len, int suf_color) {
    collect_length(depth, changes);

    for (edge e: a[u].adj) {
      if (e.v != parent) {
        if (e.color != suf_color) {
          // Schimbă culoarea: resetează sufixul.
          cost_dfs(e.v, u, depth + 1, changes, 1, e.color);
        } else if (suf_len + 1 <= k) {
          // Aceeași culoare. Mai putem extinde sufixul curent.
          cost_dfs(e.v, u, depth + 1, changes, suf_len + 1, e.color);
        } else if (changes < m) {
          // Aceeași culoare. Este nevoie de o schimbare.
          cost_dfs(e.v, u, depth + 1, changes + 1, 0, e.color);
        }
      }
    }
  }

  // Cîtă vreme sîntem pe culoarea de început a fiului curent, notează
  // lungimea căii. Pentru fiecare nod din subarbore ne interesează lungimea
  // prefixului de aceeași culoare cu care începe calea spre acel nod. Pot
  // exista schimbări și pe acest lanț, dar le vom contabiliza la final.
  void same_color_dfs(int u, int parent, int depth) {
    prefix_len = depth;
    collect_length(0, 0); // Nu avem nimic decît prefixul

    for (edge e: a[u].adj) {
      if (e.v != parent) {
        if (e.color == prefix_color) {
          same_color_dfs(e.v, u, depth + 1);
        } else {
          // Se schimbă culoarea; lansează DFS-ul normal, care contabilizează
          // schimbările.
          prefix_len = depth;
          cost_dfs(e.v, u, 1, 0, 1, e.color);
        }
      }
    }
  }

  // Compară toate căile care trec prin u și actualizează maximul global.
  void find_longest_path_through(int u) {
    // Procesează fiii în ordinea crescătoare a înălțimilor. Acest pas este
    // necesar ca să rulăm în O(mărimea componentei) per centroid. Mai jos,
    // combinarea fiului curent cu cei precedenți durează O(înălțimea maximă
    // de pînă acum), echivalent cu O(mărimea fiului curent) datorită
    // sortării.
    //
    // Facem sortarea naiv. Sortarea prin numărare este mai lentă.
    height_dfs(u, 0);
    std::sort(a[u].adj.begin(), a[u].adj.end(), [&](edge e, edge f) {
      return a[e.v].height < a[f.v].height;
    });

    c[0].reset();
    c[1].reset();

    for (edge e: a[u].adj) {
      // Notează culoarea muchiei pe care începe fiul curent.
      prefix_color = e.color;
      // Generează vectorul de costuri pentru fiul curent.
      same_color_dfs(e.v, u, 1);
      combine_and_absorb_costs(e.color);
    }
  }

  /**********************************************/
  /* Cod standard de descompunere în centroizi. */
  /**********************************************/

  // Calculează mărimea subarborelui fiecărui nod.
  void size_dfs(int u, int parent) {
    a[u].size = 1;

    for (edge e: a[u].adj) {
      if (e.v != parent) {
        size_dfs(e.v, u);
        a[u].size += a[e.v].size;
      }
    }
  }

  int find_centroid(int u, int limit) {
    for (edge e: a[u].adj) {
      if ((a[e.v].size < a[u].size) && (a[e.v].size > limit)) {
        return find_centroid(e.v, limit);
      }
    }

    return u;
  }

  // Încercare de optimizare: ca să nu tot testăm cîmpul .dead, ștergem
  // efectiv nodul din listele de adiacență ale vecinilor.
  void delete_edge(int u, int v) {
    int i = 0;
    while (a[u].adj[i].v != v) {
      i++;
    }
    a[u].adj[i] = a[u].adj.back();
    a[u].adj.pop_back();
  }

  void delete_node(int u) {
    for (edge e: a[u].adj) {
      delete_edge(e.v, u);
    }
  }

  // Descompune recursiv arborele în centroizi.
  void decompose(int u) {
    size_dfs(u, 0);
    u = find_centroid(u, a[u].size / 2);

    // Aici este efortul propriu-zis pentru centroidul curent.
    find_longest_path_through(u);

    delete_node(u);
    for (edge e: a[u].adj) {
      decompose(e.v);
    }
  }
};

tree t;

void read_data() {
  scanf("%d %d %d", &t.n, &k, &m);
  for (int i = 0; i < t.n - 1; i++) {
    int u, v, color;
    scanf("%d %d %d", &u, &v, &color);
    t.add_edge(u, v, color);
  }
}

int main() {
  read_data();
  t.decompose(1);
  printf("%d\n", longest_path);

  return 0;
}
