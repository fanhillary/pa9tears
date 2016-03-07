/****************************************************************************

                                                        Hillary Fan
                                                        CSE 12, Winter 2016
                                                        February 29, 2016
                                                        cs12xib
                                Assignment Eight

File Name:      Driver.c
Description:    The program holds the UCSDStudent class that implements all the
				functions and constructors for the UCSDStudent object. It holds
				the constructor, destructor, the function to get the name of
				the UCSDStudent and it can check if the objects are equal or
				greater than other UCSDStudent objects. 
**************************************************************************/
#ifndef DRIVER_H
#define DRIVER_H

#include <string.h>
#include <iostream>
#include <cstdlib>
using namespace std;

//=========================================================================
// class UCSDStudent
//
// Description: Implements the UCSDStudent object that holds the name of the 
//				student and the student number. The class holds the
//				constructor, destructor, and implements the operators "=="
//				to check equality, ">" to compare the objects, and "*" to
//				get the name of the UCSDStudent object. 
//
// Data Fields:
//		name: The element name that the UCSDStudent holds
//		studentnum: The number or value that the UCSDStudent holds
//
// Public functions:
//	   UCSDStudent - Constructor for  UCSDStudent object
//	   UCSDStudent - Copy constructor for a UCSDStudent object
//     delete_AllNodes - deletes all the children of the current node
//     insert - inserts a data into the current node or to one of the
//              children of the current node
//     Write - display the current node
//     Write_AllNodes - display the current node and all its children
//
//=========================================================================
class UCSDStudent {
	// declaration of operator << as a friend to give it permission
	friend ostream & operator << (ostream &, const UCSDStudent &);
	char name[20]; //the name array of the UCSDStudent
	long studentnum; // the number of the UCSDStudent

public:
/***************************************************************************
% Routine Name : UCSDStudent (public)
% File :         Driver.h
% 
% Description : This function will construct a UCSDStudent object by
%				initializing its name and student number.
%
% Parameters descriptions :
% name               description
% ------------------ ------------------------------------------------------
% nm				 The desired name that the UCSDStudent object will hold
% val				 The desired value for the studentnum of the UCSDStudent
%
% Result:  An UCSDStudent object with the passed in name and studentnum
***************************************************************************/
	UCSDStudent (char * nm, long val = 0) : studentnum (val) {
		memset(name, '\0', sizeof(name));
		// set the name and student number to the object
		strcpy (name, nm);
	}


	/***** BLHAHHAHAHAHAHAHAH*/
	UCSDStudent (void) : studentnum (0) {
		memset(name, '\0', sizeof(name));
	}

/***************************************************************************
% Routine Name : UCSDStudent (public)
% File :         Driver.h
% 
% Description : A copy constructor for a UCSDStudent object by
%				initializing its name and student number to that of another
%				UCSDStudent.
%
% Parameters descriptions :
% name               description
% ------------------ ------------------------------------------------------
% student			 The UCSDStudent that you want to make a copy of
%
% Result:  An UCSDStudent object with the passed in name and studentnum of
%		   another UCSDStudent object
***************************************************************************/
	UCSDStudent (const UCSDStudent & student) {
		memset (name, '\0', sizeof(name));
		strcpy (name, student.name);
		studentnum = student.studentnum;
	}


/***************************************************************************
% Routine Name : * (public)
% File :         Driver.h
% 
% Description : A function operator for getting the name of the UCSDStudent.
%
% Result:  The name of the UCSDStudent is returned.
***************************************************************************/
	operator const char *(void) const {
		return name; // return the name of the UCSDStudent
	}


/***************************************************************************
% Routine Name : == (public)
% File :         Driver.h
% 
% Description : A function operator for checking for equivalence between two
%				UCSDStudents.
%
% Result:  Returns true if the UCSDStudents are equal, else return false.
***************************************************************************/
	long operator == (const UCSDStudent & sss) const {
		return ! strcmp (name, sss.name); // compares to see if equal
	}

/***************************************************************************
% Routine Name : > (public)
% File :         Driver.h
% 
% Description : A function operator for comparing two UCSDStudent to see if the
%				left hand side is greater than the object on the right.
%
% Result:  Returns true if the UCSDStudent is greater than the other
%		   UCSDStudent, else return false.
****************************************************************************/
long operator > (const UCSDStudent & sss) const {
		// compares to see if greater than the other student.
		return (strcmp (name, sss.name) > 0) ? 1 : 0;
	}
};

#endif
