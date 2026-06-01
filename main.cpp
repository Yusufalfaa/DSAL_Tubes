#include "include/common.hpp"
#include "include/dataset.hpp"
#include "include/greedy.hpp"
#include "include/branch_bound.hpp"
#include "include/visualizer.hpp"

int main() {

    Dataset data;

    // Load Dataset
    if (!data.load("../datasets/la01.txt")) {
        cout << "Failed to load dataset\n";
        return 1;
    }

    // Optional debug dataset
    data.print();

    // GREEDY SCHEDULING
    GreedyScheduler greedy;

    int makespan = greedy.solve(data);

    cout << "\n=== GREEDY RESULT ===\n";
    cout << "Makespan: " << makespan << endl;

    // GANTT CHART OUTPUT
    GanttVisualizer vis;

    vis.print(greedy.getSchedule(), data.numMachines);

    return 0;
}