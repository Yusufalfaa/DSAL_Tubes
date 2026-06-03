#include "../include/bnb.hpp"

// =====================
// Fungsi: computeLowerBound
// Deskripsi: Menghitung batas bawah (lower bound) makespan dari suatu state
//            menggunakan kombinasi tiga komponen:
//            (1) makespan saat ini sebagai baseline,
//            (2) job bound — untuk tiap job, jumlahkan sisa processing time dari operasi
//                yang belum dijadwalkan ditambah waktu job itu sudah siap,
//            (3) machine load bound — untuk tiap mesin, jumlahkan sisa beban kerja
//                dari semua operasi yang belum dijadwalkan di mesin tersebut,
//                lalu tambahkan dengan waktu mesin itu sudah siap.
//            Nilai lower bound adalah maksimum dari ketiga komponen tersebut.
// Input:  State saat ini, Dataset JSSP
// Output: Nilai lower bound makespan minimum yang mungkin dicapai dari state tersebut
// =====================
int BranchAndBoundScheduler::computeLowerBound(
    const State&   state,
    const Dataset& data
) {
    int lb = state.currentMakespan;

    int nJobs     = data.numJobs;
    int nMachines = data.numMachines;

    // -------------------------
    // 1. JOB BOUND
    // -------------------------
    for (int j = 0; j < nJobs; j++) {
        int time = state.jobReadyTime[j];

        for (int op = state.jobProgress[j]; op < (int)data.jobs[j].operations.size(); op++) {
            time += data.jobs[j].operations[op].processingTime;
        }

        lb = max(lb, time);
    }

    // -------------------------
    // 2. MACHINE LOAD BOUND
    // -------------------------
    vector<int> machineLoad(nMachines, 0);

    for (int j = 0; j < nJobs; j++) {
        for (int op = state.jobProgress[j]; op < (int)data.jobs[j].operations.size(); op++) {
            int m = data.jobs[j].operations[op].machine;
            machineLoad[m] += data.jobs[j].operations[op].processingTime;
        }
    }

    for (int m = 0; m < nMachines; m++) {
        lb = max(lb, state.machineReadyTime[m] + machineLoad[m]);
    }

    return lb;
}

// =====================
// Fungsi: allJobsFinished
// Deskripsi: Mengecek apakah semua job sudah menyelesaikan seluruh operasinya
//            dengan membandingkan jobProgress tiap job terhadap jumlah operasi yang dimiliki.
// Input:  Dataset, vector jobProgress
// Output: true jika semua job selesai, false jika masih ada yang belum selesai
// =====================
bool allJobsFinished(const Dataset& data, const vector<int>& jobProgress) {
    for (int j = 0; j < data.numJobs; j++) {
        if (jobProgress[j] < (int)data.jobs[j].operations.size()) {
            return false;
        }
    }
    return true;
}

// =====================
// Fungsi: isComplete
// Deskripsi: Mengecek apakah state sudah merupakan solusi lengkap (semua job selesai).
//            Wrapper tipis di atas allJobsFinished agar konsisten dengan antarmuka class.
// Input:  State, Dataset
// Output: true jika semua job selesai diproses, false jika masih ada operasi tersisa
// =====================
bool BranchAndBoundScheduler::isComplete(
    const State&   state,
    const Dataset& data
) {
    return allJobsFinished(data, state.jobProgress);
}

// =====================
// Fungsi: generateChildren
// Deskripsi: Membentuk state anak menggunakan conflict-based branching — pendekatan
//            standar BnB untuk JSSP yang menjamin ruang pencarian lengkap dan sempit.
//
//            Cara kerja:
//            (1) Kumpulkan semua operasi yang ready (next operation tiap job).
//            (2) Cari "conflict machine" — mesin yang paling banyak diperebutkan,
//                diukur dari jumlah job ready yang membutuhkannya. Jika ada seri,
//                pilih mesin dengan beban kerja sisa terbesar sebagai tie-breaker
//                agar pruning lebih agresif di cabang yang paling berat.
//            (3) Untuk setiap job yang ready di conflict machine tersebut, buat
//                satu child: job itu dijadwalkan duluan di mesin itu, sementara
//                job-job lain yang juga butuh mesin itu harus menunggu sampai
//                job ini selesai (machineReadyTime di-update ke endTime job terpilih).
//
//            Dengan cara ini jumlah child per node = jumlah job yang bersaing di
//            conflict machine (biasanya 2–5), bukan semua job ready. Pohon pencarian
//            jauh lebih sempit sehingga pruning bekerja efektif.
//
// Input:  State saat ini, Dataset JSSP
// Output: vector state anak yang merepresentasikan semua kemungkinan urutan
//         job di conflict machine
// =====================
vector<State> BranchAndBoundScheduler::generateChildren(
    const State&   state,
    const Dataset& data
) {
    vector<State> children;

    int nJobs     = data.numJobs;
    int nMachines = data.numMachines;

    // -------------------------
    // Langkah 1: Kumpulkan semua operasi ready (next op tiap job yang belum selesai)
    // -------------------------
    struct ReadyOp {
        int jobId;
        int opIdx;
        int machine;
        int processingTime;
        int earliestStart;
    };

    vector<ReadyOp> readyOps;
    readyOps.reserve(nJobs);

    for (int j = 0; j < nJobs; j++) {
        int opIdx = state.jobProgress[j];
        if (opIdx < (int)data.jobs[j].operations.size()) {
            const Operation& op = data.jobs[j].operations[opIdx];
            int est = max(state.jobReadyTime[j], state.machineReadyTime[op.machine]);
            readyOps.push_back({j, opIdx, op.machine, op.processingTime, est});
        }
    }

    if (readyOps.empty()) return children;

    // -------------------------
    // Langkah 2: Hitung berapa job yang ready di tiap mesin (conflict count),
    //            dan hitung sisa beban tiap mesin untuk tie-breaker.
    // -------------------------
    vector<int> conflictCount(nMachines, 0);
    vector<int> machineRemainingLoad(nMachines, 0);

    for (const ReadyOp& rop : readyOps) {
        conflictCount[rop.machine]++;
    }

    // sisa beban mesin dari SEMUA operasi yang belum dijadwalkan
    for (int j = 0; j < nJobs; j++) {
        for (int op = state.jobProgress[j]; op < (int)data.jobs[j].operations.size(); op++) {
            int m = data.jobs[j].operations[op].machine;
            machineRemainingLoad[m] += data.jobs[j].operations[op].processingTime;
        }
    }

    // -------------------------
    // Langkah 3: Pilih conflict machine — mesin dengan paling banyak job ready,
    //            tie-break ke mesin dengan sisa beban terbesar.
    // -------------------------
    int conflictMachine = -1;
    int maxConflict     = 0;
    int maxLoad         = -1;

    for (int m = 0; m < nMachines; m++) {
        if (conflictCount[m] > maxConflict ||
           (conflictCount[m] == maxConflict && machineRemainingLoad[m] > maxLoad)) {
            maxConflict     = conflictCount[m];
            maxLoad         = machineRemainingLoad[m];
            conflictMachine = m;
        }
    }

    // Jika tidak ada mesin dengan konflik (semua mesin hanya dipakai 1 job),
    // jalankan semua operasi ready sekaligus sebagai satu child deterministik
    // karena tidak ada keputusan sekuensing yang perlu dibuat.
    if (maxConflict <= 1) {
        State child = state;

        for (const ReadyOp& rop : readyOps) {
            int startTime = rop.earliestStart;
            int endTime   = startTime + rop.processingTime;

            child.jobProgress[rop.jobId]++;
            child.jobReadyTime[rop.jobId]       = endTime;
            child.machineReadyTime[rop.machine]  = endTime;
            child.currentMakespan = max(child.currentMakespan, endTime);

            ScheduledOperation sched;
            sched.jobId          = rop.jobId;
            sched.operationIndex = rop.opIdx;
            sched.machine        = rop.machine;
            sched.startTime      = startTime;
            sched.endTime        = endTime;

            child.schedule.push_back(sched);
        }

        children.push_back(child);
        return children;
    }

    // -------------------------
    // Langkah 4: Branch — untuk setiap job yang ready di conflictMachine,
    //            buat satu child di mana job itu dijadwalkan duluan.
    //            Job lain yang juga butuh conflictMachine harus menunggu
    //            sampai job terpilih selesai di mesin tersebut.
    // -------------------------
    for (const ReadyOp& chosen : readyOps) {
        if (chosen.machine != conflictMachine) continue;

        State child = state;

        // Jadwalkan chosen job duluan di conflictMachine
        int startTime = chosen.earliestStart;
        int endTime   = startTime + chosen.processingTime;

        child.jobProgress[chosen.jobId]++;
        child.jobReadyTime[chosen.jobId]          = endTime;
        child.machineReadyTime[conflictMachine]    = endTime;
        child.currentMakespan = max(child.currentMakespan, endTime);

        ScheduledOperation sched;
        sched.jobId          = chosen.jobId;
        sched.operationIndex = chosen.opIdx;
        sched.machine        = conflictMachine;
        sched.startTime      = startTime;
        sched.endTime        = endTime;

        child.schedule.push_back(sched);

        // Jadwalkan semua operasi ready lainnya yang TIDAK berkonflik di conflictMachine
        // (mesinnya berbeda dari conflictMachine) — tidak ada keputusan sekuensing di sini
        for (const ReadyOp& other : readyOps) {
            if (other.jobId == chosen.jobId)      continue;
            if (other.machine == conflictMachine)  continue;

            int otherStart = max(child.jobReadyTime[other.jobId],
                                 child.machineReadyTime[other.machine]);
            int otherEnd   = otherStart + other.processingTime;

            child.jobProgress[other.jobId]++;
            child.jobReadyTime[other.jobId]      = otherEnd;
            child.machineReadyTime[other.machine] = otherEnd;
            child.currentMakespan = max(child.currentMakespan, otherEnd);

            ScheduledOperation otherSched;
            otherSched.jobId          = other.jobId;
            otherSched.operationIndex = other.opIdx;
            otherSched.machine        = other.machine;
            otherSched.startTime      = otherStart;
            otherSched.endTime        = otherEnd;

            child.schedule.push_back(otherSched);
        }

        children.push_back(child);
    }

    return children;
}

// =====================
// Fungsi: getSchedule
// Deskripsi: Mengembalikan jadwal terbaik yang ditemukan setelah proses solve selesai.
//            Digunakan untuk mengakses hasil dari luar class tanpa mengekspos state internal.
// Input:  -
// Output: Reference ke vector ScheduledOperation terbaik
// =====================
const vector<ScheduledOperation>& BranchAndBoundScheduler::getSchedule() const {
    return bestSchedule;
}

// =====================
// Fungsi: solve
// Deskripsi: Menyelesaikan JSSP menggunakan Best-First Branch and Bound
//            dengan initial upper bound dari luar (misalnya hasil greedy).
//
//            Alur utama:
//            (1) Inisialisasi root state dan masukkan ke priority queue.
//            (2) Tiap iterasi ambil node dengan lower bound terkecil (Best-First).
//            (3) Pruning: buang node jika lower bound-nya LEBIH BESAR dari bestMakespan
//                (bukan >=, agar solusi dengan makespan sama tidak ikut terpotong).
//            (4) Jika state lengkap dan makespannya lebih baik, update best.
//            (5) Jika belum lengkap, expand dengan generateChildren dan hitung
//                lower bound tiap child sebelum dimasukkan ke PQ.
//            (6) Eksplorasi dihentikan lebih awal jika node limit atau time limit tercapai;
//                hasil yang dikembalikan adalah best-so-far dengan flag isOptimal = false.
//
// Input:  Dataset, initial makespan greedy, initial schedule greedy
// Output: BnBResult berisi solusi terbaik dan statistik eksplorasi
// =====================
BnBResult BranchAndBoundScheduler::solve(
    const Dataset&                    data,
    int                               initialUB,
    const vector<ScheduledOperation>& initialSchedule
) {
    nodesExplored = 0;
    nodesPruned   = 0;

    bestMakespan = initialUB;
    bestSchedule = initialSchedule;

    PQ pq;

    // ROOT state
    State root(data.numJobs, data.numMachines);
    root.lowerBound = computeLowerBound(root, data);
    pq.push(Node(root, 0));

    // Catat waktu mulai untuk time limit
    auto startTime = chrono::steady_clock::now();
    bool hitLimit  = false;

    while (!pq.empty()) {

        // -------------------------
        // CEK BATAS EKSPLORASI
        // -------------------------
        if (nodesExplored >= NODE_LIMIT) {
            hitLimit = true;
            break;
        }

        auto now    = chrono::steady_clock::now();
        int elapsed = (int)chrono::duration_cast<chrono::seconds>(now - startTime).count();
        if (elapsed >= TIME_LIMIT_SECONDS) {
            hitLimit = true;
            break;
        }

        Node  currentNode = pq.top();
        pq.pop();

        State current = currentNode.state;
        nodesExplored++;

        // -------------------------
        // PRUNING
        // Gunakan > bukan >= agar state dengan lowerBound == bestMakespan
        // tetap dieksplorasi; bisa jadi solusi valid dengan makespan sama.
        // -------------------------
        if (current.lowerBound > bestMakespan) {
            nodesPruned++;
            continue;
        }

        // -------------------------
        // COMPLETE STATE
        // -------------------------
        if (isComplete(current, data)) {
            if (current.currentMakespan < bestMakespan) {
                bestMakespan = current.currentMakespan;
                bestSchedule = current.schedule;
            }
            continue;
        }

        // -------------------------
        // EXPAND
        // -------------------------
        vector<State> children = generateChildren(current, data);

        for (State& child : children) {
            child.lowerBound = computeLowerBound(child, data);

            // Pruning awal child sebelum masuk PQ
            if (child.lowerBound > bestMakespan) {
                nodesPruned++;
                continue;
            }

            pq.push(Node(child, currentNode.depth + 1));
        }
    }

    BnBResult result;
    result.bestMakespan  = bestMakespan;
    result.bestSchedule  = bestSchedule;
    result.nodesExplored = nodesExplored;
    result.nodesPruned   = nodesPruned;
    result.isOptimal     = !hitLimit;

    return result;
}