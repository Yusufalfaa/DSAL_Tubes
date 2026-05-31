#include "include/common.hpp"
#include "include/dataset.hpp"
#include "include/greedy.hpp"
#include "include/branch_bound.hpp"


int main() {
    Dataset data;
    
    // Load Dataset
    if (!data.load("../datasets/la01.txt")) {
        cout << "Failed to load dataset\n";
        return 1;
    }

    data.print();

    // // Check MWR (Most Work Remaining) Calcuation
    // for(int i = 0; i < data.numJobs; i++) {
    //     cout << "Job "
    //         << i
    //         << " Total PT = "
    //         << data.getTotalProcessingTime(i)
    //         << endl;
    // }

    return 0;

}