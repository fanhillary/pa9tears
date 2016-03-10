/****************************************************************************

                                                        Hillary Fan
                                                        CSE 12, Winter 2016
                                                        February 16, 2016
                                                        cs12xib
                                Assignment Nine

File Name:      Tree.c
Description:    The program implments an AVL Binary Tree data structure. The
				imlemented data structure is a self-balancing tree that
				organizes the elements it holds from "least" to "greatest". The
				program holds functions that can allocate a TNode object.
				The user can insert elements into the Tree and the program
				will find the proper location so that the elements are in
				increasing order. Each TNode has a left and a right pointer. If
				the element is smaller than the current node, it will go to the
				left pointer, else it will go to the right. The user can also 
				lookup objects in the tree and remove the elements from the
				tree. If the element removed had two children, a proper
				successor will be found and will replace the removed node. The
				program can also list out the elements that the tree currently
				holds. It also constantly update the height and balance at each
				of the nodes. At the end of the program, it will deallocate the
				entire tree and all of the nodes and the memory that it holds.
**************************************************************************/
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

//=========================================================================
// struct TNode
//
// Description: The struct implements the Tnode of the binary search tree
//				structure. Each Tnode has a left and a right pointer/child.
//				The left child will be less than the data of the
//				current node and the right child will hold the data
//				greater than the current node. It also defines the
//				balance and height of the TNode defines the Tree's
//				occupancy and tree_count.
//
// Data Fields:
//     data (Whatever)  - the data held by the current node
//     left (TNode<Whatever> *)  - the left child pointer
//     right (TNode<Whatever> *) - the right child pointer
//
// Public functions:
//	   TNode - constructor for a TNode root
//	   TNode - constructor for a regular TNode that is not the root
//	   ~TNode - destructor for a TNode that changes pointers to NULL
//     delete_AllNodes - Recurses to the bottom of the tree and deletes each
//						 node from bottom to top of the tree.
//     ReplaceAndRemoveMax  - If removing node with two children, this function
//							  is called in order to replace the removed node
//							  with a new successor.
//	   SetHeightAndBalance - Updates the height and balance of all the nodes
//							 in the tree by traversing through the tree. If
//							 the balance is over the threshold, the node
//							 will be removed and reinserted.
//     Insert - inserts a data into the current node or to one of the
//              children of the current node
//     Remove - Remove a certain node from the tree and reassigning the 
//				PointerInParent appropriately.
//     Lookup - lookup a specific node from the tree and return true if
//				found.
//==========================================================================
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

    // recursive definition of the TNode Lookupmethod
	unsigned long Lookup (Whatever &, fstream *);
	void Read (const offset &, fstream *);	// read node from disk
	unsigned long Remove (TNode<Whatever> &, fstream *, long &, offset &,
		long fromSHB = FALSE);
	void ReplaceAndRemoveMax (TNode<Whatever> &, fstream *, offset &);
	void SetHeightAndBalance (fstream *, offset &);
	void Write (fstream *) const;		// update node to disk

	ostream & Write_AllTNodes (ostream &, fstream *) const;
};

/***************************************************************************
% Routine Name : Tree<Whatever> :: Set_Debug_Off
% File :         Tree.c
% 
% Description : Turn off the debug option.
%
% Result: Debug messages will not show.
***************************************************************************/
template <class Whatever>
void Tree<Whatever> :: Set_Debug_Off(void) {
	debug_on = 0; // set debug to off.
}

/***************************************************************************
% Routine Name : Tree<Whatever> :: Set_Debug_On
% File :         Tree.c
% 
% Description : Turn on the debug option.
%
% Result: Debug messages will show.
***************************************************************************/
template <class Whatever>
void Tree<Whatever> :: Set_Debug_On(void) {
	debug_on = 1; // set debug to on.
}

/***************************************************************************
% Routine Name : Tree<Whatever> :: Insert (public)
% File :         Tree.c
% 
% Description : This function will insert a new element into the correct
%				location in the Tree. If there is nothing in the tree, it
%				inserts as the root, else it will call the TNode's insert to
%				determine where the element should be located.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% element            The element to insert into the tree.
%
% Result: A new TNode inserted into the tree.
******************************************************************************/
template <class Whatever>
unsigned long Tree<Whatever> :: Insert (Whatever & element) {
	IncrementOperation();
	long status;
	
	if(!element) {
		status = FALSE;
	} 

	if (occupancy == 0) { // if there is not root, insert new element as root
		TNode<Whatever> TempNode(element, fio, occupancy); // write in datafile
		status = TRUE;

	} else { // if there is already a root, call the TNode's insert
		TNode<Whatever> readRootNode (root, fio);
		return readRootNode.Insert(element, fio, occupancy, root);
	}
	return status; // successful insert
}

/***************************************************************************
% Routine Name : ReplaceAndRemoveMax (public)
% File :         Tree.c
% 
% Description : This function will remove a TNode with two children from the 
%				Tree structure. It will recurse to the right and find a new
%				successor for the node that is going to be removed. It will 
%				then reassign the target node's data to that of the successor
%				and reassign the PointerInParent properly.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% targetTNode       The element to remove and replace in the tree
% PointerInParent   The pointer from the parent that points at current node
%
% Result:  A tree with one less node and a different data in the place of the
%		   removed node.
***************************************************************************/	
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

		if(left) {
			PositionInParent = left;
		} else {
			PositionInParent = NULL;
		}
	}
}

/***************************************************************************
% Routine Name : Remove (public)
% File :         Tree.c
% 
% Description : This function will remove the TNode from the Tree structure.
%				After finding the desired node to remove, it will see how many
%				children it has. It will then reassign the PointerInParent
%				accordingly. If there are 2 children, it will call the
%				ReplaceAndRemoveMax function to find a new successor to take
%				its place and keep the structure of the tree.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% elementTNode       The element to look for to remove from the tree.
% PointerInParent    The pointer from the parent that points at current node
% fromSHB            Whether the function was called from SetHeightAndBalance
%
% Result: Return 0 for a failure in removal or 1 if the removal was a sucess.
%         A tree with one less node if the removal was a sucess. 
***************************************************************************/
template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
	long status = 0; // whether the remove was successful or not
	bool notReplaced = true; // Whether the TNode was replaced
	
	if (elementTNode.data == data) { // if the node is found in tree
		elementTNode.data = data; // set the element to the data
		
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
			//right = 0;
			//left = 0;

		if (!fromSHB) { // if the function was not called from SHB
			SetHeightAndBalance(fio, PositionInParent);
			//SetHeightAndBalance(fio, PositionInParent, false);
		} else {
			Write(fio);
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
			status = RightNode.Remove(elementTNode, fio, occupancy, right, FALSE); 

			if (!fromSHB) { // if the function was not called from SHB
				SetHeightAndBalance(fio, PositionInParent); 
			} 
		}

	} else { // if element's data is less than current data, go left

		if (!left) { // if nothing is at the left, remove is a failure
			status = 0;

		} else { // if left child exists, recurse on the left child.
			TNode<Whatever> LeftNode(left, fio); // recursive
			status = LeftNode.Remove(elementTNode, fio, occupancy, left, FALSE); // recursive call to insert

			if (!fromSHB) { // if function not from SHB, call SHB
				SetHeightAndBalance(fio, PositionInParent);
			} 
		}
	}
	
	return status; // return if success or failure
}

/***************************************************************************
% Routine Name : Tree<Whatever> :: Remove (public)
% File :         Tree.c
% 
% Description : This function will remove the TNode from the Tree structure.
%				If there are nodes in the tree, it will create a TNode and
%				call TNode's remove to delete the node.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% element			 The element to remove from the tree
%
% Result: Return 0 for a failure in removal or 1 if the removal was a sucess.
%         A tree with one less node if the removal was a sucess. 
***************************************************************************/	
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

/***************************************************************************
% Routine Name : SetHeightAndBalance (public)
% File :         Tree.c
% 
% Description : This function will update the height and balance of the current
%				node according to the height of its children. If the balance 
%				exceeds the threshold of 2 balance, the unbalanced node will be
%				removed and will be reinserted into the tree.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% PointerInParent    The pointer from the parent that points at current node
%
% Result:  Updated height and balance of the current Tnode.
***************************************************************************/	
template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent) {
	// bool notFromRem =1 as last param
	long leftHeight = -1;
	long rightHeight = -1;
		long temp = 0;

	if (left) {
		TNode<Whatever> LeftNode(left, fio); // recursive
		leftHeight = LeftNode.height;
	}

	if (right) {
		TNode<Whatever> RightNode(right, fio); // read right node
		rightHeight = RightNode.height; 
	} 

	balance = leftHeight - rightHeight;
	height = leftHeight>rightHeight? leftHeight+1 : rightHeight+1;

	if (abs(balance) > THRESHOLD) { // if balance exceeds threshld
		
		// store in temporary node
		TNode<Whatever> tempNode (data);

		// remove the node from th tree
		Remove(tempNode, fio, temp, PositionInParent, TRUE);
		// reinsert the node into the tree
		TNode<Whatever> newNode(PositionInParent, fio);
		newNode.Insert(tempNode.data, fio, temp, PositionInParent);
	} else {
		Write(fio);
	}
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


/***************************************************************************
% Routine Name : TNode<Whatever> :: Insert (public)
% File :         Tree.c
% 
% Description : This function will insert a new element into the correct
%				location in the Tree. The smaller items go on the left and the
%				greater items go right. After looping through the tree and
%				inserting the new element into the tree, update the balance and
%				height data for each of the TNodes.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% element            The element to insert into tree.
% PointerInParent    The pointer from the parent to get to the current TNode.
%
% Result: A new TNode inserted into the tree.
***************************************************************************/
template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent) {

	if (element == data) { // found duplicate
		data = element; // update the previous data with the new element's data
	
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

/***************************************************************************
% Routine Name : TNode<Whatever> :: Lookup (public)
% File :         Tree.c
% 
% Description : This function will lookup the desired node and return whether
%				that element was found in the tree or not. It traverses through
%				the tree using recursion.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% element			 The element the user wants to lookup.
%
% Result:  Retursn whether the element is found in the tree or not..
***************************************************************************/
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

/***************************************************************************
% Routine Name : Tree<Whatever> :: Lookup (public)
% File :         Tree.c
% 
% Description : This function will look for the TNode that is equivalent to the
%               element passed in. If there is nothing in the tree, the lookup
%				returns false, else it will call the TNode's lookup to traverse
%				through the tree and look for the element.
%
% Parameters descriptions :
% 
% name               description
% ------------------ ------------------------------------------------------
% element            The element to look for a match in the tree.
%
% Result: Return 1 if the TNode is found, else return 0 if not found
%		  in the Tree.
***************************************************************************/
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

