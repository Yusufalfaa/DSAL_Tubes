# Job Shop Scheduling Optimization using Greedy Critical Path Priority and Branch and Bound

Nama  : Muhamad Yusuf Al Farizzi  
NIM   : 2030212520052

## Deskripsi Permasalahan

Job Shop Scheduling Problem (JSSP) merupakan salah satu permasalahan optimasi kombinatorial yang bertujuan menentukan jadwal pelaksanaan sejumlah operasi pada beberapa mesin sehingga total waktu penyelesaian seluruh pekerjaan (*makespan*) menjadi minimum.

Pada JSSP, setiap *job* terdiri atas serangkaian operasi yang harus dikerjakan secara berurutan pada mesin tertentu dengan waktu proses yang telah ditentukan. Setiap mesin hanya dapat mengerjakan satu operasi pada satu waktu, dan setiap operasi tidak dapat dimulai sebelum operasi sebelumnya dalam *job* yang sama selesai dikerjakan.

Karena jumlah kemungkinan jadwal bertumbuh secara eksponensial terhadap jumlah *job* dan mesin, pencarian solusi optimal menjadi semakin sulit untuk ukuran permasalahan yang lebih besar.

---

## Tujuan Penelitian

Penelitian ini bertujuan untuk:

1. Mengimplementasikan algoritma **Greedy Critical Path Priority (Most Work Remaining)** sebagai pendekatan konstruktif untuk menghasilkan solusi scheduling secara cepat.
2. Mengimplementasikan algoritma **Branch and Bound** sebagai metode *exact optimization* yang memanfaatkan teknik pruning untuk mengurangi ruang pencarian.
3. Membandingkan performa kedua algoritma berdasarkan:
   - Makespan yang dihasilkan
   - Waktu komputasi (*runtime*)
   - Jumlah state/node yang dieksplorasi
   - Kualitas solusi terhadap nilai optimum benchmark

---

## Algoritma yang Digunakan

### 1. Greedy Priority Scheduling based on Most Work Remaining (MWR)

Algoritma Greedy membangun jadwal secara bertahap dengan memilih operasi yang berasal dari *job* dengan total waktu pekerjaan tersisa (*remaining work*) terbesar.

Prioritas suatu *job* dihitung menggunakan:

```text
Priority(Job_i) = Σ ProcessingTime yang belum dikerjakan
```

Pada setiap langkah, operasi yang siap dijadwalkan dan memiliki nilai prioritas terbesar akan dipilih terlebih dahulu.

#### Karakteristik

- Pendekatan konstruktif (*constructive scheduling*)
- Kompleksitas relatif rendah
- Runtime cepat
- Tidak menjamin solusi optimal

---

### 2. Branch and Bound

Branch and Bound membentuk *state-space tree* yang merepresentasikan berbagai kemungkinan penjadwalan.

Setiap node merepresentasikan *partial schedule*. Untuk setiap node dihitung nilai *lower bound* sebagai estimasi makespan minimum yang masih mungkin dicapai dari state tersebut.

Jika *lower bound* suatu node lebih buruk daripada solusi terbaik yang telah ditemukan, maka subtree tersebut dipangkas (*pruned*) sehingga tidak perlu dieksplorasi lebih lanjut.

#### Karakteristik

- Exact optimization algorithm
- Menjamin solusi optimal jika pencarian selesai
- Menggunakan teknik pruning
- Kompleksitas eksponensial pada kasus terburuk

---

## Dataset

Dataset yang digunakan adalah benchmark **Lawrence (LA)** untuk Job Shop Scheduling Problem.

### Dataset yang Digunakan

| Dataset | Jobs | Machines | Optimum |
|----------|------|-----------|----------|
| LA01 | 10 | 5 | 666 |
| LA06 | 15 | 5 | 926 |
| LA16 | 10 | 10 | 945 |

Format dataset:

```text
Machine ProcessingTime Machine ProcessingTime ...
```

Contoh:

```text
1 21 0 53 4 95 3 55 2 34
```

Interpretasi:

| Operasi | Mesin | Waktu Proses |
|----------|--------|-------------|
| O1 | M1 | 21 |
| O2 | M0 | 53 |
| O3 | M4 | 95 |
| O4 | M3 | 55 |
| O5 | M2 | 34 |

Setiap baris merepresentasikan satu *job*, sedangkan setiap pasangan angka menunjukkan:

- Machine ID
- Processing Time

Urutan pasangan menunjukkan urutan operasi yang wajib dikerjakan.

---

## Metodologi Eksperimen

Untuk setiap dataset:

1. Jalankan algoritma Greedy Critical Path Priority.
2. Jalankan algoritma Branch and Bound.
3. Catat hasil:
   - Makespan
   - Runtime (ms)
   - Jumlah node yang dieksplorasi
   - Persentase gap terhadap nilai optimum

Gap dihitung menggunakan:

```text
Gap(%) = ((Makespan - Optimum) / Optimum) × 100
```

---

## Metrik Evaluasi

### 1. Makespan

Waktu penyelesaian seluruh job.

```text
Cmax = max completion time across all jobs
```

Semakin kecil makespan, semakin baik kualitas solusi.

### 2. Runtime

Waktu yang diperlukan algoritma untuk menghasilkan solusi.

### 3. Expanded Nodes

Jumlah state/node yang dieksplorasi selama proses pencarian.

### 4. Optimality Gap

Selisih hasil algoritma terhadap nilai optimum benchmark.

---

## Hasil yang Diharapkan

Diharapkan algoritma **Greedy Critical Path Priority** mampu menghasilkan solusi dengan runtime yang sangat cepat namun kualitas solusi sedikit lebih rendah dibandingkan optimum.

Sebaliknya, algoritma **Branch and Bound** diharapkan mampu menghasilkan solusi yang lebih baik atau optimal melalui eksplorasi ruang solusi yang lebih luas, meskipun membutuhkan waktu komputasi yang jauh lebih besar.

Penelitian ini bertujuan menunjukkan trade-off antara kualitas solusi dan biaya komputasi dalam penyelesaian Job Shop Scheduling Problem menggunakan dua pendekatan algoritmik yang memiliki karakteristik dan kompleksitas yang berbeda.