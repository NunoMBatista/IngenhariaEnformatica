#pragma once
#include <bits/stdc++.h>
#include "Node.cpp"
using namespace std;

class BinaryTree {
public:
    Node* root;
    BinaryTree() : root(nullptr) {}

    void remove(int value) {
        // This function will swap the value of the target node with the value of the deepest node and then delete the deepest node
        if (root == nullptr) {
            return;
        }

        queue<Node*> queue;
        queue.push(root);

        Node* targetNode = nullptr;
        Node* deepestNode = nullptr;

        // BFS search for the target node and the deepest node
        while (!queue.empty()) {
            Node* currNode = queue.front();
            queue.pop();

            // Store the node to be deleted
            if (currNode->data == value) {
                targetNode = currNode;
            }

            // If the current node still has children, they must be the deepest yet
            if (currNode->left != nullptr) {
                queue.push(currNode->left);
                deepestNode = currNode->left;
            }

            if (currNode->right != nullptr) {
                queue.push(currNode->right);
                deepestNode = currNode->right;
            }
        }

        if (targetNode != nullptr) {
            // Swap the value of the target node with the value of the deepest node
            targetNode->data = deepestNode->data;
            deleteDeepestNode(deepestNode);
        }
    }

    void deleteDeepestNode(Node* node) {
        queue<Node*> queue;
        queue.push(root);

        // BFS search for the parent of the deepest node 
        while (!queue.empty()) {
            Node* currNode = queue.front();
            queue.pop();

            if (currNode->right != nullptr) {
                // If the current node is the parent of the deepest node, remove the link to the deepest node
                if (currNode->right == node) {
                    currNode->right = nullptr;
                    delete node;
                    return;
                } 
                else {
                    queue.push(currNode->right);
                }
            }

            if (currNode->left != nullptr) {
                // If the current node is the parent of the deepest node, remove the link to the deepest node
                if (currNode->left == node) {
                    currNode->left = nullptr;
                    delete node;
                    return;
                } 
                else {
                    queue.push(currNode->left);
                }
            }
        }
    }

    void inOrderTraversal() {
        if (root == nullptr) {
            return;
        }

        stack<Node*> stack;
        Node* currNode = root;

        while (currNode != nullptr || !stack.empty()) {
            // Go to the left most node of the current node
            while (currNode != nullptr) {
                stack.push(currNode);
                currNode = currNode->left;
            }
            
            // Print the current node and remove it from the stack
            currNode = stack.top();
            stack.pop();

            // Print the current node (it will only be printed after all of its left children are printed)
            cout << currNode->data << " ";

            // Start searching from the right node in the next cycle
            currNode = currNode->right;
        }
    }

    void preOrderTraversal() {
        if (root == nullptr) {
            return;
        }

        // Non recursive DFS (using a stack)
        std::stack<Node*> stack;
        stack.push(root);

        while (!stack.empty()) {
            Node* currNode = stack.top();
            stack.pop();

            // Always print the current node before adding more to the stack
            std::cout << currNode->data << " ";

            if (currNode->right != nullptr) {
                stack.push(currNode->right);
            }

            // The left node is pushed last so that it is processed first (FIFO)
            if (currNode->left != nullptr) {
                stack.push(currNode->left);
            }
        }
    }

    void postOrderTraversal() {
        if (root == nullptr) {
            return;
        }

        stack<Node*> stack;
        Node* currNode = root;
        Node* lastVisitedNode = nullptr;

        while (currNode != nullptr || !stack.empty()) {
            // Go to the left most node of the current node
            if (currNode != nullptr) {
                stack.push(currNode);
                currNode = currNode->left;
            } 
            else {
                // The top node is the current left most node
                Node* topNode = stack.top();
                // Check the right side of the top node
                if (topNode->right != nullptr && topNode->right != lastVisitedNode) {
                    currNode = topNode->right;
                } 
                else {
                    // If the right side is null or it has already been visited, the top node can be printed
                    cout << topNode->data << " ";
                    // Store the top node as the last visited node from this subtree
                    lastVisitedNode = topNode;
                    stack.pop();
                }
            }
        }
    }
    
    void insert(int value) {
        // Check if the tree is empty
        if (root == nullptr){
            root = new Node(value);
        } 
        else{
            insertRandomly(root, value);
        }
    }

    bool contains(int value) {
        if (root == nullptr) {
            return false;
        }

        queue<Node*> queue;
        queue.push(root);

        // BFS search for the target node
        while (!queue.empty()) {
            Node* currNode = queue.front();
            queue.pop();

            if (currNode->data == value) {
                return true;
            }

            if (currNode->left != nullptr) {
                queue.push(currNode->left);
            }
            if (currNode->right != nullptr) {
                queue.push(currNode->right);
            }
        }

        return false;
    }

private:
    void insertRandomly(Node* node, int value) {
        if(contains(value)){
            // Do not add repeated values
            return;
        }
       
        while(true){
            // Decide randomly whether to go left or right
            int direction = rand() % 2; // 0 for left, 1 for right
            if (direction == 0) { // Go left
                if (node->left){
                    node = node->left;
                }
                else{
                    node->left = new Node(value);
                    return;
                }
            } 
            else{ // Go right
                if (node->right){
                    node = node->right;
                }
                else{
                    node->right = new Node(value);
                    return;
                }
            }
        }
    }
};