#pragma once
#include <bits/stdc++.h>
#include <random>

using namespace std; 

vector<int> increasingOrderInputs(int size){
    vector<int> inputs;
    for(int i = 1; i <= size; i++){
        inputs.push_back(i);
    }
    return inputs;
}

vector<int> decreasingOrderInputs(int size){
    vector<int> inputs = increasingOrderInputs(size);
    reverse(inputs.begin(), inputs.end());
    return inputs;
}

vector<int> randomOrderInputs(int size){
    vector<int> inputs = increasingOrderInputs(size);
    random_shuffle(inputs.begin(), inputs.end());
    return inputs;
}