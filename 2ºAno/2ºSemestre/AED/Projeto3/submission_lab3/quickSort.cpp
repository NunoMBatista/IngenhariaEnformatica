#pragma once
#include <bits/stdc++.h> 
 
using namespace std;

int partition(vector<int> &array, int low, int high){
    int pivot = array[high]; // The pivot is the last element
    int lowIdx = low - 1; // Index of the first element

    for(int cur = low; cur < high; cur++){
        /*
        If the current element is smaller than the pivot, the lowest index is incremented
        and the current element is swapped with the element at the lowest index
        */
        if(array[cur] < pivot){
            lowIdx++;
            swap(array[lowIdx], array[cur]); 
        }
    }
    swap(array[lowIdx + 1], array[high]); // Swap the pivot with the element at the lowest index
    
    // It is now guaranteed that every element smaller than the pivot is to it's left

    return lowIdx + 1; // Return the index of the pivot
}

void quickSort(vector<int> &array, int low, int high){
    if(low < high){
        int pivotIdx = partition(array, low, high); // Return the index of the pivot 
    
        quickSort(array, low, pivotIdx - 1); // Sort the elements to the left of the pivot
        quickSort(array, pivotIdx + 1, high); // Sort the elements to the right of the pivot
    }
}

