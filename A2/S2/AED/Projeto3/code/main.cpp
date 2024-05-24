#include <bits/stdc++.h> 
#include <chrono>
#include "global.cpp"
#include "inputGenerator.cpp"
#include "insertionSort.cpp"
#include "heapSort.cpp"
#include "quickSortImproved.cpp"

#define MAX 1e5
#define STEPS 10

typedef long long ll;
using namespace std;

enum sortType{
    INSERTION,
    HEAP,
    QUICK
};

enum inputType{
    INCREASING,
    DECREASING,
    RANDOM
};

vector<string> sortNames = {"INSERTION SORT", "HEAP SORT"};

vector<vector<int>> insertionSortTimes(3, vector<int>(STEPS, 0));
vector<vector<int>> heapSortTimes(3, vector<int>(STEPS, 0));
vector<vector<int>> quickSortTimes(3, vector<int>(STEPS, 0));
map<enum sortType, vector<vector<int>>> times;

void countTimes(enum sortType type, int iteration){
    vector<int> increasingOrder = increasingOrderInputs(iteration*(MAX/STEPS));
    vector<int> decreasingOrder = decreasingOrderInputs(iteration*(MAX/STEPS));
    vector<int> randomOrder = randomOrderInputs(iteration*(MAX/STEPS));

    cout << "   Sorting elements in increasing order" <<endl;
    auto start = chrono::high_resolution_clock::now();

    if(type == INSERTION)
        insertionSort(increasingOrder);
    if(type == HEAP)
        heapSort(increasingOrder);
    if(type == QUICK)
        quickSort(increasingOrder, 0, increasingOrder.size()-1);

    auto end = chrono::high_resolution_clock::now();
    auto duration = chrono::duration_cast<chrono::microseconds>(end - start);   
    times[type][INCREASING][iteration-1] = duration.count();
    cout << "       Time Taken: " << duration.count() << "μs" << endl;

    cout << "   Sorting elements in decreasing order" << endl; 
    start = chrono::high_resolution_clock::now();

    if(type == INSERTION)
        insertionSort(decreasingOrder);
    if(type == HEAP)
        heapSort(decreasingOrder);
    if(type == QUICK)
        quickSort(decreasingOrder, 0, decreasingOrder.size()-1);

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start);
    times[type][DECREASING][iteration-1] = duration.count();
    cout << "       Time Taken: " << duration.count() << "μs" << endl;

    cout << "   Sorting elements in random order" << endl;
    start = chrono::high_resolution_clock::now();

    if(type == INSERTION)
        insertionSort(randomOrder);
    if(type == HEAP)
        heapSort(randomOrder);
    if(type == QUICK)
        quickSort(randomOrder, 0, randomOrder.size()-1);

    end = chrono::high_resolution_clock::now();
    duration = chrono::duration_cast<chrono::microseconds>(end - start);
    times[type][RANDOM][iteration-1] = duration.count();
    cout << "       Time Taken: " << duration.count() << "μs" << endl;
}

void solve(int arraySize, int iteration){
    cout << "Array Size: " << arraySize << endl;

    cout << "\n------------------------------------\n";
    cout << "   Start counting [INSERTION SORT] times with " << iteration * (MAX/STEPS) << " elements" << endl;
    countTimes(INSERTION, iteration);
    cout << "\n------------------------------------\n";

    cout << "\n------------------------------------\n";
    cout << "   Start counting [HEAP SORT] times with " << iteration * (MAX/STEPS) << " elements" << endl;
    countTimes(HEAP, iteration);
    cout << "\n------------------------------------\n";

    cout << "\n------------------------------------\n";
    cout << "   Start counting [QUICKSORT] times with " << iteration * (MAX/STEPS) << " elements" << endl;
    countTimes(QUICK, iteration);
    cout << "\n------------------------------------\n\n";
}
int main()
{
    times[INSERTION] = insertionSortTimes;
    times[HEAP] = heapSortTimes;
    times[QUICK] = quickSortTimes;

    int currentSize = 0;
    for(int it = 1; it <= STEPS; it++) {
     //cout << "Case #" << it+1 << ": ";
        currentSize += MAX/STEPS;
        solve(currentSize, it);
    }

    cout << "\nInsertion Sort Times" << endl;
    cout << "Increasing Order: ";
    printArray(times[INSERTION][INCREASING]);
    cout << "Decreasing Order: ";
    printArray(times[INSERTION][DECREASING]);
    cout << "Random Order: ";
    printArray(times[INSERTION][RANDOM]);

    cout << "\nHeap Sort Times" << endl;
    cout << "Increasing Order: ";
    printArray(times[HEAP][INCREASING]);
    cout << "Decreasing Order: ";
    printArray(times[HEAP][DECREASING]);
    cout << "Random Order: ";
    printArray(times[HEAP][RANDOM]);

    cout << "\nQuick Sort Times" << endl;
    cout << "Increasing Order: ";
    printArray(times[QUICK][INCREASING]);
    cout << "Decreasing Order: ";
    printArray(times[QUICK][DECREASING]);
    cout << "Random Order: ";
    printArray(times[QUICK][RANDOM]);

    // Put times in a csv file
    ofstream myfile;
    myfile.open("insertionSortTimes.csv");
    for(int i = 0; i < STEPS; i++){
        myfile << times[INSERTION][INCREASING][i] << "," << times[INSERTION][DECREASING][i] << "," << times[INSERTION][RANDOM][i] << "\n";
    }
    myfile.close();

    myfile.open("heapSortTimes.csv");
    for(int i = 0; i < STEPS; i++){
        myfile << times[HEAP][INCREASING][i] << "," << times[HEAP][DECREASING][i] << "," << times[HEAP][RANDOM][i] << "\n";
    }
    myfile.close();

    myfile.open("quickSortTimes.csv");
    for(int i = 0; i < STEPS; i++){
        myfile << times[QUICK][INCREASING][i] << "," << times[QUICK][DECREASING][i] << "," << times[QUICK][RANDOM][i] << "\n";
    }
    myfile.close();




    return 0;
}