#pragma once
typedef struct tree_t {  // TODO: Here you have to rewrite to OOP
	tree_t* left_node;
	tree_t* rigth_node;
	tree_t* parent_node; // XXX: Should we have/use link to parent node?
	std::string value;
} tree_t;