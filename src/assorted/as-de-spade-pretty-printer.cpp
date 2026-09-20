#include <iostream>
#include <vector>
#include <algorithm>
#include <string>

int main() {
  long long a, b;
  std::cin >> a >> b;

  std::vector<std::string> v(b - a + 1);
  for (long long i = a; i <= b; i++) {
    v[i - a] = std::to_string(i);
  }

  std::cerr << "Sortez...\n";
  std::sort(v.begin(), v.end(), [](std::string& a, std:: string& b) {
    return a + b > b + a || (a + b == b + a && a > b);
  });

  int len = 0;
  for (int i = 0; i < (int)v.size(); i++) {
    std::cout << ':' << v[i] << " @ " << len << '\n';
    len += v[i].size();
  }
  std::cout << "\n";

  return 0;
}
