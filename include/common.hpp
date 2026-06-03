#ifndef COMMON_HPP
#define COMMON_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <stack>

#include <queue>
#include <vector>
#include <climits>
#include <algorithm>

#include <iomanip>
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