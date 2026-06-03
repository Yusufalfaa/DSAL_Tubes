// include/benchmark.hpp
#ifndef BENCHMARK_HPP
#define BENCHMARK_HPP

#include <string>
#include <unordered_map>
using namespace std;

// =====================
// Tabel optimum benchmark Lawrence (LA) dan lainnya
// Sumber: Lawrence (1984), diverifikasi komunitas riset JSSP
// Nilai -1 berarti optimum belum diketahui / belum dibuktikan
// =====================
static const unordered_map<string, int> BENCHMARK_OPTIMUM = {
    // LA01-05 (10 jobs x 5 machines)
    { "la01", 666  },
    { "la02", 655  },
    { "la03", 597  },
    { "la04", 590  },
    { "la05", 593  },
    // LA06-10 (15 jobs x 5 machines)
    { "la06", 926  },
    { "la07", 890  },
    { "la08", 863  },
    { "la09", 951  },
    { "la10", 958  },
    // LA11-15 (20 jobs x 5 machines)
    { "la11", 1222 },
    { "la12", 1039 },
    { "la13", 1150 },
    { "la14", 1292 },
    { "la15", 1207 },
    // LA16-20 (10 jobs x 10 machines)
    { "la16", 945  },
    { "la17", 784  },
    { "la18", 848  },
    { "la19", 842  },
    { "la20", 902  },
};

// =====================
// Fungsi: getOptimum
// Deskripsi: Mengambil nilai optimum benchmark berdasarkan nama dataset.
//            Nama dataset di-lowercase otomatis agar case-insensitive.
//            Mengembalikan -1 jika dataset tidak ditemukan di tabel.
// Input:  nama dataset (contoh: "la01", "LA16")
// Output: nilai optimum makespan, atau -1 jika tidak diketahui
// =====================
inline int getOptimum(const string& name) {
    string key = name;
    for (char& c : key) c = tolower(c);

    auto it = BENCHMARK_OPTIMUM.find(key);
    if (it != BENCHMARK_OPTIMUM.end()) return it->second;
    return -1;
}

// =====================
// Fungsi: computeGap
// Deskripsi: Menghitung optimality gap dalam persen terhadap nilai optimum benchmark.
//            Gap = ((makespan - optimum) / optimum) x 100
//            Mengembalikan -1.0 jika optimum tidak diketahui.
// Input:  makespan hasil algoritma, nilai optimum benchmark
// Output: gap dalam persen, atau -1.0 jika optimum tidak tersedia
// =====================
inline double computeGap(int makespan, int optimum) {
    if (optimum <= 0) return -1.0;
    return ((double)(makespan - optimum) / optimum) * 100.0;
}

#endif // BENCHMARK_HPP