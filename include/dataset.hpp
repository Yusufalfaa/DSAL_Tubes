#ifndef DATASET_HPP
#define DATASET_HPP

#include "common.hpp"

struct Operation {
    int machine;
    int processingTime;
};

struct Job {
    vector<Operation> operations;
};

class Dataset {
public:
    int numJobs;
    int numMachines;

    vector<Job> jobs;

    bool load(const string& filename);
    void print() const;
    int getTotalProcessingTime(int jobId) const;
};

#endif // DATASET_HPP