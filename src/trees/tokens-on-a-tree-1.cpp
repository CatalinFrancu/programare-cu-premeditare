#include <stdio.h>

const int MAX_NODES = 1'000'000;

struct cell {
  int f, prev, next;
};

cell list[MAX_NODES + 1];
int list_ptr;
long long total_moves;

void swap(int& x, int& y) {
  int tmp = x;
  x = y;
  y = tmp;
}

struct node {
  int parent, num_children;
  int head, tail, length;
  bool has_coin;

  void reset(bool has_coin) {
    this->has_coin = has_coin;
    num_children = head = tail = length = 0;
  }

  // Procesează un nod după ce toți fiii săi i-au transmis datele lor.
  void process(node& par) {
    prepend_self();
    add_or_move_coin();
    trim();
    spill_into(par);
  }

  void prepend_self() {
    list[list_ptr].f = 0;
    list[list_ptr].prev = 0;
    list[list_ptr].next = head;
    if (head) {
      list[head].prev = list_ptr;
    } else {
      tail = list_ptr;
    }
    head = list_ptr++;
    length++;
  }

  void add_or_move_coin() {
    if (has_coin) {
      list[head].f = 1;
    } else if (list[tail].f) {
      list[head].f = 1;
      list[tail].f--;
      total_moves += length - 1;
    }
  }

  void trim() {
    while ((tail != head) && !list[tail].f) {
      tail = list[tail].prev;
      list[tail].next = 0;
      length--;
    }
  }

  void spill_into(node& other) {
    if (length > other.length) {
      swap_lists(other);
    }

    for (int p = head, q = other.head;
         p;
         p = list[p].next, q = list[q].next)  {
      list[q].f += list[p].f;
    }
  }

  void swap_lists(node& other) {
    swap(head, other.head);
    swap(tail, other.tail);
    swap(length, other.length);
  }
};

node nd[MAX_NODES + 1];
int n;

void read_input_data() {
  list_ptr = 1;

  scanf("%d ", &n);

  for (int u = 1; u <= n; u++) {
    int c = getchar();
    nd[u].reset(c == '1');
  }

  for (int u = 2; u <= n; u++) {
    scanf("%d", &nd[u].parent);
    nd[nd[u].parent].num_children++;
  }
}

void traverse() {
  for (int u = 1; u <= n; u++) {
    int p = u;
    while (p && !nd[p].num_children) {
      nd[p].process(nd[nd[p].parent]);
      p = nd[p].parent;
      nd[p].num_children--;
    }
  }
}

void run_test() {
  read_input_data();
  total_moves = 0;
  traverse();
  printf("%lld\n", total_moves);
}

int main() {
  int num_tests;
  scanf("%d", &num_tests);
  while (num_tests--) {
    run_test();
  }

  return 0;
}
