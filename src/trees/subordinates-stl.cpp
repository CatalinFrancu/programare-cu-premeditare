#include <iostream>
#include <vector>

const int MAX_N = 200'000;

int s[MAX_N + 1]; // mărimea subarborelui, inclusiv nodul însuși
std::vector<int> c[MAX_N + 1]; // c[u] = fiii lui u
int n;

void read_data() {
  std::cin >> n;
  for (int u = 2; u <= n; u++) {
    int v;
    std::cin >> v;
    c[v].push_back(u);
  }
}

void depth_first_search(int u) {
  s[u] = 1;
  for (int v: c[u]) {
    depth_first_search(v);
    s[u] += s[v];
  }
}

void write_answers() {
  for (int u = 1; u <= n; u++) {
    std::cout << (s[u] - 1) << ' ';
  }
  std::cout << '\n';
}

int main() {
  read_data();
  depth_first_search(1);
  write_answers();

  return 0;
}
