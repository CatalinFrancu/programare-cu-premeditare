#include <stdio.h>

const int MAX_NODES = 300'001;
const int OP_ADD_NODE = 1;
const int OP_BUY_GOLD = 2;

struct node {
  int depth;
  int supply;
  int cost;
  int parent;
  int jump;
};

struct purchase {
  int amount;
  long long cost;
};

node nd[MAX_NODES];
int stack[MAX_NODES];
int num_queries;

int min(int x, int y) {
  return (x < y) ? x : y;
}

void create_node_zero() {
  scanf("%d %d %d", &num_queries, &nd[0].supply, &nd[0].cost);
  nd[0].depth = 1;
}

void read_node(int u) {
  scanf("%d %d %d", &nd[u].parent, &nd[u].supply, &nd[u].cost);
  int p = nd[u].parent, p2 = nd[p].jump, p3 = nd[p2].jump;
  bool equal = (nd[p2].depth - nd[p].depth == nd[p3].depth - nd[p2].depth);
  nd[u].depth = 1 + nd[p].depth;
  nd[u].jump = equal ? p3 : p;
}

void buy_from_node(int u, int& demand, purchase& p) {
  int bought = min(demand, nd[u].supply);
  demand -= bought;
  nd[u].supply -= bought;
  p.amount += bought;
  p.cost += (long long)bought * nd[u].cost;
}

purchase buy_from_path(int path_end, int demand) {
  purchase result = { 0, 0 };
  int ptr = 1;
  stack[0] = path_end;

  // Binary lifting după caz. Păstrează rezultatele pe stivă pentru a obține
  // coborîrea în O(1) amortizat.
  while (ptr && demand) {
    int u = stack[ptr - 1];
    if (u && nd[nd[u].jump].supply) {
      stack[ptr++] = nd[u].jump;
    } else if (u && nd[nd[u].parent].supply) {
      stack[ptr++] = nd[u].parent;
    } else {
      buy_from_node(u, demand, result);
      if (!nd[u].supply) {
        ptr--;
      }
    }
  }

  return result;
}

void buy_gold() {
  int u, demand;
  scanf("%d %d", &u, &demand);
  purchase p = buy_from_path(u, demand);
  printf("%d %lld\n", p.amount, p.cost);
  fflush(stdout);
}

void process_queries() {
  for (int q = 1; q <= num_queries; q++) {
    int type;
    scanf("%d", &type);
    if (type == OP_ADD_NODE) {
      read_node(q);
    } else {
      buy_gold();
    }
  }
}

int main() {
  create_node_zero();
  process_queries();

  return 0;
}
