#include "../include/greedy.hpp"

// =====================
// Fungsi: calculateRemainingWork
// Deskripsi: Menghitung total sisa processing time dari sebuah job mulai dari operasi tertentu
// Input: Dataset, jobId, nextOp index
// Output: total remaining processing time (int)
// =====================
int GreedyScheduler::calculateRemainingWork(
    const Dataset& data,
    int jobId,
    int nextOp
) {
    int remainingWork = 0;

    const Job& job = data.jobs[jobId];

    for (size_t i = nextOp; i < job.operations.size(); i++) {
        remainingWork += job.operations[i].processingTime;
    }

    return remainingWork;
}

// =====================
// Fungsi: allJobsFinished
// Deskripsi: Mengecek apakah semua job sudah menyelesaikan seluruh operasinya
// Input: Dataset, vector nextOperation
// Output: true jika semua job selesai, false jika masih ada yang belum selesai
// =====================
bool GreedyScheduler::allJobsFinished(
    const Dataset& data,
    const vector<int>& nextOperation
) {
    for (int jobId = 0; jobId < data.numJobs; jobId++) {
        if ((size_t) nextOperation[jobId] < data.jobs[jobId].operations.size()) {
            return false;
        }
    }

    return true;
}

// =====================
// Fungsi: getSchedule
// Deskripsi: Mengembalikan hasil jadwal operasi yang sudah disusun oleh greedy
// Input: tidak ada
// Output: vector berisi seluruh ScheduledOperation
// =====================
const vector<ScheduledOperation>& GreedyScheduler::getSchedule() const {
    return schedule;
}

// =====================
// Fungsi: solve
// Deskripsi: Menjalankan algoritma Greedy MWR untuk menyusun jadwal JSSP
// Input: Dataset berisi job dan machine
// Output: makespan (total waktu penyelesaian seluruh job)
// =====================
int GreedyScheduler::solve(const Dataset& data) {

    schedule.clear();

    vector<int> nextOperation(data.numJobs, 0);
    vector<int> jobReadyTime(data.numJobs, 0);
    vector<int> machineReadyTime(data.numMachines, 0);

    while (!allJobsFinished(data, nextOperation)) {

        int selectedJob = -1;
        int bestRemainingWork = -1;

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

        jobReadyTime[selectedJob] = endTime;
        machineReadyTime[op.machine] = endTime;
        nextOperation[selectedJob]++;
    }

    int makespan = 0;
    for (const auto& s : schedule) {
        if (s.endTime > makespan)
            makespan = s.endTime;
    }

    return makespan;
}