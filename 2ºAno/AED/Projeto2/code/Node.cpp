#pragma once
#include <bits/stdc++.h>
using namespace std;

enum Color {RED, BLACK};

struct Node {
    int data;
    int height;
    int balanceFactor;
    Color color;
    Node* parent;
    Node* left;
    Node* right;

    Node(int value) : data(value), height(1), balanceFactor(0), color(RED), parent(nullptr), left(nullptr), right(nullptr) {}
};