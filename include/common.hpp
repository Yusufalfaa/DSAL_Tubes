#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <queue>
#include <stack>
#include <algorithm>

#include <chrono>

using namespace std;

struct ScheduledOperation
{
    int jobId;
    int operationIndex;
    int machine;

    int startTime;
    int endTime;
};

#endif // COMMON_HPP