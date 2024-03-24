#pragma once
#include <bits/stdc++.h>
#include "Node.cpp"
using namespace std;

int rotationsAVL = 0; 

class AVLTree {
public:
    Node* root;
    AVLTree() : root(nullptr) {}

    int height(Node* node) {
        if (node == nullptr) {
            return -1;
        }
        return node->height;
    }

    // The balance factor is given by the difference between the height of the left and right subtrees
    int balanceFactor(Node* node){
        if (node == nullptr) {
            return 0;
        }
        return height(node->left) - height(node->right);
    }
    
    void insert(int data) {
        // If the tree is empty, create a new node
        if (root == nullptr){
            root = new Node(data);
        } 
        else{
            root = insertOrderly(root, data);
        }
    }

private:
    Node* rightRotate(Node* node){
        if (node == nullptr) {
            return nullptr;
        }

        rotationsAVL++; 
        Node* newRoot = node->left; 

        // If the new root is null, return the node in order to avoid segmentation faults
        if (newRoot == nullptr) {
            return node;
        }

        Node* temp = newRoot->right;

        newRoot->right = node;
        node->left = temp;

        // Update heights and account for null pointers
        node->height = 1 + max(height(node->left ? node->left : nullptr), height(node->right ? node->right : nullptr)); 
        newRoot->height = 1 + max(height(newRoot->left ? newRoot->left : nullptr), height(newRoot->right ? newRoot->right : nullptr));
        return newRoot;
    }
    
    Node* leftRotate(Node* node){
        if (node == nullptr) {
            return nullptr;
        }

        rotationsAVL++;
        Node* newRoot = node->right;

        if (newRoot == nullptr) {
            return node;
        }

        Node* temp = newRoot->left;

        newRoot->left = node;
        node->right = temp;

        node->height = 1 + max(height(node->left ? node->left : nullptr), height(node->right ? node->right : nullptr));
        newRoot->height = 1 + max(height(newRoot->left ? newRoot->left : nullptr), height(newRoot->right ? newRoot->right : nullptr));
        return newRoot;
    }
    
    // Insert orderly just like in the BST, but with the added AVL rotations
    Node* insertOrderly(Node* node, int data){
        if (node == nullptr){
            return new Node(data);
        }
        if(data < node->data){
            node->left = insertOrderly(node->left, data);
        }
        else if(data > node->data){
            node->right = insertOrderly(node->right, data);
        }
        else{
            return node;
        }
        
        // Update height and balance factor
        node->height = 1 + max(height(node->left), height(node->right));
        node->balanceFactor = balanceFactor(node);
        int bf = balanceFactor(node);

        // Case 1: Left Left
        if(bf > 1 && data < node->left->data){ 
            return rightRotate(node);
        }
        // Case 2: Right Right
        if(bf < -1 && data > node->right->data){
            return leftRotate(node);
        }
        // Case 3: Left Right
        if(bf > 1 && data > node->left->data){
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }
        // Case 4: Right Left
        if(bf < -1 && data < node->right->data){
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }
        return node;
    }
};
