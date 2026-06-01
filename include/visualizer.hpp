#ifndef VISUALIZER_HPP
#define VISUALIZER_HPP

#include "common.hpp"

class GanttVisualizer {
public:
    void print(const vector<ScheduledOperation>& schedule, int numMachines);
};

#endif