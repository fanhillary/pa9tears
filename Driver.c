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

ostream & operator << (ostream & stream, const UCSDStudent & stu) {
        return stream << "name:  " << stu.name
                << " with studentnum:  " << stu.studentnum;
}

int main (int argc, char * const * argv) {
        char buffer[BUFSIZ];
        char command;
        long number;
        char option;
        
		istream * is = &cin;
		ostream * os = &cout;
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
			/*
			if (*is == EOF) {
				if (*is != cin) { // if already reading from file
					is = &cin; // switch to cin to read from keyboard
				} else {
					break;
				}
			} 
			*/
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

				case 'f': {
				
					*os << "Please enter file name for commands: ";
					*is >> buffer;

					if (*is != cin) { // is input is from file
						delete is; // delete and reset os streams
						is = &cin;
						delete os;
						os = &cout;
					}

					is = new ifstream(buffer);
					os = new ofstream("/dev/null");
					break;
				}
			}
        }

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
