#include <bits/stdc++.h>

typedef long long ll;

/*
    Algorithm 1
    Time Complexity: O(N^2)
*/
bool bruteForceSolution(std::vector<ll> arr, int k){
    int arrLen = arr.size();

    for(int i = 0; i < arrLen; ++i){
        for(int j = i; j < arrLen; ++j){
            if(arr[j] == arr[i]) continue;
            if(arr[i] + arr[j] == k)
                return true;
        }
    }
    return false;
}

/*
    Algorithm 2
    Time Complexity: O(Nlog(N))
*/
bool twoPointersSolution(std::vector<ll> arr, int k){
    sort(arr.begin(), arr.end());
    int low = 0, high = arr.size()-1; 

    while(low < high){
        if(arr[low] + arr[high] < k){
            low++;
        }
        else if(arr[low] + arr[high] > k){
            high--;
        }
        else if(arr[low] == arr[high]){
            break;
        }
        else{
            return true;
        }
        
    }
    return false;
}

/*
    Algorithm 3
    Time Complexity: O(N)
*/
bool cacheSolution(std::vector<ll> arr, int k, int sizeLimit){
    int arrLen = arr.size();
    std::unordered_set<int> dp;

    for(int i = 0; i < arrLen; ++i){
        if((k - arr[i] >= sizeLimit) || (arr[i] * 2 == k))
            continue;

        //Check if k-arr[i] is already in the dp set
        if(dp.find(k-arr[i]) != dp.end())
            return true;

        dp.insert(arr[i]);
    }
    return false; 
}

int sumFirst, sumSecond, sumThird;
void solve(int sizeLimit, int opt){
    //Seed
    srand(time(0));

    //Generate random vector
    auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::default_random_engine dre(seed);
    std::uniform_int_distribution<int> di(0, sizeLimit);
    
    int kLimit = 2 * sizeLimit;
    std::vector<ll> arr(sizeLimit); 

    //Generate random numbers
    std::generate(arr.begin(), arr.end(), [&]{
        return di(dre);
    });

    // // for(int i = 0; i < sizeLimit; ++i){
    // //     arr[i] = std::rand() % sizeLimit;
    // // }     
    
    
    int k = std::rand() % kLimit;

    //If option 2 is selected, k is changed in order not to be possible for it to be the sum of a pair of numbers in the array 
    if(opt == 2){
        k = sizeLimit*2+1;
    }

    //Algorithm 1 execution
    auto start = std::chrono::high_resolution_clock::now();
    std::string out = bruteForceSolution(arr, k) ? "True" : "False";
    auto end = std::chrono::high_resolution_clock::now();
    auto timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int time1 = timeTaken.count();

    //Algorithm 2 execution
    start = std::chrono::high_resolution_clock::now(); 
    out = twoPointersSolution(arr, k) ? "True" : "False";
    end = std::chrono::high_resolution_clock::now();
    timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int time2 = timeTaken.count();

    //Algorithm 3 execution
    start = std::chrono::high_resolution_clock::now();
    out = cacheSolution(arr, k, sizeLimit) ? "True" : "False";
    end = std::chrono::high_resolution_clock::now();
    timeTaken = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    int time3 = timeTaken.count();

    //printf("Algorithm 1: %d\n", time1);
    //printf("Algoritm 2: %d\n", time2);
    //printf("Algorithm 3: %d\n\n", time3);
    sumFirst += time1;
    sumSecond += time2;
    sumThird += time3;
}
int main()
{
    int t = 10, sampleSize = 10, opt;

    std::cout << "Select test case:\n1 - Random Case\n2 - Worst Case Scenario\n(Invalid option defaults to Random Case)\n";
    std::cin >> opt;   
    std::vector<int> s1, s2, s3;

    for(int it=1;it<=t;it++) {
        std::cout << "-----------------\n\nCASE #" << it << " N = " << it*10000 << "\n\n";
        sumFirst = sumSecond = sumThird = 0;
        int tempOpt = opt;
        for(int i = 1; i <= sampleSize; ++i){
            //Force the last iteration to always be the worst case scenario
            if(i > sampleSize-1)
                tempOpt = 2;

            solve(it*10000, tempOpt);
        }
        printf("Algorithm 1 average time taken: %dμs\n"
                "Algorithm 2 average time taken: %dμs\n"
                "Algorithm 3 average time taken: %dμs\n\n", sumFirst/sampleSize, sumSecond/sampleSize, sumThird/sampleSize);
        s1.push_back(sumFirst/sampleSize);
        s2.push_back(sumSecond/sampleSize);
        s3.push_back(sumThird/sampleSize);
    }
    return 0;
}
