#include "include/common.hpp"
#include "include/dataset.hpp"
#include "include/greedy.hpp"
#include "include/bnb.hpp"
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

    // =====================
    // GREEDY SCHEDULING
    // =====================
    GreedyScheduler greedy;
    auto GreedyStart = chrono::high_resolution_clock::now();
    int greedyMakespan = greedy.solve(data);
    auto GreedyEnd = chrono::high_resolution_clock::now();

    auto duration = chrono::duration_cast<chrono::microseconds>(GreedyEnd - GreedyStart);

    GanttVisualizer vis;

    vis.print(greedy.getSchedule(), data.numMachines);
    
    cout << "\n=== GREEDY RESULT ===\n";
    cout << "Makespan: " << greedyMakespan << endl;
    cout << "Runtime: " << duration.count() << " ms\n" << endl;

    return 0;
}