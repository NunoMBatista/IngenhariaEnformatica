#pragma once
#include <bits/stdc++.h>
#include "Node.cpp"
using namespace std;

class BinarySearchTree {
public:
    Node* root;
    BinarySearchTree() : root(nullptr) {}

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
        if (root == nullptr){
            root = new Node(value);
        } 
        else{
            insertOrderly(root, value);
        }
    }

    bool contains(int value){
        Node* node = root;
        while(true){
            if(node == nullptr){
                return false;
            }
            if(value < node->data){
                node = node->left;
            }
            else if(value > node->data){
                node = node->right;
            }
            else if(value == node->data){
                return true;
            }
        }
    }

private:
    void insertOrderly(Node* node, int value) {
        while(true){
            if(node == nullptr){
                node = new Node(value);
                return;
            }
            if(value < node->data){
                if(node->left == nullptr){
                    node->left = new Node(value);
                    return;
                }
                node = node->left;
            }
            else if(value > node->data){
                if(node->right == nullptr){
                    node->right = new Node(value);
                    return;
                }
                node = node->right;
            }
            else if(value == node->data){
                // Do not add a repeated value
                return;
            }
        }
    }
};