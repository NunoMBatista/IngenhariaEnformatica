#include <bits/stdc++.h> 
        
using namespace std;
        
typedef long long ll;

#define forn(i,e) for(ll i = 0; i < e; i++)
#define INF INT_MAX
#define all(x) (x).begin(), (x).end()

void printArray(vector<int> &arr, int n);


void insertionSort(vector<int> &arr, int n) {
    int i, key, j;
    for (i = 1; i < n; i++) {
        key = arr[i];
        j = i - 1;
        while (j >= 0 && arr[j] > key) {
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        arr[j + 1] = key;
    }
}

void heapify(vector<int> &arr, int n, int i) {
    int largest = i; // Initialize largest as root
    int left = 2*i + 1; // left = 2*i + 1
    int right = 2*i + 2; // right = 2*i + 2

    // If left child is larger than root
    if (left < n && arr[left] > arr[largest])
        largest = left;

    // If right child is larger than largest so far
    if (right < n && arr[right] > arr[largest])
        largest = right;

    // If largest is not root
    if (largest != i) {
        swap(arr[i], arr[largest]);

        // Recursively heapify the affected sub-tree
        heapify(arr, n, largest);
    }
}

void heapSort(vector<int>& arr, int n) {
    // Build heap (rearrange array)
    for (int i = n / 2 - 1; i >= 0; i--)
        heapify(arr, n, i);

    // One by one extract an element from heap
    for (int i=n-1; i>=0; i--) {
        // Move current root to end
        swap(arr[0], arr[i]);

        // call max heapify on the reduced heap
        heapify(arr, i, 0);
    }
}

int median (vector<int> &arr, int l, int r){
    int m = (l + r) / 2;
    if (arr[l] > arr[m]) swap(arr[l], arr[m]);
    if (arr[l] > arr[r]) swap(arr[l], arr[r]);
    if (arr[m] > arr[r]) swap(arr[m], arr[r]);
    return m;
}

int partition (vector<int> &arr, int l, int r, int pivot){
    swap(arr[pivot], arr[r]);
    int less = l - 1;
    for (int i = l; i < r; i++){
        if (arr[i] < arr[r]){
            less++;
            swap(arr[i], arr[less]);
        }
    }
    swap(arr[less + 1], arr[r]);
    return less + 1;
}

void quickSort (vector<int> &arr, int l, int r){
    if (l < r){
        int pivot = median(arr, l, r);
        int x = partition (arr, l, r, pivot);
        quickSort(arr, l, x - 1);
        quickSort(arr, x + 1, r);
    }
}


void printArray(vector<int> &arr, int n) {
    int i;
    for (i = 0; i < n; i++)
        cout << arr[i] << " ";
    cout << endl;
}

int main(){
    vector <int> original = {1, 7, 9, 2, 4, 3, 0, 10};
    vector <int> ar = original;
    //Tarefa A -> Implementar um insertion sort
    insertionSort(ar, (int)ar.size());
    printArray(ar, (int)ar.size());
    printArray(original, (int)original.size());
    ar = original;
    printArray(ar, (int)ar.size());
    heapSort(ar, (int)ar.size());
    printArray(ar, (int)ar.size()); 

    ar = original;
    printArray(ar, (int)ar.size());
    quickSort(ar, 0, (int)ar.size() - 1);
    printArray(ar, (int)ar.size());

    return 0;
}

