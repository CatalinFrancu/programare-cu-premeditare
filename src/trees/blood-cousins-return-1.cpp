// Complexitate: O(n log n).
//
// Fiecare nod calculează un set de nume distincte la fiecare adîncime în
// subarborele său.
#include <iostream>
#include <unordered_map>
#include <set>
#include <string>
#include <vector>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;

struct query {
  int orig_index;
  int depth;
};

struct node {
  std::vector<int> adj;
  std::vector<query> queries;
  int parent;
  int name;
};

struct name_map {
  std::unordered_map<std::string, int> map;

  int insert_and_get_number(std::string s) {
    auto it = map.find(s);
    if (it == map.end()) {
      int result = map.size();
      map[s] = result;
      return result;
    } else {
      return it->second;
    }
  }
};

// Urmărește elementele distincte la fiecare adîncime. Adîncimea 0 (care
// conține doar nodul însuși) este ultimul element din vector.
struct dist {
  std::vector<std::set<int>> d;

  void absorb(dist other) {
    if (other.d.size() > d.size()) {
      d.swap(other.d);
    }
    int offset = d.size() - other.d.size();
    for (unsigned i = 0; i < other.d.size(); i++) {
      std::set<int>& us = d[i + offset];
      std::set<int>& them = other.d[i];
      if (them.size() > us.size()) {
        us.swap(them);
      }
      us.merge(them);
      them.clear();
    }
  }

  void prepend(int name) {
    d.push_back({name});
  }

  int count_distinct(unsigned depth) {
    if (depth < d.size()) {
      return d[d.size() - 1 - depth].size();
    } else {
      return 0;
    }
  }
};

node nd[MAX_NODES + 1]; // Vom folosi nodul 0 ca pe un strămoș comun fals.
int sol[MAX_QUERIES];
int n, q;

void read_tree() {
  name_map map;

  std::cin >> n;
  for (int u = 1; u <= n; u++) {
    std::string name;
    std::cin >> name >> nd[u].parent;
    nd[nd[u].parent].adj.push_back(u);
    nd[u].name = map.insert_and_get_number(name);
  }
}

void read_queries() {
  std::cin >> q;
  for (int i = 0; i < q; i++) {
    int u, depth;
    std::cin >> u >> depth;
    nd[u].queries.push_back({i, depth});
  }
}

dist dfs(int u) {
  dist result;
  for (int v: nd[u].adj) {
    result.absorb(dfs(v));
  }
  result.prepend(nd[u].name);

  for (query q: nd[u].queries) {
    sol[q.orig_index] = result.count_distinct(q.depth);
  }
  return result;
}

void write_answers() {
  for (int i = 0; i < q; i++) {
    std::cout << sol[i] << '\n';
  }
}

int main() {
  read_tree();
  read_queries();
  dfs(0);
  write_answers();

  return 0;
}
