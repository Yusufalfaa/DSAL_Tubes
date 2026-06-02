#include "../include/visualizer.hpp"

// =====================
// Fungsi: print
// Deskripsi: Menampilkan Gantt Chart compact berbasis interval (start-end) sekaligus menghitung idle time per machine
// Input: schedule hasil greedy/bnb, jumlah machine
// Output: visualisasi Gantt chart + total idle time per machine
// =====================
void GanttVisualizer::print(
    const vector<ScheduledOperation>& schedule,
    int numMachines
) {
    cout << "\n=== COMPACT EVENT-BASED GANTT ===\n\n";

    // idle time per machine storage
    vector<int> idleTime(numMachines, 0);

    for (int m = 0; m < numMachines; m++) {

        cout << "M" << m << ": ";

        vector<ScheduledOperation> ops;

        for (const auto& op : schedule) {
            if (op.machine == m) {
                ops.push_back(op);
            }
        }

        sort(ops.begin(), ops.end(),
             [](const ScheduledOperation& a, const ScheduledOperation& b) {
                 return a.startTime < b.startTime;
             });

        int current = 0;
        
        // ===== PHASE 1: Output operation per Machine =====
        for (const auto& op : ops) {
            cout << " | ";
            // ===== PHASE 2: idle calculation =====
            if (op.startTime > current) {
                idleTime[m] += (op.startTime - current);
                cout << "[idle " << current << "-" << op.startTime << "]";
                cout << " | ";
            }

            // print operation
            cout << "J" << op.jobId
                 << "(" << op.startTime
                 << "-" << op.endTime << ")";

            current = op.endTime;
        }

        cout << "\n";
    }

    // ===== PHASE 2: Output Idle Time per Machine =====
    cout << "\n=== MACHINE IDLE TIME ===\n";

    for (int m = 0; m < numMachines; m++) {
        cout << "M" << m << " idle: " << idleTime[m] << "\n";
    }
}