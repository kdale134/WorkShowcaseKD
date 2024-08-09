//============================================================================
// Name        : Project_Two-Algorithms-KD
// Author      : Kyle Dale
// Version     : 1.0
// Copyright   : Copyright © 2017 SNHU COCE
// Description : Hello World in C++, Ansi-style, Additional functionality from prior codebase left in so ABCU can expand functionality easily. 
//============================================================================

#include <iostream>
#include <fstream>
#include<string>
#include"CSVparser.hpp"

using namespace std;
/*
	Global definitions
*/
// define structure to hold course information.
struct Course {
	string courseId;
	string courseName;
};

// internal structure for tree node.
struct Node {
	Course course;
	Node* left;
	Node* right;


	// default constructor
	Node() {
		left = nullptr;
		right = nullptr;
	}

	// inititalize with a class
	Node(Course aCourse) {
		course = aCourse;
	}
};

/*
	BST Class Definition
*/

class BinarySearchTree {
private:
	Node* root;
	void addNode(Node* node, Course course);
	void inOrder(Node* node);

public:
	BinarySearchTree();
	virtual ~BinarySearchTree();
	void inOrder();
	void insert(Course course);
	Course Search(string courseId);
};

/*
	Default Constructor
*/
BinarySearchTree::BinarySearchTree() {
	root = nullptr;
}

/*
	Destructor
*/
BinarySearchTree::~BinarySearchTree() {
	if (root = nullptr) {
		delete(root->left);
		delete(root->right);
		delete root;
	}
}

/*
	Traverse Tree in order
*/
void BinarySearchTree::inOrder() {
	inOrder(root);
}

/*
	Insert a bid
*/
void BinarySearchTree::insert(Course course) {
	if (root = nullptr) {
		root = new Node(course);
	}
	else {
		addNode(root, course);
	}
}

/*
	Search for specific bid.
*/

Course BinarySearchTree::Search(string courseId) {
	Node* node = root;
	while (node != nullptr) {
		if (node->course.courseId == courseId) {
			return node->course;
		}
		if (courseId < node->course.courseId) {
			node = node->left;
		}
		else {
			node = node->right;
		}
	}
	Course course;
	return course;
}


/*
	addNode to BST
*/
void BinarySearchTree::addNode(Node* node, Course course) {
	if (course.courseId < node->course.courseId) {
		if (node->left = nullptr) {
			node->left = new Node(course);
		}
		else {
			addNode(node->left, course);
		}
	}
	else {
		if (node->right == nullptr) {
			node->right = new Node(course);
		}
		else {
			addNode(node->right, course);
		}
	}
}

void BinarySearchTree::inOrder(Node* node) {
	if (node != nullptr) {
		inOrder(node->left);
		cout << node->course.courseId << ": " << node->course.courseName << endl;
		inOrder(node->right);
	}

}

/*
	Static Testing Method
*/
void displayCourse(Course course) {
	cout << course.courseId << ": " << course.courseName << endl;
	return;
}

/*
	Load file containing courses
	FIX ME
*/
void loadCourses(string csvPath, BinarySearchTree* bst) {
	cout << "Loading CSV file " << csvPath << endl;

	//initialize the CSV Parser using the given path
	csv::Parser file = csv::Parser(csvPath);

	// read and display header row - optional
	/*vector<string> header = file.getHeader();
	for (auto const& c : header) {
		cout << c << " | ";
	}
	cout << "" << endl;
	*/
	try {
		// loop to read rows of a CSV file
		for (unsigned int i = 0; i < file.rowCount(); i++) {

			// Create a data structure and add to the collection of bids
			Course course;
			course.courseId = file[i][1];
			course.courseName = file[i][0];
			

			//cout << "Item: " << bid.title << ", Fund: " << bid.fund << ", Amount: " << bid.amount << endl;

			// push this bid to the end
			bst->insert(course);
		}
	}
	catch (csv::Error& e) {
		std::cerr << e.what() << std::endl;
	}
}


/*
	Main method
*/
int main(int argc, char* argv[]) {

	// process command line arguments
	string csvPath, bidKey;
	switch (argc) {
	case 2:
		csvPath = argv[1];
		bidKey = "98109";
		break;
	case 3:
		csvPath = argv[1];
		bidKey = argv[2];
		break;
	default:
		csvPath = "Courses.csv";
		bidKey = "98109";
	}

	// Define a binary search tree to hold all bids
	BinarySearchTree* bst;
	bst = new BinarySearchTree();
	Course course;

	int choice = 0;
	while (choice != 9) {
		cout << "Menu:" << endl;
		cout << "  1. Load Bids" << endl;
		cout << "  2. Display All Bids" << endl;
		cout << "  3. Find Bid" << endl;
		cout << "  9. Exit" << endl;
		cout << "Enter choice: ";
		cin >> choice;

		switch (choice) {

		case 1:

			// Complete the method call to load the bids
			loadCourses(csvPath, bst);
			break;

		case 2:
			bst->inOrder();
			break;

		case 3:
			course = bst->Search(bidKey);
			if (!course.courseId.empty()) {
				displayCourse(course);
			}
			else {
				cout << "Course Id " << bidKey << " not found." << endl;
			}
			break;
		}
	}

	cout << "Good bye!" << endl;

	return 0;
}
