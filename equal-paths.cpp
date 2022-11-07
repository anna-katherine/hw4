#include "equal-paths.h"
using namespace std;


// You may add any prototypes of helper functions here
int countLength(Node* p)
{
	if (p == nullptr)
		return 0;
	if ((countLength(p->left)+1) > (countLength(p->right)+1))
	{
		return countLength(p->left)+1;
	}
	else
	{
		return countLength(p->right)+1;
	}
}

bool equalPaths(Node * root)
{
    // Add your code below
		if (root == nullptr)
			return true;
		if (countLength(root->left) == countLength(root->right) || (countLength(root->left) <= 1 && countLength(root->right) <= 1)) // check if paths are equal OR if there's only one node or none at all
			return true;
		return false;
		

}

