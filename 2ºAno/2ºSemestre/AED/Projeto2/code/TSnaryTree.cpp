#include <iostream>
#include <queue>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <chrono>
#include <stack>

using namespace std;

enum Color {RED, BLACK};

class Node {
public:
    Node *left;
    Node *right;
    Node *parent;
    int value, height, color;
    Node (int v, Node* l, Node* r, Node *p = nullptr){
        value = v; left = l; right = r; height = 1; parent = p;
        color = RED;
    }
    int getValue () { return value ;}
    Node* getLeft () { return left;}
    Node* getRight () { return right;}
    void setValue (int v) {value = v;}
    void setLeft (Node* l) {left = l;}
    void setRight (Node* r) {right = r;}
};

class BTree {
public:
    Node * root;
    BTree(){root = nullptr;}

    Node* getRoot () {return root;}
    void setRoot (Node* r) {root = r;}
    bool isEmpty () {return root == nullptr;}
    int numberNodes(){return numberNodes(root);}
    int depth(){return depth(root);}
    bool exists(int value){return exists(root, value);}
    void printTree(){return printTree(root);}
protected:
    int numberNodes(Node * n){
        if (n == nullptr) return 0;
        return 1 + numberNodes(n->getLeft()) + numberNodes(n->getRight());
    }
    int depth(Node * n){
        if (n == nullptr) return -1;
        return 1 + max(depth(n->getLeft()), depth(n->getRight()));
    }
    bool exists(Node * cur, int val){
        if (cur == nullptr) return false;
        if (cur->getValue() == val) return true;
        return exists(cur->getLeft(), val) || exists(cur->getRight(), val); 
    }
    void printTree(Node* root, int indent = 0) {
        if(root != nullptr) {
            if(root->getRight()) {
                printTree(root->getRight(), indent+4);
            }
            if (indent) {
                cout << setw(indent) << ' ';
            }
            if (root->getRight()) cout<<" /\n" << setw(indent) << ' ';
            cout<< root->getValue() << "\n ";
            if(root->getLeft()) {
                cout << setw(indent) << ' ' <<" \\\n";
                printTree(root->getLeft(), indent+4);
            }
        }
    }
};

class NormalBT: public BTree{
public:
    void insertNode(Node * node){
        return insertNode(node, root);
    }
    bool findNode(int value){
        return exists(value);
    }
    void removeNode(int value){
        return removeNode(value);
    }
private:
    void removeNode(int value, Node * node){
        if (node == nullptr) return;
        if (node->getValue() == value){
            if (node->getLeft() == nullptr && node->getRight() == nullptr){
                delete node;
                return;
            }
            else if (node->getLeft() == nullptr){
                Node * temp = node;
                node = node->getRight();
                delete temp;
                return;
            }
            else if (node->getRight() == nullptr){
                Node * temp = node;
                node = node->getLeft();
                delete temp;
                return;
            }
            else{
                Node * temp = node->getRight();
                while (temp->getLeft() != nullptr){
                    temp = temp->getLeft();
                }
                node->setValue(temp->getValue());
                removeNode(temp->getValue(), node->getRight());
            }
        }
        removeNode(value, node->getLeft());
        removeNode(value, node->getRight());
    }
    void insertNode(Node * node,Node *& root){
        if (root == nullptr){
            root = node;
            return;
        }
        queue<Node *> queue;
        queue.push(root);
        if (findNode(node->getValue())) {
            return;
        }
        while (!queue.empty()){
            Node * cur = queue.front();
            queue.pop();
            if (cur->getLeft() == nullptr){
                cur->setLeft(node);
                return;
            }
            else{
                queue.push(cur->getLeft());
            }
            if (cur->getRight() == nullptr){
                cur->setRight(node);
                return;
            }
            else{
                queue.push(cur->getRight());
            }
        }
    }

};

class SearchBT: public BTree{
public:
    void insertNode(Node * node){
        return insertNode(node, root);
    }
    bool findNode(int value){
        return findNode(value, root);
    }
    void removeNode(int value){
        return removeNode(value);
    }
private:
    void insertNode(Node * node, Node *& current){
        if (current == nullptr) {
            current = node;
            return;
        }
        if (node->value < current->value){
            insertNode(node, current->left);
        }
        else if (node->value > current->value){
            insertNode(node, current->right);
        }
    }
    bool findNode(int value, Node * current){
        if (current == nullptr) return false;
        if (current->value == value) return true;
        if (value < current->value) return findNode(value, current->left);
        return findNode(value, current->right);
    }

};

class AVLTree: public SearchBT{
public:
    int rotation_count = 0;
    void insertNode(Node * node){
        return insertNode(node, root);
    }
    ~AVLTree() {
        deleteTree(root);
    }
private:


    void deleteTree(Node* root) {
        stack <Node*> nodeStack;
        nodeStack.push(root);

        while (!nodeStack.empty()) {
            Node* node = nodeStack.top();
            nodeStack.pop();

            if (node->left) {
                nodeStack.push(node->left);
            }
            if (node->right) {
                nodeStack.push(node->right);
            }

            delete node;
        }
    }
    void insertNode(Node * node, Node *& current){
        if (current == nullptr) {
            current = node;
            return;
        }
        if (node->value < current->value){
            insertNode(node, current->left);
        }
        else if (node->value > current->value){
            insertNode(node, current->right);
        }
        current->height = 1 + max(height(current->left), height(current->right));
        int balance = getBalance(current);
        // Left Left Case
        if (balance > 1 && node->value < current->left->value) {
            current = rightRotate(current);
        }
        // Right Right Case
        if (balance < -1 && node->value > current->right->value) {
            current = leftRotate(current);
        }
        // Left Right Case
        if (balance > 1 && node->value > current->left->value) {
            current->left = leftRotate(current->left);
            current = rightRotate(current);
        }
        // Right Left Case
        if (balance < -1 && node->value < current->right->value) {
            current->right = rightRotate(current->right);
            current = leftRotate(current);
        }
    }
    int getBalance(Node * node) {
        if (node == NULL)
            return 0;
        return height(node->left) - height(node->right);
    }
    int height(Node * node) {
        if (node == NULL)
            return 0;
        return node->height;
    }
    Node * rightRotate (Node * node){
        if (node == nullptr || node->left == nullptr) {
            cout << "wrong\n";
            return node;
        }

        rotation_count++;
        Node * temp_left = node->left;
        Node * temp_left_right = temp_left->right;

        temp_left->right = node;
        node->left = temp_left_right;

        node->height = max(height(node->left),
                        height(node->right)) + 1;
        temp_left->height = max(height(temp_left->left),
                        height(temp_left->right)) + 1;

        return temp_left;
    }
    Node *leftRotate(Node * node) {
        if (node == nullptr || node->right == nullptr) {
            cout << "wrong\n";
            return node;
        }

        rotation_count++;
        Node * temp_right = node->right;
        Node * temp_right_left = temp_right->left;

        temp_right->left = node;
        node->right = temp_right_left;

        node->height = max(height(node->left),   
                height(node->right)) + 1;
        temp_right->height = max(height(temp_right->left),
                        height(temp_right->right)) + 1;

        return temp_right;
    }
};

class RedBlackTree: public SearchBT {
public:
    int rotation_count = 0;
    void insertNode(Node * node){
        if (root == nullptr){
            root = node;
            root->color = BLACK;
        } else {
            insert(node);  
        }
    }
    void printTree2(Node* root, int indent = 0) {
        if(root != nullptr) {
            if(root->getRight()) {
                printTree2(root->getRight(), indent+4);
            }
            if (indent) {
                cout << setw(indent) << ' ';
            }
            if (root->getRight()) cout<<" /\n" << setw(indent) << ' ';
            
            cout<< root->getValue() << "(" << (root->color == RED ? "R" : "B") << ")\n ";
            
            if(root->getLeft()) {
                cout << setw(indent) << ' ' <<" \\\n";
                printTree2(root->getLeft(), indent+4);
            }
        }
    }
private:
void insert(Node *node) {
    Node *y = nullptr;
    Node *x = root;

    while (x != nullptr) {
        y = x;
        if (node->value < x->value)
            x = x->left;
        else
            x = x->right;
    }

    node->parent = y;
    if (y == nullptr)
        root = node;
    else if (node->value < y->value)
        y->left = node;
    else
        y->right = node;

    node->left = nullptr;
    node->right = nullptr;
    node->color = RED; // New nodes are always red

    // Fix the tree
    fixInsert(node);
}

void fixInsert(Node *&node) {
    Node *parent_node = nullptr;
    Node *grand_parent_node = nullptr;

    while ((node != root) && (node->color != BLACK) &&
           (node->parent->color == RED)) {

        parent_node = node->parent;
        grand_parent_node = node->parent->parent;

        /*  Case : A
            Parent of node is left child of Grand-parent of node */
        if (parent_node == grand_parent_node->left) {

            Node *uncle_node = grand_parent_node->right;

            /* Case : 1
               The uncle of node is also red
               Only Recoloring required */
            if (uncle_node != nullptr && uncle_node->color == RED) {
                grand_parent_node->color = RED;
                parent_node->color = BLACK;
                uncle_node->color = BLACK;
                node = grand_parent_node;
            } else {
                /* Case : 2
                   node is right child of its parent
                   Left-rotation required */
                if (node == parent_node->right) {
                    rotation_count++;   
                    rotateLeft(parent_node);
                    node = parent_node;
                    parent_node = node->parent;
                }

                /* Case : 3
                   node is left child of its parent
                   Right-rotation required */
                if (node == parent_node->left) {
                    rotation_count++;
                    rotateRight(grand_parent_node);
                    swap(parent_node->color, grand_parent_node->color);
                    node = parent_node;
                }
            }
        }

        /* Case : B
           Parent of node is right child of Grand-parent of node */
        else {
            Node *uncle_node = grand_parent_node->left;

            /*  Case : 1
                The uncle of node is also red
                Only Recoloring required */
            if ((uncle_node != nullptr) && (uncle_node->color == RED)) {
                grand_parent_node->color = RED;
                parent_node->color = BLACK;
                uncle_node->color = BLACK;
                node = grand_parent_node;
            } else {
                /* Case : 2
                   node is left child of its parent
                   Right-rotation required */
                if (node == parent_node->left) {
                    rotation_count++;
                    rotateRight(parent_node);
                    node = parent_node;
                    parent_node = node->parent;
                }

                /* Case : 3
                   node is right child of its parent
                   Left-rotation required */
                if (node == parent_node->right) {
                    rotation_count++;
                    rotateLeft(grand_parent_node);
                    swap(parent_node->color, grand_parent_node->color);
                    node = parent_node;
                }
            }
        }
    }

    root->color = BLACK;
}
Node* rotateRight(Node *node) {
    Node *temp_left = node->left;
    node->left = temp_left->right;
    if (node->left != nullptr)
        node->left->parent = node;

    temp_left->parent = node->parent;

    if (node->parent == nullptr)
        root = temp_left;
    else if (node == node->parent->left)
        node->parent->left = temp_left;
    else
        node->parent->right = temp_left;

    temp_left->right = node;
    node->parent = temp_left;
    return temp_left;
}

Node* rotateLeft(Node *node) {
    Node *temp_right = node->right;
    node->right = temp_right->left;
    if (node->right != nullptr)
        node->right->parent = node;

    temp_right->parent = node->parent;

    if (node->parent == nullptr)
        root = temp_right;
    else if (node == node->parent->left)
        node->parent->left = temp_right;
    else
        node->parent->right = temp_right;

    temp_right->left = node;
    node->parent = temp_right;
    return temp_right;
}
};

vector<int> generateSortedRepeats(int size, double repeat_rate, int seed) {
    vector<int> new_vector = vector<int>(0);
    for (int i=0; i<size*(1-repeat_rate); i++) {
        // Criar parte original
        new_vector.push_back(i);
    }
    for (int i=size*(1-repeat_rate); (int)new_vector.size()<size; i++) {
        // Criar parte q repete a parte original
        srand(seed+i);
        new_vector.push_back(rand() % (int)(size*(1-repeat_rate)));
    }

    // Sort de tudo
    sort(new_vector.begin(), new_vector.end());
    return new_vector;
}
//AVP melhor do que AVL nas inserções mas pior nas consultas
int main(){
    // Conjunto A: chaves por ordem crescente, com poucos repetidos.
    vector<int> setA = generateSortedRepeats(1e4, 0.1, 1);

    // Conjunto B: chaves por ordem decrescente, com poucos repetidos.
    vector<int> setB = setA;
    reverse(setB.begin(), setB.end());

    // Conjunto C: chaves por ordem aleatória, com poucos repetidos.
    vector<int> setC = setA;
    random_shuffle(setC.begin(), setC.end());

    // Conjunto D: chaves por ordem aleatória, com 90% de repetidos.
    vector<int> setD = generateSortedRepeats(1e4, 0.9, 1);
    random_shuffle(setD.begin(), setD.end());
    char temp = 'A' - 1;
    for (auto& set : {setA, setB, setC, setD}) {
        NormalBT simpleBT;
        SearchBT searchBT;
        AVLTree avlBT;
        RedBlackTree rbBT;
        cout << "\n Conjunto " << ++temp << "\n";
        auto start = chrono::high_resolution_clock::now();
        for (int value : set) {
            Node* node = new Node(value, nullptr, nullptr);
            simpleBT.insertNode(node);
        }
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> duration = end - start; 
        cout << "NormalBT - Tempo de inserção: " << duration.count() << " s\n";
        start = chrono::high_resolution_clock::now();
        for (int value : set) {
            Node * node = new Node(value, nullptr, nullptr);
            searchBT.insertNode(node);
        }
        end = chrono::high_resolution_clock::now();
        duration = end - start; 
        cout << "SearchBT - Tempo de inserção: " << duration.count() << " s\n";
        start = chrono::high_resolution_clock::now();
        for (int value : set) {
            Node * node = new Node(value, nullptr, nullptr);
            avlBT.insertNode(node);
        }
        end = chrono::high_resolution_clock::now();
        duration = end - start;
        cout << "AVLTree - Tempo de inserção: " << duration.count() << " s\n";
        cout << "AVLTree - Número de rotações: " << avlBT.rotation_count << "\n";
        avlBT.rotation_count = 0;
        start = chrono::high_resolution_clock::now();
        for (int value : set) {
            Node * node = new Node(value, nullptr, nullptr);
            rbBT.insertNode(node);
        }
        end = chrono::high_resolution_clock::now();
        duration = end - start;
        cout << "RedBlackTree - Tempo de inserção: " << duration.count() << " s\n";
        cout << "RedBlackTree - Número de rotações: " << rbBT.rotation_count << "\n";
        rbBT.rotation_count = 0;
        //delete AVLTree
        
    }
    return 0;
}
