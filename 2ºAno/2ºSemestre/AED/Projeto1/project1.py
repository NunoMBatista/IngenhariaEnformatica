import random
import time
import numpy as np

def first_solution(a, n, k):
    for i in range(n):
        for j in range(n):
            if i != j and a[i] + a[j] == k:
                return True
    return False

def second_solution(a, n, k):
    i, j = 0, n-1
    while i < j:
        if a[i] + a[j] < k:
            i+=1
        elif a[i] + a[j] > k:
            j-=1
        else:
            return True
    return False

def third_solution(a, n, k):
    dp = [0] * (k + 1)
    for i in range(n):
        if a[i] <= k:
            dp[a[i]] += 1
    for i in range(n):
        if a[i] <= k:
            if (dp[k - a[i]] > 0 and a[i] != k - a[i]) or (dp[k - a[i]] > 1 and a[i] == k - a[i]):
                return True
    return False

def generate_random_vector(len):
    return [random.randint(0, 99999) for _ in range(len)]

def solve(n, k, a):
    start = time.time()
    first_solution(a, n, k)
    end = time.time()
    duration = int((end - start) * 1e6)
    print("Time to run:", duration)

    v = a
    start = time.time()
    sorted(v)
    second_solution(v, n, k)
    end = time.time()
    duration = int((end - start) * 1e6)
    print("Time to run:", duration)

    start = time.time()
    third_solution(a, n, k)
    end = time.time()
    duration = int((end - start) * 1e6)
    print("Time to run:", duration)

if __name__ == "__main__":
    t = 10
    random.seed(time.time())
    lens = [20000, 40000, 60000, 80000, 100000]
    for l in lens:
        print (f"Testing arrays with {l} elements")
        for it in range(t):
            print(f"Case #{it + 1}: ")
            n = l
            k = random.randint(0, 200000)
            a = generate_random_vector(l)
            solve (n, k, a)
