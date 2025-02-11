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

int main(){
    srand(time(0));

    RedBlackTree RBT;

    RBT.insert(10);
    RBT.insert(20);
    RBT.insert(30);
    RBT.insert(40);
    RBT.insert(50);
    RBT.insert(25);

    printTree(RBT.root, "", false, true);

    return 0;
}

