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

    auto greedyDuration = chrono::duration_cast<chrono::milliseconds>(GreedyEnd - GreedyStart);

    GanttVisualizer vis;

    vis.print(greedy.getSchedule(), data.numMachines);

    cout << "\n=== GREEDY RESULT ===\n";
    cout << "Makespan: " << greedyMakespan << endl;
    cout << "Runtime : " << greedyDuration.count() << " ms\n" << endl;

    // =====================
    // BRANCH AND BOUND SCHEDULING
    // =====================
    BranchAndBoundScheduler bnb;
    auto BnBStart = chrono::high_resolution_clock::now();
    BnBResult bnbResult = bnb.solve(data, greedyMakespan, greedy.getSchedule());
    auto BnBEnd = chrono::high_resolution_clock::now();

    auto bnbDuration = chrono::duration_cast<chrono::milliseconds>(BnBEnd - BnBStart);

    vis.print(bnbResult.bestSchedule, data.numMachines);

    cout << "\n=== BRANCH AND BOUND RESULT ===\n";
    cout << "Makespan      : " << bnbResult.bestMakespan << endl;
    cout << "Nodes Explored: " << bnbResult.nodesExplored << endl;
    cout << "Nodes Pruned  : " << bnbResult.nodesPruned << endl;
    cout << "Status        : " << (bnbResult.isOptimal ? "Optimal" : "Best-so-far (limit reached)") << endl;
    cout << "Runtime       : " << bnbDuration.count() << " ms\n" << endl;

    return 0;
}