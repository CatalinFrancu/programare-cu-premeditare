#include <algorithm>
#include <stdio.h>
#include <string.h>
#include <vector>
#include <unordered_map>

const int MAX_LEN = 200'000;
const int MAX_ROOT = 1'000;
const int SIGMA = 26;
const int NONE = -1;

// Informații despre candidatul curent și optimul găsit.
char s[MAX_LEN + 1], cand[MAX_LEN + 1], best[MAX_LEN + 1];
int best_id;

int num_queries;

// Un pointer la fiu.
struct ptr {
  int u;       // fiul
  char letter; // litera de pe muchie (0...SIGMA-1, nu 'a'...'z')
};

struct trie_node {
  std::vector<ptr> children; // listă de fii
  int id; // șirul terminat aici (doar pentru frunze)
};

trie_node nd[MAX_LEN]; // spațiu comun pentru toate trie-urile
int nd_ptr;

int circ(int x) {
  return (x >= SIGMA) ? (x - SIGMA) : x;
}

struct trie {
  int root;
  int klen;

  int get_child(int u, char letter) {
    for (ptr p: nd[u].children) {
      if (p.letter == letter) {
        return p.u;
      }
    }
    nd[u].children.push_back({++nd_ptr, letter});
    return nd_ptr;
  }

  void insert(int id, char* s) {
    int u = root;
    for (int i = 0; s[i]; i++) {
      u = get_child(u, s[i] - 'a');
    }
    if (!nd[u].id) { // dintre două chei identice, preferă ID-ul minim
      nd[u].id = id;
    }
  }

  // Literele fiind ordonate, returnează prima literă începînd cu cea dorită.
  ptr find_continuation(int u, char letter) {
    char want = circ(SIGMA - letter);
    for (ptr p: nd[u].children) {
      if (p.letter >= want) {
        return p;
      }
    }
    return nd[u].children[0];
  }

  // Construiește în cand[] cel mai mic candidat și returnează ID-ul cheii cu
  // care l-a obținut.
  int build_cand(char* s, int slen) {
    int u = root;
    for (int i = 0; i < klen; i++) {
      ptr p = find_continuation(u, s[i] - 'a');
      for (int j = i; j < slen; j += klen) {
        // s[i] se repetă din klen în klen caractere
        cand[j] = circ(s[j] - 'a' + p.letter) + 'a';
      }
      u = p.u;
    }
    return nd[u].id;
  }
};

trie t[MAX_ROOT];
std::unordered_map<int, int> len_to_trie;

int get_trie_for_string(char* s) {
  static int num_tries = 0;

  int len = strlen(s);
  auto it = len_to_trie.find(len);
  if (it == len_to_trie.end()) {
    t[num_tries] = { .root = ++nd_ptr, .klen = len };
    len_to_trie[len] = num_tries;
    return num_tries++;
  } else {
    return it->second;
  }
}

void read_key_strings_into_trie() {
  int k;
  scanf("%d %d ", &num_queries, &k);
  for (int i = 1; i <= k; i++) {
    scanf("%s ", s);
    int trie_id = get_trie_for_string(s);
    t[trie_id].insert(i, s);
  }
}

void sort_children() {
  for (int u = 1; u <= nd_ptr; u++) {
    std::sort(nd[u].children.begin(), nd[u].children.end(), [](ptr a, ptr b) {
      return a.letter < b.letter;
    });
  }
}

void try_length(char* s, int slen, int klen) {
  auto it = len_to_trie.find(klen);
  if (it != len_to_trie.end()) {
    int tree_id = it->second;
    int cand_id = t[tree_id].build_cand(s, slen);
    int cmp = strcmp(cand, best);
    if ((best_id == NONE) ||
        (cmp < 0) ||
        ((cmp == 0) && (cand_id < best_id))) {
      strcpy(best, cand);
      best_id = cand_id;
    }
  }
}

void process_queries() {
  while (num_queries--) {
    scanf("%s ", s);
    int len = strlen(s);
    best_id = NONE;

    for (int d = 1; d * d <= len; d++) {
      if (len % d == 0) {
        try_length(s, len, d);
        try_length(s, len, len / d);
      }
    }
    printf("%d\n", best_id);
  }
}

int main() {
  read_key_strings_into_trie();
  sort_children();
  process_queries();

  return 0;
}
