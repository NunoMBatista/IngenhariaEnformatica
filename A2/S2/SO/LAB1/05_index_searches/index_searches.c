/*
 *      index_searches.c
 *      
 *      Copyright 2009 Bruno Cabral <bcabral@dei.uc.pt>
 *      
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *      
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *      
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 *      MA 02110-1301, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

typedef struct __item {
		int id;
		char product_name[256];
		float price;
		struct __item* next;
		struct __item* previous;
} list_item;

typedef struct __index_item {
		char letter;
		list_item* item;
		struct __index_item* next;
} index_item;

//Create and return the head 
//element of the list
list_item* create_list();
//Eliminate the list (free memory) and return NULL
list_item* destroy_list(list_item* head);
//Check if the list is empty
int is_empty(list_item* head);
//Insert new item into list sorted by name
void insert_item(list_item* head, int id, char* product_name, float price);
//remove item from list
void remove_item(list_item* head, list_item* item_to_remove);
//read list elements from file and insert sorted by name
void read_from_file(list_item* head, char* path);
//Search an element on the list using the index to speedup the process
list_item* search_list(list_item* head, index_item* index, char* product_name);
//Print product info on screen
void print_item(list_item* item);


//Create and return the head element of the index
index_item* create_index();
//Update index
void update_index(index_item* index, list_item* head);
//Eliminate the index (free memory) and return NULL
index_item* destroy_index(index_item* head);

int main(int argc, char** argv)
{
	list_item* head_sorted_list = NULL;
	index_item* head_index = NULL;
	list_item* search_result = NULL;
	char path[256];
	char token[256];
	char tmp[8];
	printf("Insert path: ");
	scanf("%s",path);
	printf("\nReading from %s...", path);
	head_sorted_list = create_list();
	read_from_file(head_sorted_list, path);
	printf("\nCreating index...");
	head_index = create_index();
	update_index(head_index,head_sorted_list);
	
	while (1) {
		printf("\nInsert name of product: (or \"exit\" to leave)");
		scanf("%s",token);
		if (strcmp(token,"exit") == 0)
			break;
		printf("\nSearching for %s...", token);		
		search_result = search_list(head_sorted_list, head_index, token);
		if (search_result != NULL) {
			print_item(search_result);
			printf("\nDelete (Y/N)?");
			scanf("%s",tmp);
			if (tmp[0] == 'Y' || tmp[0] == 'y') {
					remove_item(head_sorted_list, search_result);
					update_index(head_index, head_sorted_list);
					printf("\nItem eliminated...");
			}
		} else
			printf("\nproduct not found... try again!");
	}
	destroy_list(head_sorted_list);
	destroy_index(head_index);
	return 0;
}
