#include <bits/stdc++.h>
using namespace std;

// Function to generate a vector with increasing order and only a few repeated numbers
vector<int> generateIncreasing(int size) {
    vector<int> result(size); // 
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<int> distribution(0, size * 0.9);

    for (int i = 0; i < size; i++) {
        result[i] = distribution(g);
    }
    sort(result.begin(), result.end());
    return result;
}

// Function to generate a vector with decreasing order and only a few repeated numbers
vector<int> generateDecreasing(int size) {
    vector<int> result(size);
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<int> distribution(0, size * 0.9);
    for (int i = 0; i < size; i++) {
        result[i] = distribution(g);
    }
    sort(result.begin(), result.end());
    reverse(result.begin(), result.end());
    return result;
}

// Function to generate a vector with random order and only a few repeated numbers
vector<int> generateRandom(int size) {
    vector<int> result(size);
    for (int i = 0; i < size; i++) {
        result[i] = i;
    }
    random_device rd;
    mt19937 g(rd());
    shuffle(result.begin(), result.end(), g);
    return result;
}

// Function to generate a vector with random order and 90% repeated numbers
vector<int> generateRandomWithRepeats(int size) {
    vector<int> result(size);
    random_device rd;
    mt19937 g(rd());
    uniform_int_distribution<int> distribution(0, size * 0.1);
    for (int i = 0; i < size; i++) {
        result[i] = distribution(g);
    }
    return result;
}

void printVector(vector<int> v){
    for(int i = 0; i < (int)v.size(); i++){
        cout << v[i] << " ";
    }
    cout << endl;
}
