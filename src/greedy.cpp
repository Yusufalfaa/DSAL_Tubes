#include "../include/greedy.hpp"

// =====================
// Fungsi: calculateRemainingWork
// Menghitung total processing time operasi yang belum dikerjakan.
//
// Time Complexity: O(O)
// O = jumlah operasi per job
// =====================
int GreedyScheduler::calculateRemainingWork(
    const Dataset& data,
    int jobId,
    int nextOp
) {
    int remainingWork = 0;

    const Job& job = data.jobs[jobId];

    // Loop sisa operasi
    // O(O)
    for (size_t i = nextOp; i < job.operations.size(); i++) {
        remainingWork += job.operations[i].processingTime;
    }

    return remainingWork;
}

// =====================
// Fungsi: allJobsFinished
// Mengecek apakah semua job telah selesai.
//
// Time Complexity: O(J)
// J = jumlah job
// =====================
bool GreedyScheduler::allJobsFinished(
    const Dataset& data,
    const vector<int>& nextOperation
) {
    // Loop seluruh job
    // O(J)
    for (int jobId = 0; jobId < data.numJobs; jobId++) {
        if ((size_t) nextOperation[jobId] < data.jobs[jobId].operations.size()) {
            return false;
        }
    }

    return true;
}

// =====================
// Fungsi: getSchedule
// Mengembalikan hasil penjadwalan.
//
// Time Complexity: O(1)
// =====================
const vector<ScheduledOperation>& GreedyScheduler::getSchedule() const {
    return schedule;
}

// =====================
// Fungsi: solve
// Menyusun jadwal menggunakan Greedy MWR.
//
// Time Complexity: O(J × O × (J + O))
// J = jumlah job
// O = jumlah operasi per job
// =====================
int GreedyScheduler::solve(const Dataset& data) {

    schedule.clear();

    vector<int> nextOperation(data.numJobs, 0);
    vector<int> jobReadyTime(data.numJobs, 0);
    vector<int> machineReadyTime(data.numMachines, 0);

    // Loop seluruh operasi
    // O(J × O)
    while (!allJobsFinished(data, nextOperation)) {

        int selectedJob = -1;
        int bestRemainingWork = -1;

        // Loop seluruh job
        // O(J × O)
        // (O(J) × calculateRemainingWork O(O))
        for (int jobId = 0; jobId < data.numJobs; jobId++) {

            int nextOp = nextOperation[jobId];

            if ((size_t) nextOp >= data.jobs[jobId].operations.size())
                continue;

            int remaining = calculateRemainingWork(data, jobId, nextOp);

            if (selectedJob == -1 || remaining > bestRemainingWork) {
                bestRemainingWork = remaining;
                selectedJob = jobId;
            }
            else if (remaining == bestRemainingWork) {

                if (jobId < selectedJob) {
                    selectedJob = jobId;
                }
            }
        }

        int opIndex = nextOperation[selectedJob];
        const Operation& op = data.jobs[selectedJob].operations[opIndex];

        int startTime = max(jobReadyTime[selectedJob],
                            machineReadyTime[op.machine]);

        int endTime = startTime + op.processingTime;

        schedule.push_back({
            selectedJob,
            opIndex,
            op.machine,
            startTime,
            endTime
        });

        // Update waktu
        jobReadyTime[selectedJob] = endTime;
        machineReadyTime[op.machine] = endTime;
        nextOperation[selectedJob]++;
    }

    int makespan = 0;

    // Loop seluruh jadwal
    // O(J × O)
    for (const auto& s : schedule) {
        if (s.endTime > makespan)
            makespan = s.endTime;
    }

    return makespan;
}