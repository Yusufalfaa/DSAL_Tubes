#include "../include/dataset.hpp"

bool Dataset::load(const string& filename) {
    ifstream file(filename);

    if (!file.is_open()) {
        return false;
    }

    file >> numJobs >> numMachines;

    jobs.clear();

    for (int i = 0; i < numJobs; i++) {
        Job job;

        for (int j = 0; j < numMachines; j++) {
            Operation op;

            file >> op.machine >> op.processingTime;

            job.operations.push_back(op);
        }

        jobs.push_back(job);
    }

    file.close();
    return true;
}

void Dataset::print() const {
    cout << "Jobs     : " << numJobs << endl;
    cout << "Machines : " << numMachines << endl;

    for (int i = 0; i < numJobs; i++) {
        cout << "Job " << i << ": ";

        for (const auto& op : jobs[i].operations) {
            cout << "("
                 << op.machine
                 << ","
                 << op.processingTime
                 << ") ";
        }

        cout << endl;
    }
}

int Dataset::getTotalProcessingTime(int jobId) const {
    int total = 0;

    for (const auto& op : jobs[jobId].operations)
    {
        total += op.processingTime;
    }

    return total;
}