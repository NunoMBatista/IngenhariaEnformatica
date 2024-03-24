#include <bits/stdc++.h>
#include <chrono>
#include "inputGenerator.cpp"
#include "BinaryTree.cpp"
#include "BinarySearchTree.cpp"
#include "AVLTree.cpp"
#include "RedBlackTree.cpp"
using namespace std;
using namespace chrono;

typedef long long ll;

#define MAX_SIZE 100000
#define INCREMENTS 10000
#define REPEATS MAX_SIZE/INCREMENTS

vector<vector<ll>> BTTimes(4, vector<ll>(REPEATS)); 
vector<vector<ll>> BSTTimes(4, vector<ll>(REPEATS));
vector<vector<ll>> AVLTimes(4, vector<ll>(REPEATS));
vector<vector<ll>> RBTimes(4, vector<ll>(REPEATS));
vector<vector<ll>> AVLrotations(4, vector<ll>(REPEATS));
vector<vector<ll>> RBTrotations(4, vector<ll>(REPEATS));

enum setType{
    INCREASING,
    DECREASING,
    RANDOM,
    RANDOM_WITH_REPEATS
};

void printTree(Node *node, string indent = "", bool isLeft = false, bool redBlack = false){
    if (node == nullptr){
        return;
    }

    cout << indent;
    if (isLeft){
        cout << "├── ";
        indent += "│   ";
    }
    else{
        cout << "└── ";
        indent += "    ";

    }

    cout << node->data;
    if(redBlack){
        cout << (node->color == RED ? " (R)" : " (B)");
    }
    cout << endl;

    printTree(node->right, indent, true, redBlack);
    printTree(node->left, indent, false, redBlack);
}

void fillTree(auto &tree, vector<int> v){
    for (int i = 0; i < (int)v.size(); i++){
        tree.insert(v[i]);
        // if ((i+1) % INCREMENTS == 0){
        //     cout << i+1 << " nodes inserted" << endl;
        // }
    }
}

void testSet(setType type, int size){
    rotationsAVL = 0;
    rotationsRB = 0;

    vector<int> vector;
    switch (type){
        case INCREASING:
            vector = generateIncreasing(size);
            break;
        case DECREASING:
            vector = generateDecreasing(size);
            break;
        case RANDOM:
            vector = generateRandom(size);
            break;
        case RANDOM_WITH_REPEATS:
            vector = generateRandomWithRepeats(size);
            break;
    }

    BinaryTree BT;
    BinarySearchTree BST;
    AVLTree AVL;
    RedBlackTree RBT;

    cout << "Filling Binary Tree" << endl;
    auto start = high_resolution_clock::now();
    fillTree(BT, vector);
    auto end = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    cout << "Time taken to fill Binary Search Tree: " << duration.count() << " microseconds\n" << endl;
    BTTimes[type][size/INCREMENTS-1] = duration.count();

    cout << "Filling Binary Search Tree" << endl;
    start = high_resolution_clock::now();
    fillTree(BST, vector);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Time taken to fill Binary Search Tree: " << duration.count() << " microseconds\n" << endl;
    BSTTimes[type][size/INCREMENTS-1] = duration.count();    
    
    cout << "Filling AVL Tree" << endl;
    start = high_resolution_clock::now();
    fillTree(AVL, vector);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Time taken to fill AVL Tree: " << duration.count() << " microseconds\n" << endl;
    AVLTimes[type][size/INCREMENTS-1] = duration.count();
    AVLrotations[type][size/INCREMENTS-1] = rotationsAVL;
    
    cout << "Filling Red-Black Tree" << endl;
    start = high_resolution_clock::now();
    fillTree(RBT, vector);
    end = high_resolution_clock::now();
    duration = duration_cast<microseconds>(end - start);
    cout << "Time taken to fill Red-Black Tree: " << duration.count() << " microseconds\n" << endl;
    RBTimes[type][size/INCREMENTS-1] = duration.count();
    RBTrotations[type][size/INCREMENTS-1] = rotationsRB;
    
    cout << "\n\n";
}

int main(){
    srand(time(0));

    for(int sz = INCREMENTS; sz <= MAX_SIZE; sz += INCREMENTS){
        cout << "\n----------------------\nNow filling trees with " << sz << " nodes" << endl;

        cout << "Testing with increasing order\n" << endl;
        testSet(INCREASING, sz);

        cout << "Testing with decreasing order\n" << endl;
        testSet(DECREASING, sz);

        cout << "Testing with random order\n" << endl;
        testSet(RANDOM, sz);

        cout << "Testing with random order and 90% repeated numbers\n" << endl;
        testSet(RANDOM_WITH_REPEATS, sz);

        cout << endl;
    }

    ofstream file("output.csv");

    file << "Binary Tree Times\n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << BTTimes[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file << "Binary Search Tree Times\n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << BSTTimes[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file << "AVL Tree Times\n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << AVLTimes[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file << "Red-Black Tree Times\n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << RBTimes[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file << "AVL Rotations\,n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << AVLrotations[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file << "Red-Black Rotations\n";
    for (int i = 0; i < 4; i++){
        for (int j = 0; j < REPEATS; j++){
            file << RBTrotations[i][j];
            if (j < REPEATS - 1) {
                file << ",";
            }
        }
        file << "\n";
    }

    file.close();



    return 0;
}

