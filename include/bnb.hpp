#ifndef BNB_HPP
#define BNB_HPP

#include "common.hpp"
#include "dataset.hpp"

// =====================
// Struct: State
// Deskripsi: Merepresentasikan satu node dalam pohon pencarian BnB.
//            Menyimpan progres tiap job, waktu siap job dan mesin,
//            makespan saat ini, lower bound estimasi, dan jadwal yang sudah terbentuk.
// =====================
struct State {
    vector<int> jobProgress;
    vector<int> jobReadyTime;
    vector<int> machineReadyTime;

    int currentMakespan;
    int lowerBound;

    vector<ScheduledOperation> schedule;

    State() {}

    State(int nJobs, int nMachines) {
        jobProgress.assign(nJobs, 0);
        jobReadyTime.assign(nJobs, 0);
        machineReadyTime.assign(nMachines, 0);
        currentMakespan = 0;
        lowerBound = INT_MAX;
    }
};

// =====================
// Struct: Node
// Deskripsi: Pembungkus State untuk dimasukkan ke priority queue.
//            Menyimpan state dan kedalaman node dalam pohon pencarian.
// =====================
struct Node {
    State state;
    int depth;

    Node() {}

    Node(const State& s, int d = 0) {
        state = s;
        depth = d;
    }
};

// =====================
// Struct: CompareNode
// Deskripsi: Komparator untuk priority queue berbasis Best-First Search.
//            Node dengan lower bound lebih kecil diprioritaskan (min-heap atas lowerBound).
// =====================
struct CompareNode {
    bool operator()(const Node& a, const Node& b) const {
        return a.state.lowerBound > b.state.lowerBound;
    }
};

// PQ = priority queue Best-First: node dengan lowerBound terkecil keluar duluan
using PQ = priority_queue<Node, vector<Node>, CompareNode>;

// =====================
// Struct: BnBResult
// Deskripsi: Menyimpan hasil akhir dari proses Branch and Bound,
//            meliputi makespan terbaik, jadwal operasi terbaik,
//            jumlah node yang dieksplorasi dan dipruning,
//            serta flag apakah solusi yang ditemukan sudah optimal atau hanya best-so-far.
// =====================
struct BnBResult {
    int bestMakespan;
    vector<ScheduledOperation> bestSchedule;

    long long nodesExplored;
    long long nodesPruned;

    bool isOptimal;  // true jika BnB selesai penuh, false jika terpotong limit

    BnBResult() {
        bestMakespan  = INT_MAX;
        nodesExplored = 0;
        nodesPruned   = 0;
        isOptimal     = false;
    }
};

// =====================
// Class: BranchAndBoundScheduler
// Deskripsi: Menyelesaikan Job Shop Scheduling Problem (JSSP) menggunakan
//            algoritma Best-First Branch and Bound.
//            Upper bound awal disuplai dari luar (misalnya hasil greedy),
//            sehingga pruning lebih agresif sejak node pertama.
// =====================
class BranchAndBoundScheduler {
private:
    int bestMakespan;
    vector<ScheduledOperation> bestSchedule;

    long long nodesExplored;
    long long nodesPruned;

    // Batas eksplorasi untuk mencegah hang pada instance besar
    static const long long NODE_LIMIT = 2'000'000;
    static const int       TIME_LIMIT_SECONDS = 60;

    int computeLowerBound(const State& state, const Dataset& data);
    bool isComplete(const State& state, const Dataset& data);
    vector<State> generateChildren(const State& state, const Dataset& data);

public:
    BnBResult solve(
        const Dataset& data,
        int initialUB,
        const vector<ScheduledOperation>& initialSchedule
    );

    const vector<ScheduledOperation>& getSchedule() const;
};

#endif // BNB_HPP