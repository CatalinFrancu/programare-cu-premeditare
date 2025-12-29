// Complexitate: O(n + q).
#include <stdio.h>

const int MAX_NODES = 100'000;
const int MAX_QUERIES = 100'000;

struct cell {
  int v, next;
};

struct query_cell {
  int v, p, answer, next;
};

struct node {
  int adj;
  int qptr; // pointer în lista de interogări
};

cell list[MAX_NODES + 1];
query_cell q[MAX_QUERIES + 1];
node nd[MAX_NODES + 1];
int stack[MAX_NODES];
int n, num_queries;

void read_input_data() {
  int list_ptr = 1;

  scanf("%d", &n);

  // Pseudonodul 0 va avea toate rădăcinile drept fii.
  for (int u = 1; u <= n; u++) {
    int p;
    scanf("%d", &p);
    list[list_ptr] = { u, nd[p].adj };
    nd[p].adj = list_ptr++;
  }

  scanf("%d", &num_queries);
  for (int i = 1; i <= num_queries; i++) {
    scanf("%d %d", &q[i].v, &q[i].p);
  }
}

void distribute_queries() {
  for (int u = 1; u <= n; u++) {
    nd[u].qptr = 0;
  }
  for (int i = 1; i <= num_queries; i++) {
    if (q[i].v) { // poate fi 0 după primul DFS
      q[i].next = nd[q[i].v].qptr;
      nd[q[i].v].qptr = i;
    }
  }
}

void replace_query_nodes_dfs(int u, int depth) {
  stack[depth] = u;

  // Scanează toate interogările despre u și înlocuiește nodul cu al p-lea său
  // strămoș.
  for (int ptr = nd[u].qptr; ptr; ptr = q[ptr].next) {
    q[ptr].v = (depth > q[ptr].p)
      ? stack[depth - q[ptr].p]
      : 0;
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    replace_query_nodes_dfs(list[ptr].v, depth + 1);
  }

  stack[depth] = 0; // curățenie pentru al doilea DFS
}

void answer_queries_dfs(int u, int depth) {
  stack[depth]++;

  // Scanează toate interogările despre u și reține contorul inițial.
  for (int ptr = nd[u].qptr; ptr; ptr = q[ptr].next) {
    q[ptr].answer = stack[depth + q[ptr].p];
  }

  for (int ptr = nd[u].adj; ptr; ptr = list[ptr].next) {
    answer_queries_dfs(list[ptr].v, depth + 1);
  }

  // Rescanează interogările și reține diferența între valorile contorului.
  for (int ptr = nd[u].qptr; ptr; ptr = q[ptr].next) {
    q[ptr].answer = stack[depth + q[ptr].p] - q[ptr].answer - 1;
  }
}

void write_output_data() {
  for (int i = 1; i <= num_queries; i++) {
    printf("%d ", q[i].answer);
  }
  printf("\n");
}

int main() {
  read_input_data();
  distribute_queries();
  replace_query_nodes_dfs(0, 0);
  distribute_queries();
  answer_queries_dfs(0, 0);
  write_output_data();

  return 0;
}
