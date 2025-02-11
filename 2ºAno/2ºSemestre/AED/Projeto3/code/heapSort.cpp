#pragma once
#include <bits/stdc++.h> 
#include "global.cpp"

using namespace std;

void heapify(vector<int>& array, int n, int i)
{
    int largest = i;
    int left = 2 * i + 1;
    int right = 2 * i + 2;

    // If left child is larger than root 
    if (left < n && array[left] > array[largest])
        largest = left;

    // If right child is larger than largest so far
    if (right < n && array[right] > array[largest]){
        largest = right;
    }

    // If largest is not root
    if (largest != i){
        swap(array[i], array[largest]); // Swap the largest with the root
        heapify(array, n, largest); // Recursively heapify the affected sub-tree
    }
}

void heapSort(vector<int>& array)
{
    int n = array.size();

    // Start by building max heap
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(array, n, i);

    // Put the element on top of the heap (the largest one) at the end of the array
    for (int i = n - 1; i > 0; i--){
        swap(array[0], array[i]);
        heapify(array, i, 0);
    }
}

