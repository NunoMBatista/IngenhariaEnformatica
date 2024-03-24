#pragma once
#include <bits/stdc++.h>
#include "Node.cpp"
using namespace std;

int rotationsRB = 0; 

class RedBlackTree {
public:
    Node* root;
    
    RedBlackTree() : root(nullptr) {}

    void insert(int data) {
        Node* newNode = new Node(data); 

        Node* current = root;
        Node* parent = nullptr;

        //Regular BST insertion
        while (current != nullptr) { 
            parent = current;
            if (data < current->data) {
                current = current->left;
            } 
            else if(data > current->data) {
                current = current->right;
            }
            // Node already exists
            else{
                return;
            }
        }

        // Set the parent of the new node
        newNode->parent = parent; 
        // If the tree is empty, the new node is the root
        if (parent == nullptr) { 
            root = newNode;
        }
        // check if the new node is a left or right child
        else if (data < parent->data) {
            parent->left = newNode; // right child
        } 
        // 
        else {
            parent->right = newNode; // left child
        }

        // The new node is created as red by default

        // Function to maintain the properties of the RB Tree
        fixInsertion(newNode); 
    }

private:
    void rotateLeft(Node* node) {
        rotationsRB++;

        // Get the right child of the node
        Node* rightChild = node->right;
        // The right child of the node is now the left child of the right child
        node->right = rightChild->left;

        // Check if the left child of the right child exists and set the parent
        if (node->right != nullptr) {
            node->right->parent = node;
        }

        // The parent of the right child is now the parent of the current node
        rightChild->parent = node->parent;

        // If the parent of the current node is null, the right child is the new root
        if (node->parent == nullptr) {
            root = rightChild;
        } 
        // If the current node is the parent's left child, the right child is set as the left child
        else if (node == node->parent->left) {
            node->parent->left = rightChild;
        }
        // If the current node is the parent's right child, the right child is set as the right child
        else {
            node->parent->right = rightChild;
        }
        
        // The current node is now the left child of the right child
        rightChild->left = node;
        // The parent of the current node is now the right child
        node->parent = rightChild;
    }

    void rotateRight(Node* node) {
        rotationsRB++;
        Node* leftChild = node->left;
        node->left = leftChild->right;

        if (node->left != nullptr) {
            node->left->parent = node;
        }

        leftChild->parent = node->parent;

        if (node->parent == nullptr) {
            root = leftChild;
        } 
        else if (node == node->parent->left) {
            node->parent->left = leftChild;
        }
        else {
            node->parent->right = leftChild;
        }

        leftChild->right = node;
        node->parent = leftChild;
    }

    void fixInsertion(Node* node) {
        // While the parent of the node is red, the tree is not a valid RB Tree
        while (node != root && node->parent->color == RED) { 
            // Check if the node's parent is the left child of the grandparent
            if (node->parent == node->parent->parent->left) {
                // Get the uncle of the node, which is the right child of the grandparent
                Node* uncle = node->parent->parent->right;

                // If the uncle is red, change the colors of the parent, uncle and grandparent
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } 
                // If the uncle is black, check if the node is the right child of the parent
                else {
                    if (node == node->parent->right) {
                        node = node->parent;
                        rotateLeft(node);
                    }

                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateRight(node->parent->parent);
                }
            } 
            // If the parent of the node is the right child of the grandparent
            else {
                // Get the uncle of the node, which is the left child of the grandparent
                Node* uncle = node->parent->parent->left;
                
                // If the uncle is red, change the colors of the parent, uncle and grandparent
                if (uncle != nullptr && uncle->color == RED) {
                    node->parent->color = BLACK;
                    uncle->color = BLACK;
                    node->parent->parent->color = RED;
                    node = node->parent->parent;
                } 
                // If the uncle is black, check if the node is the left child of the parent
                else {
                    if (node == node->parent->left) {
                        node = node->parent;
                        rotateRight(node);
                    }

                    node->parent->color = BLACK;
                    node->parent->parent->color = RED;
                    rotateLeft(node->parent->parent);
                }
            }
        }
        
        root->color = BLACK;
    }

};
