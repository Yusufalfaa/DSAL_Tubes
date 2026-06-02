#ifndef BNB_HPP
#define BNB_HPP

#include "common.hpp"
#include "dataset.hpp"

class BranchAndBoundScheduler {
private:

public:

    int solve(const Dataset& data);
    const vector<ScheduledOperation>& getSchedule() const;
};

#endif