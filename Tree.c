#include <stdlib.h>
#include <string.h>
#include "Tree.h"

// debug messages
static const char ALLOCATE[] = " - Allocating]\n";
static const char COST_READ[] = "[Cost Increment (Disk Access): Reading ";
static const char COST_WRITE[] = "[Cost Increment (Disk Access): Writing ";
static const char DEALLOCATE[] = " - Deallocating]\n";
static const char TREE[] = "[Tree ";

template <class Whatever>
int Tree<Whatever>::debug_on = 0;

template <class Whatever>
long Tree<Whatever>::cost = 0;

template <class Whatever>
long Tree<Whatever>::operation = 0;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define THRESHOLD 2

template <class Whatever>
ostream & operator << (ostream &, const TNode<Whatever> &);

template <class Whatever>
struct  TNode {
// friends:

// data fields:
	Whatever data;
	long height;
	long balance;
	offset left;
	offset right;
	offset this_position;	// current position

// function fields:
	TNode () : height (0), balance (0), left (0), right (0), 
		this_position (0) {}

	// to declare the working TNode in Tree's Remove
	TNode (Whatever & element) : data (element), height (0), balance (0),
		left (0), right (0), this_position (0) {}
	
	TNode (Whatever &, fstream *, long &);	// to add new node to disk
	TNode (const offset &, fstream *);	// to read node from disk
	
	unsigned long Insert (Whatever &, fstream *, long &, offset &);
	// optional recursive Lookup declaration would go here
	unsigned long Lookup (Whatever &, fstream *);
	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
		long fromSHB = FALSE);
	void ReplaceAndRemoveMax (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;
};

template <class Whatever>
void Tree<Whatever> :: Set_Debug_Off(void) {
	debug_on = 0; // set debug to off.
}


template <class Whatever>
void Tree<Whatever> :: Set_Debug_On(void) {
	debug_on = 1; // set debug to on.
}

template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element) {
	IncrementOperation();
	long status;
	
	if(element) {
		status = FALSE;
	} 

	if (occupancy == 0) { // if there is not root, insert the new element as the root
		TNode<Whatever> TempNode(element, fio, occupancy); // write in datafile
		status = TRUE;

	} else { // if there is already a root, call the TNode's insert
		TNode<Whatever> readRootNode (root, fio);
		return readRootNode.Insert(element, fio, occupancy, root);
	}
	return status; // successful insert
}

template <class Whatever>
void TNode<Whatever> :: ReplaceAndRemoveMax (TNode<Whatever> & targetTNode, 
	fstream * fio, offset & PositionInParent) {
	if(right) {
		TNode<Whatever> rightNode(right, fio);

		rightNode.ReplaceAndRemoveMax(targetTNode, fio, right);

		SetHeightAndBalance(fio, PositionInParent);

	} else {
		// replace with the max TNode in the left subtree
		targetTNode.data = data;
		PositionInParent = left;
	}
}

template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
	long status = 0; // whether the remove was successful or not
	bool notReplaced = true; // Whether the TNode was replaced
	long temp = 0; // ajsdkf;ajk;dsf
	
	if (elementTNode.data == data) { // if the node is found in tree
		elementTNode.data = data; // set the element to the data
		
		if (occupancy > 1) { // there is more Tnodes besides the root
			
			if (!right && !left) { // if there are no children
				PositionInParent = NULL;
			
			// if left child exists but no right
			} else if (!right && left) {
				// set the PointerInParent to that node
				PositionInParent = left; 

			// right child exists but left doesn't
			} else if (right && !left) { 
				// set the PointerInParent to the right child
				PositionInParent = right; 
			
			} else if (right && left) { // if has two children
				
				// replace the node with the proper child
				TNode<Whatever> LeftNode(left, fio); // recursive
				LeftNode.ReplaceAndRemoveMax(*this, fio, left); 
				notReplaced = false; // the node is replaced
			}
		}

		if (!fromSHB) { // if the function was not called from SHB
			SetHeightAndBalance(fio, PositionInParent); 
		} 

		//if (notReplaced) { // if ReplaceAndRemoveMax was not called
	//		delete(this); // delete that node
	//	}

		status = 1; // the removal was a success

	// the data is greater than the current data
	} else if (elementTNode.data > data) {

		if (!right) { // look right, if there is nothing there, failure
			status = 0;

		} else { // if there is a right child, recurse on the right 
			TNode<Whatever> RightNode(right, fio); // recursive
			status = RightNode.Remove(elementTNode, fio, temp, right, FALSE); 

			if (!fromSHB) { // if the function was not called from SHB
				SetHeightAndBalance(fio, PositionInParent); 
			} 
		}

	} else { // if element's data is less than current data, go left

		if (!left) { // if nothing is at the left, remove is a failure
			status = 0;

		} else { // if left child exists, recurse on the left child.
			TNode<Whatever> LeftNode(left, fio); // recursive
			status = LeftNode.Remove(elementTNode, fio, temp, left, FALSE); // recursive call to insert

			if (!fromSHB) { // if function not from SHB, call SHB
				SetHeightAndBalance(fio, PositionInParent);
			} 
		}
	}
	
	return status; // return if success or failure
}
	
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element) {
	IncrementOperation();
	long status = 0; // whether the removal is a success or not

	if (occupancy == 0) { // if there is nothing in the tree, return false
		return 0;

	} else { // if there is something in the tree
		TNode<Whatever> tempNode (element);
		TNode<Whatever> readRootNode(root, fio);
		
		// remove the node from th tree
		status = readRootNode.Remove(tempNode, fio, occupancy, root, FALSE);
		
		element = tempNode.data; // update the previous data with the new element's data
		occupancy--;

		if (occupancy == 0) {
			ResetRoot();
		}
	}

    return status; // return whether the removal was a success
}

template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent) {
	long leftHeight = -1;
	long rightHeight = -1;

	if (left) {
		TNode<Whatever> LeftNode(left, fio); // recursive
		leftHeight = LeftNode.height;
	}
	if (right) {
		TNode<Whatever> RightNode(right, fio); // recursive
		rightHeight = RightNode.height;
	} 
	balance = leftHeight - rightHeight;
	height = leftHeight>rightHeight? leftHeight+1 : rightHeight+1;

	if (abs(balance) > THRESHOLD) { // if balance exceeds threshld
		long temp = 0;
		// store in temporary node
		TNode<Whatever> tempNode (data);
		// remove the node from th tree
		Remove(tempNode, fio, temp, PositionInParent, TRUE);
		// reinsert the node into the tree
		TNode<Whatever> newNode(PositionInParent, fio);
		newNode.Insert(tempNode.data, fio, temp, PositionInParent);
	}		 
	
	Write(fio);
}

template <class Whatever>
long Tree <Whatever> :: GetCost () {
	return Tree<Whatever>::cost;
}

template <class Whatever>
long Tree <Whatever> :: GetOperation () {
	return Tree<Whatever>::operation;
}

template <class Whatever>
void Tree <Whatever> :: IncrementCost () {
	Tree<Whatever>::cost++;
}

template <class Whatever>
void Tree <Whatever> :: IncrementOperation () {
	Tree<Whatever>::operation++;
}

template <class Whatever>
void Tree <Whatever> :: ResetRoot () {
	fio -> seekp(0, ios:: end);
	root = fio -> tellp();
}

template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent) {

	if (element == data) { // found duplicate
		data = element; // update the previous data with the new element's data
		TNode<Whatever> TmpNode(element, fio, occupancy);
	
	} else if (element > data) { // if element is greater than data, go right
		if (right) { // if there is nothing at the right
			TNode<Whatever> RightNode(right, fio); // recursive
			RightNode.Insert(element, fio, occupancy, right);
		
		} else {
			TNode<Whatever> RightNode(element, fio, occupancy); // recursive
			right = RightNode.this_position;
		}

	} else { // element is less than data, go left
		if (left) { // if nothing is at left, insert at left
			TNode<Whatever> LeftNode(left, fio); // recursive
			LeftNode.Insert(element, fio, occupancy, left);

		} else { 
			TNode<Whatever> LeftNode(element, fio, occupancy); // recursive
			left = LeftNode.this_position;
		}
	}
	
	SetHeightAndBalance(fio, PositionInParent); // update the height and balance
	return 1;
}

template <class Whatever>
unsigned long TNode<Whatever> :: Lookup (Whatever & element, fstream * fio) {
	if (element == data) { // the element is found
		element = data; // set the element to the data
		return 1; // element is in the tree

	} else if (element > data) { // element is greater than the current data
		if (!right) { // if nothing is at the right, not in tree
			return 0;

		} else { // if right exists, recurse using right node
			TNode<Whatever> RightNode(right, fio); // recursive
		    return RightNode.Lookup(element, fio);
		}

	} else { // element is less than current data, want to go left
		if (!left) { // if no left exists, not in tree
			return 0;
		} else { 
			TNode<Whatever> LeftNode(left, fio); // recursive
			return LeftNode.Lookup(element, fio);
		}
	}
}

template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const {
	IncrementOperation();
	
	if (occupancy == 0) { // if there is nothing in the tree, the lookkup fails
		return 0;
	} else { // if there is something in the tree
		TNode<Whatever> readRootNode (root, fio);
		return readRootNode.Lookup(element, fio);
	}
	return 1;
}

template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio) {
	fio -> seekg(position);
	fio -> read ((char*)this, sizeof(TNode<Whatever>));
	
	Tree<Whatever>:: IncrementCost();	
	if (Tree<Whatever> :: debug_on) {
		cerr<< COST_READ << (const char*)data << "]\n";
	}
}

// tnode constructor for reading
template <class Whatever> 
TNode<Whatever> :: TNode (const offset & position, fstream * fio) {
	Read(position, fio);
}

// tnode constructor for writing
template <class Whatever>
TNode<Whatever> :: TNode (Whatever & element, fstream * fio, long & occupancy): 
			data (element), height (0), balance (0), left (0), 
			right (0) {
	occupancy++;
	fio -> seekp(0, ios::end);
	this_position = fio -> tellp();
	Write(fio);
}

template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const {
	fio -> seekp(this_position);
	fio -> write((const char *)this, sizeof(TNode<Whatever>));

	Tree<Whatever>:: IncrementCost();
	if (Tree<Whatever> :: debug_on) {
		cerr<< COST_WRITE << (const char*)data << "]\n";
	}
}

template <class Whatever>
Tree<Whatever> :: Tree (const char * datafile) :
	fio (new fstream (datafile, ios :: out | ios :: in)) {
		fio -> seekp(0, ios::beg);
		offset beginning = fio -> tellp();
		fio -> seekp(0, ios::end);
		offset ending = fio -> tellp();
		occupancy = 0;
		tree_count = 0;

		if (beginning == ending) {
			root = 0;
			// file is empty, reserve space for root and occupancy
			fio -> seekp(0, ios::end);
			fio -> write((const char *) &root, sizeof(root));
			fio -> write((const char *) &occupancy, sizeof(occupancy));

			// sets the root to the next location after the offsets
			root = fio -> tellp(); 
		} else {
			fio -> seekg(0, ios::beg);
			fio -> read ((char*)&root, sizeof(root));
			fio -> read ((char*)&occupancy, sizeof(occupancy));
		}
		tree_count++;

		if (debug_on) {
			cerr << TREE << tree_count << ALLOCATE;
		}
}

template <class Whatever>
Tree<Whatever> :: ~Tree (void)
/***************************************************************************
% Routine Name : Tree :: ~Tree  (public)
% File :         Tree.c
% 
% Description :  deallocates memory associated with the Tree.  It
%                will also delete all the memory of the elements within
%                the table.
***************************************************************************/
{	
	if (debug_on) {
		cerr<< TREE << tree_count << DEALLOCATE;
	}
	fio -> seekp(0, ios::beg);
	fio -> write((const char *) &root, sizeof(root));
	fio -> write((const char *) &occupancy, sizeof(occupancy));
	delete fio;
}	/* end: ~Tree */

template <class Whatever>
ostream & operator << (ostream & stream, const TNode<Whatever> & nnn) {
	stream << "at height:  :" << nnn.height << " with balance:  "
		<< nnn.balance << "  ";
	return stream << nnn.data << "\n";
}

template <class Whatever>
ostream & Tree<Whatever> :: Write (ostream & stream) const
/***************************************************************************
% Routine Name : Tree :: Write (public)
% File :         Tree.c
% 
% Description : This funtion will output the contents of the Tree table
%               to the stream specificed by the caller.  The stream could be
%               cerr, cout, or any other valid stream.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% stream             A reference to the output stream.
% <return>           A reference to the output stream.
***************************************************************************/

{
        long old_cost = cost;

	stream << "Tree " << tree_count << ":\n"
		<< "occupancy is " << occupancy << " elements.\n";

	fio->seekg (0, ios :: end);
	offset end = fio->tellg ();

	// check for new file
	if (root != end) {
		TNode<Whatever> readRootNode (root, fio);
		readRootNode.Write_AllTNodes (stream, fio);
	}

        // ignore cost when displaying nodes to users
        cost = old_cost;

	return stream;
}

template <class Whatever>
ostream & TNode<Whatever> ::
Write_AllTNodes (ostream & stream, fstream * fio) const {
	if (left) {
		TNode<Whatever> readLeftNode (left, fio);
		readLeftNode.Write_AllTNodes (stream, fio);
	}
	stream << *this;
	if (right) {
		TNode<Whatever> readRightNode (right, fio);
		readRightNode.Write_AllTNodes (stream, fio);
	}

	return stream;
}

