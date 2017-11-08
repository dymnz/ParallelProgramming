// C program for different tree traversals
#include <stdio.h>
#include <stdlib.h>

/* A binary tree node has data, pointer to left child
   and a pointer to right child */
struct node {
	int data;
	struct node* left;
	struct node* right;
};

/* Helper function that allocates a new node with the
   given data and NULL left and right pointers. */
struct node* newNode(int data) {
	struct node* node = (struct node*)
	                    malloc(sizeof(struct node));
	node->data = data;
	node->left = NULL;
	node->right = NULL;

	return(node);
}

/* Given a binary tree, print its nodes according to the
  "bottom-up" postorder traversal. */
int getPostorder(struct node* node) {
	int l=0, r=0;

	// first recur on left subtree
	if (node->left)
		l = getPostorder(node->left);

	// then recur on right subtree
	if (node->right)
		r = getPostorder(node->right);

	// now deal with the node
	return (l+r+node->data);
}


/* Driver program to test above functions*/
int main() {
	struct node *root				= newNode(1);
	root->left							= newNode(2);
	root->right							= newNode(3);
	root->left->left				= newNode(4);
	root->left->right				= newNode(5);
	root->right->left				= newNode(6);
	root->right->right			= newNode(7);
	root->left->left->left	= newNode(8);
	root->left->left->right	= newNode(9);


	printf("\nBy postorder traversal of binary tree, sum = %d \n\n", getPostorder(root));

	return 0;
}
