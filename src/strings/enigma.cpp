#include <stdio.h>

const int MAX_N = 300'000;
const int MAX_M = 4'500;
const int MAX_S = 100;
const int SIGMA = 26;
const int MOD = 31'333;

// Un trie cu rădăcina 1. Nodul 0 este nefolosit.
struct trie {
  struct node {
    int child[SIGMA];
    int cnt;
  };

  node nd[MAX_M * MAX_S + 1];
  int size = 1;

  void insert(char* s) {
    int u = 1;
    for (int i = 0; s[i]; i++) {
      int c = s[i] - 'a';
      if (!nd[u].child[c]) {
        nd[u].child[c] = ++size;
      }
      u = nd[u].child[c];
      nd[u].cnt++;
    }
  }
};

char s[MAX_N + 2]; // indexat de la 1
int ways[MAX_N + 1];
trie t;
int n;

void read_and_build_trie() {
  FILE* f = fopen("enigma.in", "r");
  int num_words;
  char word[MAX_S + 1];

  fscanf(f, "%d %d %s", &n, &num_words, s + 1);
  while (num_words--) {
    fscanf(f, "%s", word);
    t.insert(word);
  }
  fclose(f);
}

void dyn_prog() {
  ways[0] = 1;
  for (int i = 0; i <= n; i++) {
    ways[i] %= MOD; // o singură dată, după ce am adunat toate contribuțiile
    int j = i + 1;
    int u = 1;
    while (s[j] && (u = t.nd[u].child[s[j] - 'a'])) {
      ways[j++] += ways[i] * t.nd[u].cnt;
    }
  }
}

void write_answer() {
  FILE* f = fopen("enigma.out", "w");
  fprintf(f, "%d\n", ways[n]);
  fclose(f);
}

int main() {
  read_and_build_trie();
  dyn_prog();
  write_answer();

  return 0;
}
