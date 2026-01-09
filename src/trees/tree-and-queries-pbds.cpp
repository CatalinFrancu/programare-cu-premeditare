// Small-to-large relativ brut. Fiecare nod stochează informații despre
// subarbore:
//
// 1. Un map de culoare => frecvență.
// 2. Un multiset doar cu frecvențele.
//
// Folosim STL cu larghețe pentru un cod cît mai lent.
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>
#include <iostream>
#include <map>
#include <vector>

// Multiseturile PBDS sînt obscene, dar par să meargă. Ștergerile necesită cod
// extra. Vezi https://stackoverflow.com/q/59731946/6022817
typedef __gnu_pbds::tree<
  int,
  __gnu_pbds::null_type,
  std::less_equal<int>,
  __gnu_pbds::rb_tree_tag,
  __gnu_pbds::tree_order_statistics_node_update
  > multiset;

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;

struct query {
  int orig_index;
  int min_freq;
};

struct node {
  int color;
  std::vector<int> adj;
  std::vector<query> q;
};

struct freq_info {
  std::map<int, int> f;
  multiset sorted_f;

  freq_info(int color) {
    f.insert({color, 1});
    sorted_f.insert(1);
  }

  void update_frequency(int color, int cnt) {
    int new_freq;
    auto it = f.find(color);
    if (it != f.end()) {
      int old_freq = it->second;
      int rank = sorted_f.order_of_key(old_freq);
      multiset::iterator to_erase = sorted_f.find_by_order(rank);
      sorted_f.erase(to_erase);
      it->second += cnt;
      new_freq = it->second;
    } else {
      f.insert({color, cnt});
      new_freq = cnt;
    }
    sorted_f.insert(new_freq);
  }

  void absorb(freq_info& other) {
    if (other.f.size() > f.size()) {
      f.swap(other.f);
      sorted_f.swap(other.sorted_f);
    }
    for (auto [color, cnt]: other.f) {
      update_frequency(color, cnt);
    }
    other.f.clear();
    other.sorted_f.clear();
  }

  int count_freq_gte(int min_freq) {
    return f.size() - sorted_f.order_of_key(min_freq);
  }
};

node nd[MAX_NODES + 1];
int sol[MAX_QUERIES];
int n, q;

void read_data() {
  std::cin >> n >> q;
  for (int u = 1; u <= n; u++) {
    std::cin >> nd[u].color;
  }
  for (int i = 1; i < n; i++) {
    int u, v;
    std::cin >> u >> v;
    nd[u].adj.push_back(v);
    nd[v].adj.push_back(u);
  }
  for (int i = 0; i < q; i++) {
    int u, min_freq;
    std::cin >> u >> min_freq;
    nd[u].q.push_back({i, min_freq});
  }
}

freq_info dfs(int u) {
  freq_info result(nd[u].color);
  nd[u].color = 0; // previne revizitarea fără a necesita un argument în plus

  for (int v: nd[u].adj) {
    if (nd[v].color) {
      freq_info fi = dfs(v);
      result.absorb(fi);
    }
  }

  for (query q: nd[u].q) {
    sol[q.orig_index] = result.count_freq_gte(q.min_freq);
  }

  return result;
}

void write_solution() {
  for (int i = 0; i < q; i++) {
    std::cout << sol[i] << '\n';
  }
}

int main() {
  read_data();
  dfs(1);
  write_solution();

  return 0;
}
