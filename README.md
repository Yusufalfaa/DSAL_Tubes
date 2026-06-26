# Job Shop Scheduling Optimization using Greedy MWR and Branch and Bound

Nama  : Muhamad Yusuf Al Farizzi  
NIM   : 2030212520052

## Instruksi Menjalankan Program

### 1. Build Program

1. Buka file `main.cpp`.
2. Tekan **Ctrl + Shift + B** untuk melakukan proses build.
3. Jika build berhasil, jalankan program melalui terminal:

```bash
.\output\main.exe
```

> Pastikan terminal berada pada root folder project.

---

### 2. Mengganti Dataset

Dataset yang digunakan dapat diubah melalui `main.cpp`.

Ubah bagian berikut:

```cpp
Dataset data;
string datasetName = "la01";
string path = "datasets/" + datasetName + ".txt";
```

Sesuaikan nilai `datasetName` dengan nama file yang tersedia pada folder `datasets/`.

Contoh:

```cpp
datasetName = "la01";
datasetName = "la06";
datasetName = "la16";
```

Folder dataset:

```text
datasets/
├── la01.txt
├── la06.txt
└── la16.txt
```

Setelah mengganti dataset, lakukan **build ulang** (`Ctrl + Shift + B`) kemudian jalankan kembali:

```bash
.\output\main.exe
```

---

## Deskripsi Permasalahan

Job Shop Scheduling Problem (JSSP) merupakan salah satu permasalahan optimasi kombinatorial yang bertujuan menentukan jadwal pelaksanaan sejumlah operasi pada beberapa mesin sehingga total waktu penyelesaian seluruh pekerjaan (*makespan*) menjadi minimum.

Pada JSSP, setiap *job* terdiri atas serangkaian operasi yang harus dikerjakan secara berurutan pada mesin tertentu dengan waktu proses yang telah ditentukan. Setiap mesin hanya dapat mengerjakan satu operasi pada satu waktu, dan setiap operasi tidak dapat dimulai sebelum operasi sebelumnya dalam *job* yang sama selesai dikerjakan.

Karena jumlah kemungkinan jadwal bertumbuh secara eksponensial terhadap jumlah *job* dan mesin, pencarian solusi optimal menjadi semakin sulit untuk ukuran permasalahan yang lebih besar.

---

## Tujuan Penelitian

1. Mengimplementasikan algoritma **Greedy Priority Scheduling based on Most Work Remaining (MWR)** sebagai pendekatan konstruktif untuk menghasilkan solusi scheduling secara cepat.
2. Mengimplementasikan algoritma **Branch and Bound** dengan pendekatan *Best-First Search* dan *conflict-based branching* sebagai metode *exact optimization* yang memanfaatkan teknik pruning untuk mengurangi ruang pencarian.
3. Menggunakan solusi greedy sebagai *initial upper bound* untuk Branch and Bound agar pruning lebih agresif sejak node pertama.
4. Membandingkan performa kedua algoritma berdasarkan:
   - Makespan yang dihasilkan
   - Waktu komputasi (*runtime*)
   - Jumlah node yang dieksplorasi dan dipruning
   - Kualitas solusi terhadap nilai optimum benchmark (*optimality gap*)

---

## Algoritma yang Digunakan

### 1. Greedy Priority Scheduling — Most Work Remaining (MWR)

Algoritma Greedy membangun jadwal secara bertahap dengan memilih operasi yang berasal dari *job* dengan total sisa waktu pekerjaan (*remaining work*) terbesar.

Prioritas suatu *job* dihitung menggunakan:

```text
Priority(Job_i) = Σ ProcessingTime operasi yang belum dikerjakan
```

Pada setiap langkah, operasi yang siap dijadwalkan dan memiliki nilai prioritas terbesar akan dipilih terlebih dahulu. Hasilnya digunakan langsung sebagai *initial upper bound* untuk algoritma Branch and Bound.

#### Karakteristik

- Pendekatan konstruktif (*constructive scheduling*)
- Kompleksitas rendah, runtime sangat cepat
- Tidak menjamin solusi optimal

---

### 2. Branch and Bound — Best-First Search dengan Conflict-Based Branching

Branch and Bound membentuk *state-space tree* yang merepresentasikan berbagai kemungkinan penjadwalan. Eksplorasi dilakukan dengan strategi *Best-First Search* — node dengan *lower bound* terkecil selalu dieksplorasi lebih dulu.

#### State

Setiap node dalam pohon pencarian direpresentasikan sebagai `State` yang menyimpan:

- `jobProgress` — indeks operasi berikutnya tiap job
- `jobReadyTime` — waktu job bisa mulai operasi berikutnya
- `machineReadyTime` — waktu mesin selesai dan siap dipakai
- `currentMakespan` — makespan aktual sampai titik ini
- `lowerBound` — estimasi makespan minimum dari titik ini
- `schedule` — daftar operasi yang sudah dijadwalkan

#### Lower Bound

Untuk setiap node dihitung *lower bound* menggunakan dua komponen yang digabung dengan `max`:

**Job Bound** — untuk setiap job, jumlahkan sisa *processing time* dari operasi yang belum dijadwalkan ditambah waktu job tersebut sudah siap:

```text
JobBound(j) = jobReadyTime[j] + Σ processingTime(op yang belum selesai di job j)
```

**Machine Load Bound** — untuk setiap mesin, jumlahkan sisa beban kerja dari semua operasi yang belum dijadwalkan di mesin tersebut, ditambah waktu mesin tersebut sudah siap:

```text
MachineBound(m) = machineReadyTime[m] + Σ processingTime(semua op sisa yang butuh mesin m)
```

Nilai *lower bound* akhir:

```text
LB = max(currentMakespan, max(JobBound), max(MachineBound))
```

#### Conflict-Based Branching

Branching tidak dilakukan per-job, melainkan per *conflict machine* — mesin yang paling banyak diperebutkan oleh job-job yang siap dijadwalkan.

Langkah-langkah `generateChildren`:

1. Kumpulkan semua operasi yang *ready* (next operation tiap job yang belum selesai)
2. Hitung berapa job yang bersaing di tiap mesin (*conflict count*)
3. Pilih *conflict machine* — mesin dengan conflict count terbesar; jika seri, pilih mesin dengan sisa beban terbesar sebagai tie-breaker
4. Untuk setiap job yang bersaing di conflict machine tersebut, buat satu child: job itu dijadwalkan duluan, operasi lain yang tidak berkonflik langsung dijadwalkan di child yang sama

Jumlah child per node = jumlah job yang bersaing di conflict machine (umumnya 2–5), jauh lebih kecil dibanding pendekatan per-job konvensional.

#### Pruning

Node dipruning jika *lower bound*-nya **lebih besar** dari `bestMakespan` saat ini:

```text
if (lowerBound > bestMakespan) → pruning
```

Kondisi menggunakan `>` (bukan `>=`) agar state dengan `lowerBound == bestMakespan` tetap dieksplorasi sebagai kandidat solusi valid.

#### Batas Eksplorasi

Untuk mencegah *hang* pada instance besar, eksplorasi dibatasi oleh:

- **Node limit**: 2.000.000 node
- **Time limit**: 60 detik

Jika salah satu batas tercapai, dikembalikan solusi *best-so-far* dengan flag `isOptimal = false`.

#### Karakteristik

- *Exact optimization* — menjamin solusi optimal jika eksplorasi selesai
- *Upper bound* awal dari greedy membuat pruning lebih agresif
- Conflict-based branching menghasilkan pohon pencarian yang jauh lebih sempit
- Kompleksitas eksponensial pada kasus terburuk

---

## Dataset

Dataset yang digunakan adalah benchmark **Lawrence (LA)** untuk Job Shop Scheduling Problem.

Link Dataset: https://github.com/tamy0612/JSPLIB/tree/master  
Link Penting Terkait Dataset: https://jsp-instance-utils.readthedocs.io/en/latest/

### Dataset yang Digunakan

| Dataset | Jobs | Machines | Optimum |
|---------|------|----------|---------|
| LA01    | 10   | 5        | 666     |
| LA06    | 15   | 5        | 926     |
| LA16    | 10   | 10       | 945     |

### Format Dataset

```text
Machine ProcessingTime Machine ProcessingTime ...
```

Contoh baris pertama LA01:

```text
1 21 0 53 4 95 3 55 2 34
```

Interpretasi:

| Operasi | Mesin | Waktu Proses |
|---------|-------|-------------|
| O1      | M1    | 21          |
| O2      | M0    | 53          |
| O3      | M4    | 95          |
| O4      | M3    | 55          |
| O5      | M2    | 34          |

Setiap baris merepresentasikan satu *job*. Setiap pasangan angka menunjukkan Machine ID dan Processing Time sesuai urutan operasi yang wajib dikerjakan.

---

## Struktur Program

```
.
├── datasets/
│   ├── la01.txt
│   ├── la06.txt
│   └── la16.txt
├── include/
│   ├── common.hpp       # struct ScheduledOperation, library includes
│   ├── dataset.hpp      # struct Operation, Job, Dataset
│   ├── greedy.hpp       # class GreedyScheduler
│   ├── bnb.hpp          # struct State, Node, BnBResult, class BranchAndBoundScheduler
│   └── visualizer.hpp   # class GanttVisualizer
├── src/
│   ├── dataset.cpp
│   ├── greedy.cpp
│   ├── bnb.cpp
│   └── visualizer.cpp
└── main.cpp
```

---

## Metodologi Eksperimen

Untuk setiap dataset:

1. Load dataset dari file benchmark Lawrence.
2. Jalankan algoritma **Greedy MWR**, catat makespan dan runtime.
3. Gunakan makespan dan jadwal greedy sebagai *initial upper bound* untuk Branch and Bound.
4. Jalankan algoritma **Branch and Bound**, catat makespan, runtime, jumlah node dieksplorasi, jumlah node dipruning, dan status optimalitas.
5. Hitung *optimality gap* terhadap nilai optimum benchmark.

*Gap* dihitung menggunakan:

```text
Gap(%) = ((Makespan - Optimum) / Optimum) × 100
```

---

## Metrik Evaluasi

### 1. Makespan

Waktu penyelesaian seluruh job — semakin kecil semakin baik:

```text
Cmax = max completion time across all jobs
```

### 2. Runtime

Waktu komputasi algoritma dalam milidetik (ms).

### 3. Nodes Explored / Pruned

Khusus Branch and Bound — jumlah node yang dieksplorasi dan dipangkas selama pencarian. Rasio pruning yang tinggi menunjukkan *lower bound* yang efektif.

### 4. Optimality Gap

Selisih makespan hasil algoritma terhadap nilai optimum benchmark:

```text
Gap(%) = ((Makespan - Optimum) / Optimum) × 100
```

Gap = 0% berarti solusi optimal tercapai.

---

## Hasil yang Diharapkan

Algoritma **Greedy MWR** diharapkan menghasilkan solusi dengan runtime mendekati nol namun dengan *optimality gap* yang cukup signifikan, karena keputusan greedy bersifat lokal dan tidak mempertimbangkan dampak global terhadap makespan.

Algoritma **Branch and Bound** diharapkan menghasilkan solusi yang lebih baik atau mendekati optimal melalui eksplorasi ruang solusi yang sistematis, dengan trade-off berupa waktu komputasi yang jauh lebih besar. Penggunaan solusi greedy sebagai *initial upper bound* dan strategi *conflict-based branching* diharapkan menekan jumlah node yang perlu dieksplorasi secara signifikan.

Penelitian ini bertujuan menunjukkan trade-off antara kualitas solusi dan biaya komputasi dalam penyelesaian JSSP menggunakan dua pendekatan algoritmik dengan karakteristik dan kompleksitas yang berbeda.