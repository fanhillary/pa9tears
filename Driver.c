/****************************************************************************
                                                        Hillary Fan
                                                        Christine Do
                                                        CSE 12, Winter 2016
                                                        February 16, 2016
                                                        cs12xib
                                                        cs12xhy
                                Assignment Nine

File Name:      Driver.c
Description:    The program holds the driver that has the prompts to the
		program. It can perform lookup, insert, remove, and write
                operations. This driver also polymorphically switches 
                between reading input from the keyboard and reading from a
                file. When the program terminates, cost and operations of
                the program will be displayed to the user. 
**************************************************************************/
#include <iostream>
#include <cstdio>
#include <string>
#include <getopt.h>
#include <fstream>
#include "Driver.h"
#include "SymTab.h"

using namespace std;

/* DO NOT CHANGE:  This file is used in evaluation */

#ifdef NULL
#undef NULL
#define NULL 0
#endif
#define FROM_KEYBOARD 1 // constants to distinguish reading from keyboard
#define FROM_FILE 0 // constant to distinguish reading from file

ostream & operator << (ostream & stream, const UCSDStudent & stu) {
        return stream << "name:  " << stu.name
                << " with studentnum:  " << stu.studentnum;
}

int main (int argc, char * const * argv) {
        char buffer[BUFSIZ];
        char command;
        long number;
        char option;
		int readingFrom = FROM_KEYBOARD; // initialize to reading from keyboard
        
		istream * is = &cin; // declare istream object is to keyboard
		ostream * os = &cout; // assign ostream object to cout
        SymTab<UCSDStudent>::Set_Debug_Off ();

        while ((option = getopt (argc, argv, "x")) != EOF) {

        switch (option) {
                case 'x': SymTab<UCSDStudent>::Set_Debug_On ();
                        break;
                }       
        }
        
        SymTab<UCSDStudent> ST("Driver.datafile"); // reads from disk9 disk now
        ST.Write (cout << "Initial Symbol Table:\n" );

        
		while (cin) { 
			if (!*is) { // checks if it reads EOF
				if (readingFrom == FROM_FILE) { // if input is from file
					delete is; // delete the is pointer
					is = &cin; // switch to cin to read from keyboard
					delete os; // delete the os pointer
					os = &cout; // switch to cout to output regularly
					readingFrom = FROM_KEYBOARD; 

				} else {
					break; // terminate program
				}
			} 
			command = NULL;         // reset command each time in loop
			*os << "Please enter a command ((i)nsert, "
					<< "(l)ookup, (r)emove, (w)rite):  ";
			*is >> command;

			switch (command) {

				case 'i': {
					*os << "Please enter UCSD student name to insert:  ";
					*is >> buffer;  // formatted input

					*os << "Please enter UCSD student number:  ";
					*is >> number;

					UCSDStudent stu (buffer, number);

					// create student and place in symbol table
					ST.Insert (stu);
					break;
				}
				case 'l': { 
					unsigned long found;    // whether found or not

					*os << "Please enter UCSD student name to lookup:  ";
					*is >> buffer;  // formatted input

					UCSDStudent stu (buffer, 0);
					found = ST.Lookup (stu);
					
					if (found)
							cout << "Student found!!!\n" << stu << "\n";
					else
							cout << "student " << buffer << " not there!\n";
					break;
				}
				case 'r': { 
					unsigned long removed;

					*os << "Please enter UCSD student name to remove:  ";
					*is >> buffer;  // formatted input

					UCSDStudent stu (buffer, 0);
					removed = ST.Remove(stu);

					if (removed)
							cout << "Student removed!!!\n" << stu << "\n";
					else
							cout << "student " << buffer << " not there!\n";
					break;
				}
				case 'w': {
					ST.Write (cout << "The Symbol Table contains:\n");
					break;
				}
                                //
                                // Read from a file
                                //
				case 'f': {
					*os << "Please enter file name for commands: ";

                                        // prompt and reading in input
					*is >> buffer;

                                        // 
                                        // user is using the keyboard
                                        //
					if (readingFrom == FROM_KEYBOARD) {
                                                // start reading from file
						is = new ifstream(buffer);

						// change output location
						os = new ofstream("/dev/null");

                                                // switch from reading keyboard input to file
						readingFrom = FROM_FILE;
					}
					break;
				}
			}
        }

        //
        // Cost and Operations output
        // To be executed after the user ends the driver program with ^D
        //
        ST.Write (cout << "\nFinal Symbol Table:\n");

		if (ST.GetOperation() != 0) {
				cout << "\nCost of operations: ";
				cout << ST.GetCost();
				cout << " tree accesses";

				cout << "\nNumber of operations: ";
				cout << ST.GetOperation();

				cout << "\nAverage cost: ";
				cout << (((float)(ST.GetCost()))/(ST.GetOperation()));
				cout << " tree accesses/operation\n";
		}

		else {
				cout << "\nNo cost information available.\n";
		}
	}
