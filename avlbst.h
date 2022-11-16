#ifndef RBBST_H
#define RBBST_H

#include <iostream>
#include <exception>
#include <cstdlib>
#include <cstdint>
#include <algorithm>
#include "bst.h"

struct KeyError { };

/**
* A special kind of node for an AVL tree, which adds the balance as a data member, plus
* other additional helper functions. You do NOT need to implement any functionality or
* add additional data members or helper functions.
*/
template <typename Key, typename Value>
class AVLNode : public Node<Key, Value>
{
public:
    // Constructor/destructor.
    AVLNode(const Key& key, const Value& value, AVLNode<Key, Value>* parent);
    virtual ~AVLNode();

    // Getter/setter for the node's height.
    int8_t getBalance () const;
    void setBalance (int8_t balance);
    void updateBalance(int8_t diff);

    // Getters for parent, left, and right. These need to be redefined since they
    // return pointers to AVLNodes - not plain Nodes. See the Node class in bst.h
    // for more information.
    virtual AVLNode<Key, Value>* getParent() const override;
    virtual AVLNode<Key, Value>* getLeft() const override;
    virtual AVLNode<Key, Value>* getRight() const override;

protected:
    int8_t balance_;    // effectively a signed char
};

/*
  -------------------------------------------------
  Begin implementations for the AVLNode class.
  -------------------------------------------------
*/

/**
* An explicit constructor to initialize the elements by calling the base class constructor and setting
* the color to red since every new node will be red when it is first inserted.
*/
template<class Key, class Value>
AVLNode<Key, Value>::AVLNode(const Key& key, const Value& value, AVLNode<Key, Value> *parent) :
    Node<Key, Value>(key, value, parent), balance_(0)
{

}

/**
* A destructor which does nothing.
*/
template<class Key, class Value>
AVLNode<Key, Value>::~AVLNode()
{

}

/**
* A getter for the balance of a AVLNode.
*/
template<class Key, class Value>
int8_t AVLNode<Key, Value>::getBalance() const
{
    return balance_;
}

/**
* A setter for the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::setBalance(int8_t balance)
{
    balance_ = balance;
}

/**
* Adds diff to the balance of a AVLNode.
*/
template<class Key, class Value>
void AVLNode<Key, Value>::updateBalance(int8_t diff)
{
    balance_ += diff;
}

/**
* An overridden function for getting the parent since a static_cast is necessary to make sure
* that our node is a AVLNode.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getParent() const
{
    return static_cast<AVLNode<Key, Value>*>(this->parent_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getLeft() const
{
    return static_cast<AVLNode<Key, Value>*>(this->left_);
}

/**
* Overridden for the same reasons as above.
*/
template<class Key, class Value>
AVLNode<Key, Value> *AVLNode<Key, Value>::getRight() const
{
    return static_cast<AVLNode<Key, Value>*>(this->right_);
}


/*
  -----------------------------------------------
  End implementations for the AVLNode class.
  -----------------------------------------------
*/


template <class Key, class Value>
class AVLTree : public BinarySearchTree<Key, Value>
{
public:
    virtual void insert (const std::pair<const Key, Value> &new_item); // TODO
    virtual void remove (const Key& key);  // TODO
protected:
    virtual void nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2);

    // Add helper functions here
    // left and right rotate functions
    virtual void rotateRight (AVLNode<Key, Value> *z);
    virtual void rotateLeft (AVLNode<Key, Value> *x);
    // insertfix, removefix
    virtual void insertFix (AVLNode<Key,Value>* p, AVLNode<Key,Value>* n);
    virtual void removeFix (AVLNode<Key, Value>* n, int diff);
};

/*
 * Recall: If key is already in the tree, you should 
 * overwrite the current value with the updated value.
 */
template<class Key, class Value>
void AVLTree<Key, Value>::insert (const std::pair<const Key, Value> &new_item)
{
    // TODO
		AVLNode<Key, Value>* val = static_cast<AVLNode<Key, Value>*> (this->internalFind(new_item.first));
		AVLNode<Key,Value> *item = new AVLNode<Key,Value>(new_item.first, new_item.second, nullptr); // item to insert
		AVLNode<Key, Value> *curr =  static_cast<AVLNode<Key, Value>*>(this->root_);
		bool finished = false;
	  if (val != nullptr) // node already exists
		{
			val->setValue(new_item.second);
			delete item;
			return;
		}
		if (this->root_ == nullptr) // if empty tree
		{
				this->root_ = item;
				return;
		}
		else // insert n by walking to leaf and insert as child, set balance to 0, look at parent p
		{
			while (!finished)
			{
				if (item->getKey() < curr->getKey()) // if we go left
				{
					if (curr->getLeft() == nullptr) // if no left child
					{
						curr->setLeft(item);
						item->setParent(curr);
						finished = true;
					}
					else 
					{
						curr = curr->getLeft(); // keep going left
					}
				}
				else if (new_item.first > curr->getKey()) // we go right
				{ 
					if (curr->getRight() == nullptr) // no more rights
					{
						curr->setRight(item);
						item->setParent(curr);
						finished = true; 
					}
					else 
					{
						curr = curr->getRight();
					}
				}
			}
			if (curr->getBalance() != 0)
			{
				curr->setBalance(0);
			}
			else if (curr->getBalance() == 0)
			{
				if (curr->getRight() == item)
					curr->updateBalance(+1);
				else 
					curr->updateBalance(-1);
				insertFix(curr, item);
			}
		}
}

/*
 * Recall: The writeup specifies that if a node has 2 children you
 * should swap with the predecessor and then remove.
 */
template<class Key, class Value>
void AVLTree<Key, Value>:: remove(const Key& key)
{
	// TODO
	// find node to remove 
	AVLNode<Key, Value> *n = static_cast<AVLNode<Key,Value>*>(this->internalFind(key));
	if (n == nullptr)
		return; 
	if (n->getRight() != nullptr && n->getLeft() != nullptr)
	{
		AVLNode<Key, Value> *pred = static_cast<AVLNode<Key,Value>*>(this->predecessor(n));
		nodeSwap(pred, n);
	}
	AVLNode<Key, Value> *p = n->getParent();
	int diff = 0;
	if (p != nullptr)
	{
		// if n is a left child
		if (p->getLeft() == n)
		{
			diff = 1;
		}
		else // must be right child
		{
			diff = -1;
		}
		this->deleteNode(n);
		removeFix(p, diff);
	}
	else 
	{
		this->deleteNode(n);
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>:: rotateRight (AVLNode<Key, Value> *z)
{
    /*
    * taking a left child, making it the parent and
    * making the original parent the new right child
    */
		AVLNode<Key,Value> *y = z->getLeft();
		AVLNode<Key,Value> *c = y->getRight();	

	  if (z == this->root_) // if z is the parent, just rearrange
		{
			this->root_ = y; // y is parent, x left child, z right child, z's left child is c
			y->setParent(nullptr); 
			y->setRight(z);
			z->setParent(y);
			z->setLeft(c);
			if (c != nullptr)
				c->setParent(z);
		}
		else
		{
			AVLNode<Key,Value> *par = z->getParent(); // get z's current parent to set y's new parent
			y->setParent(par);
			y->setRight(z);
		  z->setParent(y);
			if (par->getRight() == z)
			{
				par->setRight(y);
			}
			else
			{
				par->setLeft(y);
			}
			z->setLeft(c);
			if (c != nullptr)
				c->setParent(z);
		}
}

template<class Key, class Value>
void AVLTree<Key, Value>:: rotateLeft (AVLNode<Key, Value> *x)
{
    /*
    * taking a right child, making it the parent and
    * making the original parent the new left child
    */
		AVLNode<Key,Value> *y = x->getRight();
		AVLNode<Key,Value> *b = y->getLeft();	

		if (x == this->root_) // if z is the parent, just rearrange
		{
			this->root_ = y; // y is parent, x left child, z right child, z's left child is c
			y->setParent(nullptr);
			y->setLeft(x); 
			x->setParent(y);
			x->setRight(b);
			if (b != nullptr)
				b->setParent(x);
		}
		else
		{
			AVLNode<Key,Value> *par = x->getParent();
			y->setParent(par);
			y->setLeft(x);
			x->setParent(y);
			if (par->getRight() == x)
			{
				par->setRight(y);
			}
			else
			{
				par->setLeft(y);
			}
			x->setRight(b);
			if (b != nullptr)
				b->setParent(x);
		}
}


template<class Key, class Value>
void AVLTree<Key, Value>:: insertFix (AVLNode<Key,Value>* p, AVLNode<Key,Value>* n)
{
  // TODO

	/*
	* p is parent of n
	* n is current node
	* c is taller child of n
	* g is grandchild of n
	*/

	// if p or parent = null, return
	if (p == nullptr ||  p->getParent() == nullptr)
		return;

	// let g = parent
	AVLNode<Key, Value> *g = p->getParent();
	// assume p is left child of g
	if (p == g->getLeft())
	{
		// update g balance by -1 for now 
		g->setBalance(g->getBalance() - 1);
		// case 1: if b(g) = 0, return 
		if (g->getBalance() == 0)
			return;
		// case 2: b(g) = -1, recurse 
		else if (g->getBalance() == -1)
			insertFix(g, p);
		// case 3: b(g) = -2, check zig/zag
		else if (g->getBalance() == -2)
		{
			if (g->getLeft()->getLeft() == n)
			{ 
					rotateRight(g);
					p->setBalance(0);
					g->setBalance(0);
			}
			else
			{
				rotateLeft(p);
				rotateRight(g);
				if (n->getBalance() == -1)
				{
						p->setBalance(0);
						g->setBalance(1);
						n->setBalance(0);
				}
				else if (n->getBalance() == 0)
				{
						p->setBalance(0);
						g->setBalance(0);
						n->setBalance(0);
				}
				else if (n->getBalance() == 1)
				{
						p->setBalance(-1);
						g->setBalance(0);
						n->setBalance(0);
				}
			}
		}
	}
	else if (p == g->getRight()) // same as left, just swap 
	{
		g->setBalance(g->getBalance() + 1); 
		if (g->getBalance() == 0)
			return;
		else if (g->getBalance() == 1)
		{
			insertFix(g, p);
			return;
		}
		else if (g->getBalance() == 2)
		{
			if (g->getRight()->getRight() == n)
			{
					rotateLeft(g);
					p->setBalance(0);
					g->setBalance(0);
			}
			else
			{
				rotateRight(p);
				rotateLeft(g);
				if (n->getBalance() == -1)
				{
						p->setBalance(1);
						g->setBalance(0);
						n->setBalance(0);
				}
				else if (n->getBalance() == 0)
				{
						p->setBalance(0);
						g->setBalance(0);
						n->setBalance(0);
				}
				else if (n->getBalance() == 1)
				{
						p->setBalance(0);
						g->setBalance(-1);
						n->setBalance(0);
				}
			}
		}
	}
}

template<class Key, class Value>
void AVLTree<Key, Value>:: removeFix (AVLNode<Key, Value>* n, int diff)
{
	// TODO

	/*
	* p is parent of n
	* n is current node
	* c is taller child of n
	* g is grandchild of n
	*/
	
	// return if n is null
	if (n == nullptr)
		return;
	// compute next recursive call's arguments now before altering
	AVLNode<Key, Value> *p = n->getParent();
	int ndiff = -1;
	if (p != nullptr)
	{
		if (n == p->getLeft()) // n is left child, ndiff = 1 (otherwise stays -1)
		{
			ndiff = 1;
		} 
	}
	// assume diff = -1
	if (diff == -1)
	{
		// case 1: b(n) + diff = -2
		if (n->getBalance() + diff == -2)
		{
			AVLNode<Key, Value> *c = n->getLeft();
			// case 1a
			if (c->getBalance() == -1)
			{
				rotateRight(n);
				n->setBalance(0);
				c->setBalance(0);
				removeFix(p, ndiff); // recursive call
			}
			// case 1b
			else if (c->getBalance() == 0)
			{
				rotateRight(n);
				n->setBalance(-1);
				c->setBalance(1);
			}
			// case 1c
			else if (c->getBalance() == 1)
			{
				// let g = right c 
				AVLNode<Key, Value> *g = c->getRight();
				rotateLeft(c);
				rotateRight(n);
				if (g->getBalance() == 1)
				{
					n->setBalance(0);
					c->setBalance(-1);
					g->setBalance(0);
				}
				else if (g->getBalance() == 0)
				{
					n->setBalance(0);
					c->setBalance(0);
					g->setBalance(0);
				}
				else if (g->getBalance() == -1)
				{
					n->setBalance(1);
					c->setBalance(0);
					g->setBalance(0);
				}
				removeFix(p, ndiff); // recursive call
			}
		}
		// case 2
		else if (n->getBalance() + diff == -1)
		{
			n->setBalance(-1);
		}
		// case 3 
		else if (n->getBalance() + diff == 0)
		{
			n->setBalance(0);
			removeFix(p, ndiff);
		}
	}
	else if (diff == 1) // repeat everything but mirrored...
	{
		if (n->getBalance() + diff == 2) // case 1 
		{
			AVLNode<Key, Value> *c = n->getRight();
			// case 1a
			if (c->getBalance() == 1)
			{
				rotateLeft(n);
				n->setBalance(0);
				c->setBalance(0);
				removeFix(p, ndiff); // recursive call
			}
			// case 1b
			else if (c->getBalance() == 0)
			{
				rotateLeft(n);
				n->setBalance(1);
				c->setBalance(-1);
			}
			// case 1c
			else if (c->getBalance() == -1)
			{
				// let g = left c 
				AVLNode<Key, Value> *g = c->getLeft();
				rotateRight(c);
				rotateLeft(n);
				if (g->getBalance() == -1)
				{
					n->setBalance(0);
					c->setBalance(1);
					g->setBalance(0);
				}
				else if (g->getBalance() == 0)
				{
					n->setBalance(0);
					c->setBalance(0);
					g->setBalance(0);
				}
				else if (g->getBalance() == 1)
				{
					n->setBalance(-1);
					c->setBalance(0);
					g->setBalance(0);
				}
				removeFix(p, ndiff); // recursive call
			}
		}
		else if (n->getBalance() + diff == 1) // case 2
		{
			n->setBalance(1);
		}
		else if (n->getBalance() + diff == 0) // case 3
		{
			n->setBalance(0);
			removeFix(p, ndiff);
		}
	} 
}


template<class Key, class Value>
void AVLTree<Key, Value>::nodeSwap( AVLNode<Key,Value>* n1, AVLNode<Key,Value>* n2)
{
    BinarySearchTree<Key, Value>::nodeSwap(n1, n2);
    int8_t tempB = n1->getBalance();
    n1->setBalance(n2->getBalance());
    n2->setBalance(tempB);
}


#endif
