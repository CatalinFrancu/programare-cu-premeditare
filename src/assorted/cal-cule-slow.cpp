// Soluție neoptimă. Folosește doar valorile 0 și 1 indiferent de k. Poate
// ajunge la 8.000 de interogări.
#include "calcule.h"

#include <algorithm>
#include <vector>

// Returnează 1 (true) dacă și numai dacă există elemente duplicate.
int dreseaza_cal(int n, int k, std::vector<int> frecvente_sortate) {
  return frecvente_sortate.back() == 2;
}

// Pune o întrebare despre pos[0...len] + indicele extra.
bool make_query(int n, std::vector<int> pos, int len, int extra) {
  std::vector<int> query = std::vector<int>(pos.begin(), pos.begin() + len + 1);
  query.push_back(extra);
  return intreaba_cal(query);
}

// Caută binar valoarea de la poziția ind printre primele apariții ale
// elementelor cunoscute.
int find_previous_occurrence_of(int ind, int n, std::vector<int>& pos) {
  int lo = -1, hi = pos.size(); // (lo, hi]
  while (hi - lo > 1) {
    int mid = (lo + hi) / 2;
    int has_duplicates = make_query(n, pos, mid, ind);
    if (has_duplicates) {
      hi = mid;
    } else {
      lo = mid;
    }
  }

  return hi;
}

void scan_input(int n, std::vector<int>& pos, std::vector<int>& freq) {
  // Află dacă valoarea input[i] este nouă sau dacă am întîlnit-o anterior, și
  // dacă da, pe ce poziție.
  for (int i = 1; i < n; i++) {
    int prev = find_previous_occurrence_of(i, n, pos);
    if (prev < (int)pos.size()) {
      freq[prev]++;
    } else {
      pos.push_back(i);
      freq.push_back(1);
    }
  }
}

std::vector<int> descuie_grajdul(int n, int k) {
  // Nu cunoaștem valorile din vector, așa că le vom boteza 0, 1, 2, ... în
  // ordinea primei lor apariții. De exemplu, vom boteza „0” primul element.
  std::vector<int> pos;  // poziția primei apariții
  std::vector<int> freq; // frecvența

  pos.push_back(0);
  freq.push_back(1);

  scan_input(n, pos, freq);

  std::sort(freq.begin(), freq.end());
  return freq;
}
