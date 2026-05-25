#include <algorithm>
#include <stdio.h>

const int MAX_STRINGS = 40'000;
const int MAX_QUERIES = 20'000;
const int MAX_LEN = 15 * 30;
const int MAX_BIG_SHELF = 30'000 * 30;

struct string {
  char* p;
  short qindex;
};

struct query {
  short lo_prefix, hi_prefix;
  short answer;
};

char buf[MAX_STRINGS][MAX_LEN + 1];
string s[MAX_STRINGS];
query q[MAX_QUERIES + 1];
char big_shelf[MAX_BIG_SHELF + 1];
int n, k, p, num_queries;

// Interclasează cele K×P litere.
void read_string(int ind, FILE* f) {
  s[ind].p = buf[ind];
  for (int i = 0; i < k; i++) {
    for (int j = 0; j < p; j++) {
      s[ind].p[k * j + i] = fgetc(f);
    }
    fgetc(f);
  }
}

// Construiește șirul explicit pentru interogare și pune-l laolaltă cu
// șirurile inițiale.
void read_query(int ind, FILE* f) {
  int score;
  fscanf(f, "%d", &score);
  q[ind].lo_prefix = k * score;
  q[ind].hi_prefix = k * (score + 1);

  int pos = n + ind - 1;
  s[pos].p = buf[pos];
  s[pos].qindex = ind;
  for (int i = 0; i < k; i++) {
    int book;
    fscanf(f, "%d", &book);
    book--;
    for (int j = 0; j < p; j++) {
      s[pos].p[k * j + i] = big_shelf[p * book + j];
    }
  }
}

void read_data() {
  int m;
  FILE* f = fopen("sabin.in", "r");
  fscanf(f, "%d %d %d %d %d ", &n, &k, &m, &p, &num_queries);
  for (int i = 0; i < n; i++) {
    read_string(i, f);
  }
  for (int i = 0; i < m; i++) {
    fscanf(f, "%s ", big_shelf + i * p);
  }
  for (int i = 1; i <= num_queries; i++) {
    read_query(i, f);
  }
  fclose(f);
}

int count_strings(int first, int last) {
  int res = 0;
  for (int i = first; i < last; i++) {
    res += (s[i].qindex == 0);
  }
  return res;
}

void answer_query(int ind, int prefix_len, int num_strings) {
  if (ind) {
    if (q[ind].lo_prefix == prefix_len) {
      q[ind].answer = num_strings;
      q[ind].lo_prefix = -1; // nu mai întreba niciodată despre acest prefix
    } else if (q[ind].hi_prefix == prefix_len) {
      q[ind].answer -= num_strings;
      q[ind].hi_prefix = -1;
    }
  }
}

void tsort(int left, int right, int pos) {
  if (right - left <= 1) {
    return;
  }

  int num_strings = count_strings(left, right);
  int i = left, lt = left, gt = right;
  char pivot = s[left].p[pos];

  while (i < gt) {
    answer_query(s[i].qindex, pos, num_strings);
    if (s[i].p[pos] < pivot) {
      std::swap(s[i++], s[lt++]);
    } else if (s[i].p[pos] > pivot)  {
      std::swap(s[i], s[--gt]);
    } else {
      i++;
    }
  }

  tsort(left, lt, pos);
  if (s[lt].p[pos] != '\0') {
    tsort(lt, gt, pos + 1);
  }
  tsort(gt, right, pos);
}

void write_answers() {
  FILE* f = fopen("sabin.out", "w");
  for (int i = 1; i <= num_queries; i++) {
    fprintf(f, "%d\n", q[i].answer);
  }
  fclose(f);
}

int main() {
  read_data();
  tsort(0, n + num_queries, 0);
  write_answers();

  return 0;
}
