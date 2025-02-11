#include <bits/stdc++.h>
using namespace std;

bool solucao1(vector<int> array, int k) {
    for (int i = 0; i < array.size(); ++i) {
        for (int j = 0; j < array.size(); ++j) {
            if (array[i] != array[j] && array[i] + array[j] == k) {
                return true;
            }
        }
    }
    return false;
}
bool solucao2(vector<int> array_ordenado, int k) {
    sort(array_ordenado.begin(), array_ordenado.end());
    int left = 0;
    int right = array_ordenado.size() - 1;

    while (left < right) {
        if (array_ordenado[left] + array_ordenado[right] < k) {
            left++;
        } else if (array_ordenado[left] + array_ordenado[right] > k) {
            right--;
        } else {
            if (array_ordenado[left] != array_ordenado[right]) {
                return true; //isto porque o k nao pode ser formado por 2 valores iguais(k/2+k/2)
            }
            return false;
        }
    }
    return false;
}

bool solucao3(vector<int> array, int k) {
    vector<bool> visto(k + 1, false);
    for (int i = 0; i < array.size(); ++i) {
        int complemento = k - array[i];
        if (array[i] * 2 == k) {
            continue; //isto porque o k nao pode ser formado por 2 valores iguais(k/2+k/2)
        }
        if (visto[complemento]) {
            return true;
        }
        visto[array[i]] = true;
    }
    return false;
}

int main() {
    int numm = 0;
    srand(time(0));
    int samples = 1;
    vector<int> inp = {20000, 40000, 60000, 80000, 100000};
    for (int sz : inp) {
        auto start = std::chrono::high_resolution_clock::now();
        auto end = std::chrono::high_resolution_clock::now();
        auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

        cout << "teste" << numm << endl;
        numm +=1;
        int tempo1 = 0;
        int tempo2 = 0;
        int tempo3 = 0;
        vector<int> array(sz);
        for (int i = 0; i < sz; ++i) {
            array[i] = rand() % sz;
        }
        int k = rand() % (sz*2);
        for (int i = 0; i < samples; ++i) {
            start = std::chrono::high_resolution_clock::now();
            solucao1(array, k);
            end = std::chrono::high_resolution_clock::now();
            timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            tempo1 += timeTaken.count();
        }
        for (int i = 0; i < samples; ++i) {
            start = std::chrono::high_resolution_clock::now();
            solucao2(array, k);
            end = std::chrono::high_resolution_clock::now();
            timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            tempo2 += timeTaken.count();
        }
        for (int i = 0; i < samples; ++i) {
            start = std::chrono::high_resolution_clock::now();
            solucao3(array, k);
            end = std::chrono::high_resolution_clock::now();
            timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            tempo3 += timeTaken.count();
        }
        cout << "Para " << sz << " o tempo médio da solução 1 é: " << tempo1 / samples << endl;
        cout << "Para " << sz << " o tempo médio da solução 2 é: " << tempo2 / samples << endl;
        cout << "Para " << sz << " o tempo médio da solução 3 é: " << tempo3 / samples << endl;

    }
    return 0;
}