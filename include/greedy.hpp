#ifndef GREEDY_HPP
#define GREEDY_HPP

#include "common.hpp"
#include "dataset.hpp"

// =====================
// Class: BranchAndBoundScheduler
// Deskripsi: Menyelesaikan Job Shop Scheduling Problem (JSSP) menggunakan
//            algoritma Greedy Priority Scheduling based on Most Work Remaining (MWR).
// =====================
class GreedyScheduler {
public:
    int solve(const Dataset& data);

    const vector<ScheduledOperation>& getSchedule() const;

private:
    int calculateRemainingWork(
        const Dataset& data,
        int jobId,
        int nextOp
    );

    bool allJobsFinished(
        const Dataset& data,
        const vector<int>& nextOperation
    );

    vector<ScheduledOperation> schedule;
};

#endif // GREEDY_HPP