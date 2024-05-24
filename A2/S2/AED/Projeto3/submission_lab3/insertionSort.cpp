#pragma once
#include <bits/stdc++.h> 
#include "global.cpp"

using namespace std;

void insertionSort(vector<int> &array){
    for(int i = 0; i < (int)array.size(); i++){
        int j = i;
        while(j > 0 && array[j] < array[j-1]){
            swap(array[j], array[j-1]);
            j--;
        }
    }
}