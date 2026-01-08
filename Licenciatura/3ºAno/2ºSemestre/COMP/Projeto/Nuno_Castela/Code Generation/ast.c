#include <stdlib.h>
#include <stdio.h>
#include "ast.h"
#include <string.h>

// Create a node of a given category with a given lexical symbol
struct node *new_node(enum category category, char *token) {
    struct node *new = malloc(sizeof(struct node));
    new->category = category;
    new->token = token;
    new->type = none;
    new->parameter_list = NULL;
    new->children = malloc(sizeof(struct node_list));
    new->children->node = NULL;
    new->children->next = NULL;
    return new;
}

// Append a node to the list of children of the parent node
void add_child(struct node *parent, struct node *child) {
    if(parent == NULL || child == NULL){
        return;
    }

    struct node_list *new = malloc(sizeof(struct node_list));
    new->node = child;
    new->next = NULL;
    struct node_list *children = parent->children;
    while(children->next != NULL)
        children = children->next;
    children->next = new;
}

// Remove the first child from the list of children of the parent node
void remove_first_child(struct node *parent) {
    struct node_list *children = parent->children;
    if(children->next == NULL)
        return;
    struct node_list *new = children->next;
    parent->children = new;
    free(children);
}

// get a pointer to a specific child, numbered 0, 1, 2, ...
struct node *get_child(struct node *parent, int position) {
    struct node_list *children = parent->children;
    while((children = children->next) != NULL)
        if(position-- == 0)
            return children->node;
    return NULL;
}

// Names defined in ast.h
char *category_name[] = names;
char *type_name[] = t_name;

// Perform a depth-first search to print the AST
void dfs(struct node *cur_node, int depth){
    if(cur_node == NULL){
        return;
    }

    // Add indentation according to the current depth
    for(int i = 0; i < depth; i++){
        printf("..");
    }    
    // Print the category and the token (if it exists)
    if(cur_node->token == NULL){
        printf("%s", category_name[cur_node->category]);
    }

    else{
        printf("%s(%s)", category_name[cur_node->category], cur_node->token);
    }

    // Print the type (if it exists)
    if(cur_node->type != none){
        printf(" - %s", type_name[cur_node->type]);
    }
    if(cur_node->parameter_list != NULL){
        if(strcmp(cur_node->parameter_list, "undef") == 0){
            printf(" - undef");
        }
        else{
           printf(" - (%s)", cur_node->parameter_list);
        }
    }


    printf("\n");
    
    // Visit all children
    if(cur_node->children == NULL){
        return;
    }
    struct node_list *child = cur_node->children;
    while((child = child->next) != NULL){
        dfs(child->node, depth+1);
    }
}

// Free the AST
void free_ast (struct node * cur_node){
    if(cur_node == NULL){
        return;
    }

    struct node_list *child = cur_node->children;
    while(child != NULL){
        struct node_list *next = child->next;
        
        if(child->node != NULL){
            free_ast(child->node);
        }
        free(child);
        child = next;
    }
    if(cur_node->token != NULL){
        free(cur_node->token);
        cur_node->token = NULL;
    }
    free(cur_node);
}

// free the AST
void deallocate(struct node *node) {
    if(node != NULL) {
        struct node_list *child = node->children;
        while(child != NULL) {
            deallocate(child->node);
            struct node_list *tmp = child;
            child = child->next;
            free(tmp);
        }
        if(node->token != NULL)
            free(node->token);
        free(node);
    }
}

/*
Count the number of block elements in order to 
determine whether or not the node is supperfluous
*/
int block_elements(struct node* cur_node){
    int count = 0;
    if(cur_node == NULL){
        return count;
    }

    struct node_list *child = cur_node->children;
    while((child = child->next) != NULL){
        if(child->node->category != AUX){
            count++;
        }
        else{
            // If the child node is an auxiliary node, we need to count its children
            count += block_elements(child->node);
        }
    }
    return count;
}

void remove_aux(struct node *parent) {
    if (parent == NULL || parent->children == NULL) {
        return;
    }

    struct node_list *prev = parent->children;
    struct node_list *current = prev->next;

    while (current != NULL) {
        if (current->node->category == AUX) {
            // The AUX node's children should go into it's parent's child list at the AUX node's position
            struct node_list *aux_children = current->node->children->next;

            if (aux_children != NULL) {
                // Find the last AUX child
                struct node_list *last_aux_child = aux_children;
                while (last_aux_child->next != NULL) {
                    last_aux_child = last_aux_child->next;
                }

                // Link previous parent node to AUX node's first child
                prev->next = aux_children;

                // Link last AUX child to AUX node's next node in the parent's list
                last_aux_child->next = current->next;
            } 
            else {
                // The AUX node is a leaf
                prev->next = current->next;
            }

            // Free the AUX node and it's children
            free(current->node->children);
            free(current->node);


            struct node_list *temp = current;
            current = prev->next; // Changed from current->next to prev->next
            free(temp);

            // Only update prev if the current node was not removed
        } 
        else {
            // Continue DFS
            remove_aux(current->node);
            prev = current;
            current = current->next;
        }
    }
}