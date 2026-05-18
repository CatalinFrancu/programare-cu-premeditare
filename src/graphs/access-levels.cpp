#include <stdio.h>

const int MAX_USERS = 500;
const int MAX_DOCUMENTS = 500;
const int NONE = -1;

int num_users, num_documents;

struct t_document {
  bool users[MAX_USERS];
  bool incl[MAX_DOCUMENTS];

  // Date pentru algoritmul lui Kuhn
  short match;    // cu ce document a fost împerecheat
  bool has_match; // true dacă și numai dacă alt document ne consideră pereche
  bool vis;       // bit de vizitare

  short group, level;

  bool is_included_in(t_document& other, bool tiebreaker) {
    bool equal = true;
    for (int u = 0; u < num_users; u++) {
      if (users[u] > other.users[u]) {
        return false;
      }
      equal &= (users[u] == other.users[u]);
    }
    return !equal || tiebreaker;
  }
};

struct t_user {
  int level[MAX_DOCUMENTS];
};

t_document doc[MAX_DOCUMENTS];
t_user user[MAX_USERS];

void read_data() {
  scanf("%d %d ", &num_users, &num_documents);
  for (int u = 0; u < num_users; u++) {
    for (int d = 0; d < num_documents; d++) {
      doc[d].users[u] = (getchar() == '1');
    }
    getchar();
  }
}

void compute_inclusions() {
  for (int i = 0; i < num_documents; i++) {
    for (int j = 0; j < num_documents; j++) {
      doc[i].incl[j] = doc[i].is_included_in(doc[j], (i < j));
    }
  }
}

void reset_visited() {
  for (int i = 0; i < num_documents; i++) {
    doc[i].vis = false;
  }
}

bool kuhn_dfs(int u) {
  if (doc[u].vis) {
    return false;
  }

  doc[u].vis = true;
  for (int v = 0; v < num_documents; v++) {
    if (doc[u].incl[v]) {
      if ((doc[v].match == NONE) || kuhn_dfs(doc[v].match)) {
        doc[v].match = u;
        return true;
      }
    }
  }

  return false;
}

void kuhn_matching() {
  for (int d = 0; d < num_documents; d++) {
    doc[d].match = NONE;
  }

  for (int d = 0; d < num_documents; d++) {
    reset_visited();
    kuhn_dfs(d);
  }
}

void build_group(int d, int group) {
  // Dă-le utilizatorilor nivelul 1, iar documentelor niveluri începînd cu 2.
  // Astfel, implicit nimeni nu poate vedea nimic.
  for (int u = 0; u < num_users; u++) {
    user[u].level[group] = 1;
  }

  int level = 2;
  while (d != NONE) {
    for (int u = 0; u < num_users; u++) {
      if (doc[d].users[u]) {
        // Oferă acces doar la nevoie.
        user[u].level[group] = level;
      }
    }
    doc[d].group = group;
    doc[d].level = level++;
    d = doc[d].match;
  }
}

int build_groups() {
  for (int d = 0; d < num_documents; d++) {
    if (doc[d].match != NONE) {
      doc[doc[d].match].has_match = true;
    }
  }

  int group = 0;
  for (int d = 0; d < num_documents; d++) {
    if (!doc[d].has_match) {
      build_group(d, group++);
    }
  }

  return group;
}

void write_solution(int num_groups) {
  printf("%d\n", num_groups);

  for (int d = 0; d < num_documents; d++) {
    printf("%d ", doc[d].group + 1);
  }
  printf("\n");

  for (int d = 0; d < num_documents; d++) {
    printf("%d ", doc[d].level);
  }
  printf("\n");

  for (int u = 0; u < num_users; u++) {
    for (int g = 0; g < num_groups; g++) {
      printf("%d ", user[u].level[g]);
    }
    printf("\n");
  }
}

int main() {
  read_data();
  compute_inclusions();
  kuhn_matching();
  int num_groups = build_groups();
  write_solution(num_groups);

  return 0;
}
