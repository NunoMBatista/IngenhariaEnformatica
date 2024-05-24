#pragma once
#include <bits/stdc++.h>

typedef long long ll;
using namespace std;

void printArray(vector<int> &array){
    for(int i = 0; i < (int) array.size(); i++){
        cout << array[i] << " ";
    }
    cout << endl;
}

void swap(int &a, int &b){
    int temp = a;
    a = b;
    b = temp;
}
