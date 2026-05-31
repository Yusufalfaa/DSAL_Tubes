#ifndef BRANCH_BOUND_HPP
#define BRANCH_BOUND_HPP

#include "common.hpp"
#include "dataset.hpp"

class BranchAndBound {
public:
    int solve(const Dataset& data);
};

#endif // BRANCH_BOUND_HPP