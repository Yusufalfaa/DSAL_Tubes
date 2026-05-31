#include "../include/greedy.hpp"

int GreedyScheduler::calculateRemainingWork(
    const Dataset& data,
    int jobId,
    int nextOp
) {
    int remainingWork = 0;

    const Job& job = data.jobs[jobId];

    for (int i = nextOp; i < job.operations.size(); i++) {
        remainingWork += job.operations[i].processingTime;
    }

    return remainingWork;
}

bool GreedyScheduler::allJobsFinished(
    const Dataset& data,
    const vector<int>& nextOperation
) {
    for (int jobId = 0; jobId < data.numJobs; jobId++) {
        if (nextOperation[jobId] < data.jobs[jobId].operations.size()) {
            return false;
        }
    }

    return true;
}

const vector<ScheduledOperation>& GreedyScheduler::getSchedule() const {
    return schedule;
}

int GreedyScheduler::solve(const Dataset& data) {
    schedule.clear();

    vector<int> nextOperation(data.numJobs, 0);

    vector<int> jobReadyTime(data.numJobs, 0);

    vector<int> machineReadyTime(data.numMachines, 0);

    while (!allJobsFinished(data, nextOperation)) {

        int selectedJob = -1;
        int bestRemainingWork = -1;

        // TODO:
        // Hitung MWR seluruh job
        // Pilih job dengan remaining work terbesar

        // TODO:
        // Ambil operasi berikutnya

        // TODO:
        // Hitung startTime dan finishTime

        // TODO:
        // Update state
    }

    // TODO:
    // Hitung makespan

    return 0;
}