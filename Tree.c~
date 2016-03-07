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

	// MAGIC NUMBERRRRRR how do I tell if the root is empty? If it returns
	// 00000000, how do I check the equivalence?
	
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
	/* YOUR CODE GOES HERE */
}

template <class Whatever>
unsigned long TNode<Whatever> :: Remove (TNode<Whatever> & elementTNode,
	fstream * fio, long & occupancy, offset & PositionInParent,
	long fromSHB) {
	/* YOUR CODE GOES HERE */
}
	
template <class Whatever>
unsigned long Tree<Whatever> :: Remove (Whatever & element) {
	IncrementOperation();
	/* YOUR CODE GOES HERE */
}

template <class Whatever>
void TNode<Whatever> :: SetHeightAndBalance (fstream * fio,
	offset & PositionInParent) {
	/* YOUR CODE GOES HERE */
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
        /* YOUR CODE GOES HERE */       
}

template <class Whatever>
unsigned long TNode<Whatever> :: Insert (Whatever & element, fstream * fio,
	long & occupancy, offset & PositionInParent) {
/*
	if (element == data) { // found duplicate
		if(Tree<Whatever> :: debug) { // debug message for inserting
			cerr << TREENUM << tree_count << INSERT << (const char *)element <<
			"]\n";
		}

		data = element; // update the previous data with the new element's data
	
	} else if (element > data) { // if element is greater than data, go right
		if (!right) { // if there is nothing at the right

			if(Tree<Whatever> :: debug) { // debug message for insertion
				cerr << TREENUM << tree_count << INSERT <<
				(const char *)element << "]\n";
			}

			right = new TNode<Whatever>(element, *this); // insert at right
		
		} else {
			right -> Insert(element, right); // recursive call to insert
		}

	} else { // element is less than data, go left
		if (!left) { // if nothing is at left, insert at left

			if(Tree<Whatever> :: debug) { // debug message for insertion
				cerr << TREENUM << tree_count << INSERT <<
				(const char *)element << "]\n";
			}

			left = new TNode<Whatever>(element, *this); // insert at left

		} else { 
			left -> Insert(element, left); // recursive call to insert
		}
	}
	
	SetHeightAndBalance(PointerInParent); // update the height and balance
	return 1;

*/
}

template <class Whatever>
unsigned long Tree<Whatever> :: Lookup (Whatever & element) const {
	IncrementOperation();
	/* YOUR CODE GOES HERE */
}

template <class Whatever>
void TNode<Whatever> :: Read (const offset & position, fstream * fio) {
	fio -> seekg(position);
	fio -> read ((char*)this, sizeof(TNode<Whatever>));
	
	Tree<Whatever>:: IncrementCost();	
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
	fio -> seekp(0, ios::beg);
	fio -> write((const char *) &root, sizeof(root));
	fio -> write((const char *) &occupancy, sizeof(occupancy));

	fio -> seekp(0, ios::end);
	this_position = fio -> tellp();
	Write(fio);
}

template <class Whatever>
void TNode<Whatever> :: Write (fstream * fio) const {
	fio -> seekp(this_position);
	fio -> write((const char *)this, sizeof(TNode<Whatever>));

	Tree<Whatever>:: IncrementCost();	
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

