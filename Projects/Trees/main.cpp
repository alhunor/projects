#include <stdio.h>
#include <conio.h>
#include <iostream>


struct node
{
	node *p, *left, *right;
	node(int d = 0) { p = left = right = NULL; key = d; }
	int key;
};

class tree
{
public:
	tree() : root(NULL) {}
	void insert(int d);
	bool isBinarySearchTree();
	void inorder();
	void inorder2();
	node* find(int key);
	bool isEmpty() { return root != NULL; }
	void tree::transplant(node* n); // from CLRS
	void tree::remove2(node* n); // from CLRS
	void tree::remove(node* n); // from Hunor

	int min()
	{
		_ASSERT(root != NULL);
		return min(root)->key;
	}
	int max()
	{
		_ASSERT(root != NULL);
		return max(root)->key;
	}



//private:
	node* root;
	void inorder(node* n);
	bool isBinary(node* n);
	node* max(node* n);
	node* min(node* n);
	node* tree::successor(node* n);
};


node* tree::find(int key)
{
	return NULL;
}


bool tree::isBinary(node* n)
{
	if (n->left && n->left->key > n->key) return false;
	if (n->right && n->right->key < n->key) return false;
	return isBinary(n->left) && isBinary(n->right);
}

node* tree::min(node* n)
{
	if (!n->left) return n;
	return min(n->left);
}

node* tree::max(node* n)
{
	if (!n->right) return n;
	return min(n->right);
}

node* tree::successor(node* n)
{
	_ASSERT(n != NULL);
	if (n->right) return min(n->right);
	node* y = n->p;
	while (y!=NULL && y->right == n)
	{
		n = y;
		y = y->p;
	}
	return y;
}


bool tree::isBinarySearchTree()
{
	if (!root) return true;
	return isBinary(root);
}

void tree::inorder()
{
	std::cout << "Inorder: ";
	inorder(root);
	std::cout << std::endl;
}

void tree::inorder2()
{
	std::cout << "Inorder2: ";
	node * n = min(root);
	while (n != NULL)
	{
		std::cout << n->key << " ";
		n = successor(n);
	}
	std::cout << std::endl;
}


void tree::inorder(node* n)
{
	if (n == NULL) return;
	inorder(n->left);
	std::cout << n->key << " ";
	inorder(n->right);

}

void tree::insert(int d)
{
	node* z = new node(d);
	if (root == NULL)
	{
		root = z;
		return;
	}
	node *x = root;
	while (x != NULL)
	{
		z->p = x;
		if (d < x->key)
		{
			x = x->left;
		}
		else
		{
			x = x->right;
		}
	}
	x = z->p;
	if (d < x->key) x->left = z; else x->right = z;
} // void tree::insert(int d)


void tree::transplant(node* n) // from CLRS
{

}

void tree::remove2(node* n) // from CLRS
{

}

void tree::remove(node* n) // from Hunor
{

}

void main()
{
	tree t;
	t.insert(6);
	t.insert(16);
	t.insert(-6);
	t.insert(7);
	t.insert(19);
	t.insert(9);
	t.insert(1);
	t.insert(2);
	t.insert(10);
	t.insert(5);
	t.insert(4);
	t.insert(15);
	t.insert(20);

	t.inorder();

	_getch();
}

