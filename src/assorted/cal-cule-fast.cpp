#include "calcule.h"

#include <vector>

typedef std::vector<int> vector;

const int MAX_N = 1'000;

// Returnează numărul de perechi egale, modulo k.
int dreseaza_cal(int n, int k, vector frecvente_sortate) {
  int s = 0;
  for (int f: frecvente_sortate) {
    s += f * (f - 1) / 2;
  }
  return s % k;
}

struct freq_info {
  int cnt;    // Cîte elemente au această frecvență?
  vector pos; // La ce poziții a atins vreun element această frecvență?
};

struct range {
  int lo_f, hi_f;
};

// O structură care menține situația frecvențelor. Funcția extend() face
// interogările necesare pentru a încorpora încă un element.
struct guesser {
  int n, k;
  freq_info freq[MAX_N + 1];
  range rng[MAX_N];
  int lo_f, hi_f;     // frecvențele-candidat pe durata lui extend()
  int pairs_in_query; // perechi cunoscute printre pozițiile puse în interogare

  guesser(int n, int k) {
    this->n = n;
    this->k = k;
    init_freq();
  }

  void init_freq() {
    for (int i = 0; i < n; i++) {
      freq[i].cnt = 0;
    }
    freq[0].cnt = n - 1; // pozițiile nu ne trebuie
    freq[1].cnt = 1;
    freq[1].pos.push_back(0);
  }

  int count_distinct_freqs() {
    int cnt = 0;
    for (int f = lo_f; f <= hi_f; f++) {
      cnt += (freq[f].cnt > 0);
    }
    return cnt;
  }

  void add_to_query(int f, int freq_in_query, vector& dest) {
    if (freq_in_query) {
      for (int p: freq[f].pos) {
        dest.push_back(p);
        pairs_in_query += freq_in_query - 1;
      }
    }
  }

  int find_next_nonzero_freq(int f) {
    do {
      f++;
    } while ((f <= n) && !freq[f].cnt);
    return f;
  }

  vector build_query(int curr_pos) {
    vector result;

    pairs_in_query = 0;
    int num_dist = count_distinct_freqs();
    int f = lo_f;
    for (int rem = 0; rem < k; rem++) {
      int batch_size = num_dist / k + (rem < num_dist % k);
      if (batch_size) {
        add_to_query(f, rem, result);
        rng[rem].lo_f = f;
        while (batch_size) {
          rng[rem].hi_f = f;
          f = find_next_nonzero_freq(f);
          batch_size--;
        }
      }
    }

    result.push_back(curr_pos);
    return result;
  }

  // Ținînd cont de răspunsul calului și de numărul cunoscut de perechi,
  // restrînge intervalul de frecvențe-candidat.
  void interpret_response(int resp) {
    int growth = (resp + k - (pairs_in_query % k)) % k;
    lo_f = rng[growth].lo_f;
    hi_f = rng[growth].hi_f;
  }

  void update_frequencies(int curr_pos) {
    freq[lo_f].cnt--;
    freq[lo_f + 1].cnt++;
    freq[lo_f + 1].pos.push_back(curr_pos);
  }

  void extend(int curr_pos) {
    lo_f = 0;
    hi_f = n;

    while (lo_f != hi_f) {
      vector query = build_query(curr_pos);
      int resp = intreaba_cal(query);
      interpret_response(resp);
    }

    update_frequencies(curr_pos);
  }

  vector get_sorted_frequencies() {
    vector result;
    for (int i = 1; i < n; i++) {
      for (int j = 0; j < freq[i].cnt; j++) {
        result.push_back(i);
      }
    }
    return result;
  }
};

vector descuie_grajdul(int n, int k) {
  guesser g(n, k);

  for (int i = 1; i < n; i++) {
    g.extend(i);
  }

  return g.get_sorted_frequencies();
}
