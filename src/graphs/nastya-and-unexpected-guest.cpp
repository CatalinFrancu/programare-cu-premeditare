#include <algorithm>
#include <stdio.h>

const int MAX_ISLANDS = 10'000;
const int MAX_GREEN = 1'000;
const int INFINITY = 1'000'000'000;

struct cell {
  short island, rem;
  int next;
};

int d[MAX_ISLANDS + 2][MAX_GREEN];
int x[MAX_ISLANDS + 2];

struct bucket_queue {
  cell list[MAX_ISLANDS * MAX_GREEN]; // buffer comun
  int list_ptr; // următorul element disponibil in bufferul comun

  int head[MAX_GREEN + 1]; // începuturi de bloc
  int pos; // blocul curent
  int n; // numărul de blocuri
  int size; // numărul de elemente în toate blocurile

  void init(int n) {
    this->n = n;
  }

  bool is_empty() {
    return !size;
  }

  void push(int steps_ahead, short island, short rem) {
    int p = (pos + steps_ahead) % n;
    list[++list_ptr] = { island, rem, head[p] };
    head[p] = list_ptr;
    size++;
  }

  cell pop() {
    while (!head[pos]) {
      pos = (pos + 1) % n;
    }

    size--;
    int h = head[pos];
    head[pos] = list[h].next;
    return list[h];
  }

};

bucket_queue bq;
int target, num_islands;
int green_duration, red_duration;

void read_data() {
  scanf("%d %d", &target, &num_islands);
  for (int i = 1; i <= num_islands; i++) {
    scanf("%d", &x[i]);
  }
  scanf("%d %d", &green_duration, &red_duration);
}

void preprocess_coords() {
  std::sort(x + 1, x + num_islands + 1);
  x[0] = x[1] - (green_duration + 1);
  x[num_islands + 1] = x[num_islands] + (green_duration + 1);
}

void init_distances() {
  for (int i = 1; i <= num_islands; i++) {
    for (int rem = 0; rem < green_duration; rem++) {
      d[i][rem] = INFINITY;
    }
  }
  d[1][0] = 0;
}

void try_hop(short src, short dest, short src_rem, int cost) {
  int dest_rem = src_rem + cost;
  if (dest_rem > green_duration) {
    return;
  } else if (dest_rem == green_duration) {
    dest_rem = 0;
  }

  if (d[src][src_rem] + cost < d[dest][dest_rem]) {
    d[dest][dest_rem] = d[src][src_rem] + cost;
    bq.push(cost, dest, dest_rem);
  }
}

void dial() {
  init_distances();
  bq.init(green_duration + 1);
  bq.push(0, 1, 0);

  while (!bq.is_empty()) {
    cell c = bq.pop();
    try_hop(c.island, c.island - 1, c.rem, x[c.island] - x[c.island - 1]);
    try_hop(c.island, c.island + 1, c.rem, x[c.island + 1] - x[c.island]);
  }

}

void write_min_distance() {
  int min = INFINITY;
  for (int rem = 0; rem < green_duration; rem++) {
    if (d[num_islands][rem] < min) {
      min = d[num_islands][rem];
    }
  }

  int answer = -1;
  if (min < INFINITY) {
    answer = min + (min  - 1) / green_duration * red_duration;
  }
  printf("%d\n", answer);
}

int main() {
  read_data();
  preprocess_coords();
  dial();
  write_min_distance();
  return 0;
}
