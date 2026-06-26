#include "../include/bnb.hpp"

// =====================
// Fungsi: computeLowerBound
// Menghitung lower bound suatu state.
//
// Time Complexity: O(J × O + M)
// J = jumlah job
// O = jumlah operasi per job
// M = jumlah machine
// =====================
int BranchAndBoundScheduler::computeLowerBound(
    const State&   state,
    const Dataset& data
) {
    int lb = state.currentMakespan;

    int nJobs     = data.numJobs;
    int nMachines = data.numMachines;

    // Job bound
    // O(J × O)
    for (int j = 0; j < nJobs; j++) {
        int time = state.jobReadyTime[j];

        // Loop sisa operasi
        // O(O)
        for (int op = state.jobProgress[j]; op < (int)data.jobs[j].operations.size(); op++) {
            time += data.jobs[j].operations[op].processingTime;
        }

        lb = max(lb, time);
    }

    // Machine load
    vector<int> machineLoad(nMachines, 0);

    // Loop seluruh job
    // O(J × O)
    for (int j = 0; j < nJobs; j++) {

        // Loop sisa operasi
        // O(O)
        for (int op = state.jobProgress[j]; op < (int)data.jobs[j].operations.size(); op++) {
            int m = data.jobs[j].operations[op].machine;
            machineLoad[m] += data.jobs[j].operations[op].processingTime;
        }
    }

    // Loop seluruh machine
    // O(M)
    for (int m = 0; m < nMachines; m++) {
        lb = max(lb, state.machineReadyTime[m] + machineLoad[m]);
    }

    return lb;
}

// =====================
// Fungsi: allJobsFinished
// Mengecek apakah semua job telah selesai.
//
// Time Complexity: O(J)
// J = jumlah job
// =====================
bool allJobsFinished(const Dataset& data, const vector<int>& jobProgress) {

    // Loop seluruh job
    // O(J)
    for (int j = 0; j < data.numJobs; j++) {
        if (jobProgress[j] < (int)data.jobs[j].operations.size()) {
            return false;
        }
    }

    return true;
}

// =====================
// Fungsi: isComplete
// Mengecek apakah semua job telah selesai.
//
// Time Complexity: O(J)
// J = jumlah job
// =====================
bool BranchAndBoundScheduler::isComplete(
    const State&   state,
    const Dataset& data
) {
    return allJobsFinished(data, state.jobProgress);
}

// =====================
// Fungsi: generateChildren
// Deskripsi: Membentuk state anak menggunakan conflict-based branching.
//
// Langkah:
// (1) Kumpulkan seluruh operasi yang ready.
// (2) Hitung conflict tiap machine dan remaining load.
// (3) Pilih conflict machine.
// (4) Generate child dari setiap kandidat pada conflict machine.
//
// Time Complexity: O(J × O + J² + M)
// J = jumlah job
// O = jumlah operasi per job
// M = jumlah machine
// =====================
vector<State> BranchAndBoundScheduler::generateChildren(
    const State&   state,
    const Dataset& data
) {
    vector<State> children;

    int nJobs     = data.numJobs;
    int nMachines = data.numMachines;

    // -------------------------
    // Langkah 1: Kumpulkan operasi ready
    // Time Complexity: O(J)
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

    // Loop seluruh job
    // O(J)
    for (int j = 0; j < nJobs; j++) {
        int opIdx = state.jobProgress[j];

        if (opIdx < (int)data.jobs[j].operations.size()) {
            const Operation& op = data.jobs[j].operations[opIdx];

            int est = max(state.jobReadyTime[j],
                          state.machineReadyTime[op.machine]);

            readyOps.push_back({
                j,
                opIdx,
                op.machine,
                op.processingTime,
                est
            });
        }
    }

    if (readyOps.empty())
        return children;

    // -------------------------
    // Langkah 2: Hitung conflict dan remaining load
    // Time Complexity: O(J × O)
    // -------------------------
    vector<int> conflictCount(nMachines, 0);
    vector<int> machineRemainingLoad(nMachines, 0);

    // Hitung conflict tiap machine
    // O(J)
    for (const ReadyOp& rop : readyOps) {
        conflictCount[rop.machine]++;
    }

    // Hitung remaining load
    // O(J × O)
    for (int j = 0; j < nJobs; j++) {

        // Loop sisa operasi
        // O(O)
        for (int op = state.jobProgress[j];
             op < (int)data.jobs[j].operations.size();
             op++) {

            int m = data.jobs[j].operations[op].machine;
            machineRemainingLoad[m] +=
                data.jobs[j].operations[op].processingTime;
        }
    }

    // -------------------------
    // Langkah 3: Pilih conflict machine
    // Time Complexity: O(M)
    // -------------------------
    int conflictMachine = -1;
    int maxConflict = 0;
    int maxLoad = -1;

    // Loop seluruh machine
    // O(M)
    for (int m = 0; m < nMachines; m++) {

        if (conflictCount[m] > maxConflict ||
           (conflictCount[m] == maxConflict &&
            machineRemainingLoad[m] > maxLoad)) {

            maxConflict = conflictCount[m];
            maxLoad = machineRemainingLoad[m];
            conflictMachine = m;
        }
    }

    // Tidak ada konflik
    if (maxConflict <= 1) {

        State child = state;

        // Loop seluruh ready operation
        // O(J)
        for (const ReadyOp& rop : readyOps) {

            int startTime = rop.earliestStart;
            int endTime = startTime + rop.processingTime;

            child.jobProgress[rop.jobId]++;
            child.jobReadyTime[rop.jobId] = endTime;
            child.machineReadyTime[rop.machine] = endTime;
            child.currentMakespan =
                max(child.currentMakespan, endTime);

            ScheduledOperation sched;
            sched.jobId = rop.jobId;
            sched.operationIndex = rop.opIdx;
            sched.machine = rop.machine;
            sched.startTime = startTime;
            sched.endTime = endTime;

            child.schedule.push_back(sched);
        }

        children.push_back(child);
        return children;
    }

    // -------------------------
    // Langkah 4: Generate child
    // Time Complexity: O(J²)
    // -------------------------

    // Loop kandidat pada conflict machine
    // O(J²)
    for (const ReadyOp& chosen : readyOps) {

        if (chosen.machine != conflictMachine)
            continue;

        State child = state;

        int startTime = chosen.earliestStart;
        int endTime = startTime + chosen.processingTime;

        child.jobProgress[chosen.jobId]++;
        child.jobReadyTime[chosen.jobId] = endTime;
        child.machineReadyTime[conflictMachine] = endTime;
        child.currentMakespan =
            max(child.currentMakespan, endTime);

        ScheduledOperation sched;
        sched.jobId = chosen.jobId;
        sched.operationIndex = chosen.opIdx;
        sched.machine = conflictMachine;
        sched.startTime = startTime;
        sched.endTime = endTime;

        child.schedule.push_back(sched);

        // Jadwalkan operasi ready lain
        // O(J)
        for (const ReadyOp& other : readyOps) {

            if (other.jobId == chosen.jobId)
                continue;

            if (other.machine == conflictMachine)
                continue;

            int otherStart =
                max(child.jobReadyTime[other.jobId],
                    child.machineReadyTime[other.machine]);

            int otherEnd =
                otherStart + other.processingTime;

            child.jobProgress[other.jobId]++;
            child.jobReadyTime[other.jobId] = otherEnd;
            child.machineReadyTime[other.machine] = otherEnd;
            child.currentMakespan =
                max(child.currentMakespan, otherEnd);

            ScheduledOperation otherSched;
            otherSched.jobId = other.jobId;
            otherSched.operationIndex = other.opIdx;
            otherSched.machine = other.machine;
            otherSched.startTime = otherStart;
            otherSched.endTime = otherEnd;

            child.schedule.push_back(otherSched);
        }

        children.push_back(child);
    }

    return children;
}

// =====================
// Fungsi: getSchedule
// Mengembalikan jadwal terbaik.
//
// Time Complexity: O(1)
// =====================
const vector<ScheduledOperation>& BranchAndBoundScheduler::getSchedule() const {
    return bestSchedule;
}

// =====================
// Fungsi: solve
// Deskripsi: Menyelesaikan JSSP menggunakan Best-First Branch and Bound.
//
// Langkah:
// (1) Inisialisasi root state.
// (2) Ambil node dengan lower bound terkecil.
// (3) Lakukan pruning.
// (4) Update solusi jika state lengkap.
// (5) Generate child dan hitung lower bound.
// (6) Berhenti jika mencapai node/time limit.
//
// Time Complexity: O(N × J × (J × O + M))
//
// N = jumlah node yang dieksplorasi
// J = jumlah job
// O = jumlah operasi per job
// M = jumlah machine
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

    // Inisialisasi root
    // O(J × O + M)
    State root(data.numJobs, data.numMachines);
    root.lowerBound = computeLowerBound(root, data);
    pq.push(Node(root, 0));

    auto startTime = chrono::steady_clock::now();
    bool hitLimit  = false;

    // Eksplorasi seluruh node
    // Total: O(N × J × (J × O + M))
    while (!pq.empty()) {

        // -------------------------
        // Cek batas eksplorasi
        // O(1)
        // -------------------------
        if (nodesExplored >= NODE_LIMIT) {
            hitLimit = true;
            break;
        }

        auto now = chrono::steady_clock::now();
        int elapsed = (int)chrono::duration_cast<chrono::seconds>(now - startTime).count();

        if (elapsed >= TIME_LIMIT_SECONDS) {
            hitLimit = true;
            break;
        }

        Node currentNode = pq.top();
        pq.pop();

        State current = currentNode.state;
        nodesExplored++;

        // -------------------------
        // Pruning
        // O(1)
        // -------------------------
        if (current.lowerBound > bestMakespan) {
            nodesPruned++;
            continue;
        }

        // -------------------------
        // Complete state
        // O(J)
        // -------------------------
        if (isComplete(current, data)) {
            if (current.currentMakespan < bestMakespan) {
                bestMakespan = current.currentMakespan;
                bestSchedule = current.schedule;
            }
            continue;
        }

        // -------------------------
        // Generate child
        // O(J × O + J² + M)
        // -------------------------
        vector<State> children = generateChildren(current, data);

        // Evaluasi seluruh child
        // O(J × (J × O + M))
        for (State& child : children) {

            // Hitung lower bound
            // O(J × O + M)
            child.lowerBound = computeLowerBound(child, data);

            // Pruning awal
            // O(1)
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