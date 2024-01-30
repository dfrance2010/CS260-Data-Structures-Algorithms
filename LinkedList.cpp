//============================================================================
// Name        : LinkedList.cpp
// Author      : David France
// Version     : 1.0
//============================================================================

#include <algorithm>
#include <iostream>
#include <time.h>

#include "CSVparser.hpp"

using namespace std;

//============================================================================
// Global definitions visible to all methods and classes
//============================================================================

// forward declarations
double strToDouble(string str, char ch);

// define a structure to hold bid information
struct Bid {
    string bidId; // unique identifier
    string title;
    string fund;
    double amount;
    Bid() {
        amount = 0.0;
    }
};

// I moved this to a forward declaration to use it in the PrintList() method.
void displayBid(Bid bid);

//============================================================================
// Linked-List class definition
//============================================================================

/**
 * Define a class containing data members and methods to
 * implement a linked-list.
 */
class LinkedList {

private:
    // Create internal structure for list entries and housekeeping variables
	struct Node {
		Bid bid;
		Node* next;

		Node() {
			next = nullptr;
		}

		Node(Bid aBid) {
			bid = aBid;
			next = nullptr;
		}
	};

	Node* head;
	Node* tail;
	int size = 0;

public:
    LinkedList();
    virtual ~LinkedList();
    void Append(Bid bid);
    void Prepend(Bid bid);
    void PrintList();
    void Remove(string bidId);
    Bid Search(string bidId);
    int Size();
};

/**
 * Default constructor
 */
LinkedList::LinkedList() {
    // Initialize housekeeping variables.
	head = tail = nullptr;
}

/**
 * Destructor
 */
LinkedList::~LinkedList() {
}

/**
 * Append a new bid to the end of the list
 */
void LinkedList::Append(Bid bid) {
    // Create a new node with the given bid. Using 'new' will pull memory from 'the heap', aka all available memory.
	Node* node = new Node(bid);

	// If the list is empty - head is a null pointer - node becomes head, else current tail node
	// now points to the new node.
	if (head == nullptr) {
		head = node;
	} else {
		tail->next = node;
	}

	// New node becomes the tail regardless of the size of the list and list size is incremented.
	tail = node;
	size++;
}

/**
 * Prepend a new bid to the start of the list
 */
void LinkedList::Prepend(Bid bid) {
    // Create a new node, just like in Append() method.
	Node* node = new Node(bid);

	// If the list is not empty the new node's next pointer will point to the current head.
	if (head != nullptr) {
		node->next = head;
	}

	// New node becomes the head, regardless of the size of the list and list size is incremented.
	head = node;
	size++;
}

/**
 * Simple output of all bids in the list
 */
void LinkedList::PrintList() {
	// Set current node to head to start iterating through entire list.
	Node* currNode = head;

	// As long as there is another node, display the bid stored in the node and move the pointer to the
	// next node.
	while(currNode != nullptr) {
		displayBid(currNode->bid);
		currNode = currNode->next;
	}

}

/**
 * Remove a specified bid
 *
 * @param bidId The bid id to remove from the list
 */
void LinkedList::Remove(string bidId) {

	// Special case for if the head exists and is the node remove.
	// Set head pointer to next node, delete head, then set next node to be new head.
	if(head != nullptr) {
		if (head->bid.bidId.compare(bidId) == 0) {
			Node* tempNode = head->next;
			delete head;
			head = tempNode;

			// I added a statement to let the user know the bid was removed and a
			// return statement to keep the program from running .
			cout << "Bid removed" << endl;
			return;
		}
	}

	// Iterate through nodes, starting with the head, until a match is found.
	// If the next node bid id is a match, current node points to node after next and
	// next node is deleted. Confirmation message is printed to screen and method returns
	// without further iteration.
	Node* currNode = head;
	while(currNode->next != nullptr) {
		if (currNode->next->bid.bidId.compare(bidId) == 0) {
			Node* tempNode = currNode->next;
			currNode->next = tempNode->next;
			cout << "Bid removed" << endl;
			delete tempNode;
			size--;
			return;
		}
		currNode = currNode->next;
	}

	cout << "Bid not found" << endl;

}

/**
 * Search for the specified bidId
 *
 * @param bidId The bid id to search for
 */
Bid LinkedList::Search(string bidId) {
	// Start at head
	Node* currNode = head;

	// Iterate through all nodes until bid id is found;
	while(currNode != nullptr) {
		if (currNode->bid.bidId.compare(bidId) == 0) {
			return currNode->bid;
		}
		currNode = currNode->next;
	}

	// Create and return null bid if bidId is not found.
	Bid nullBid;
	return nullBid;

}

/**
 * Returns the current size (number of elements) in the list
 */
int LinkedList::Size() {
    return size;
}

//============================================================================
// Static methods used for testing
//============================================================================

/**
 * Display the bid information
 *
 * @param bid struct containing the bid info
 */
void displayBid(Bid bid) {
    cout << bid.bidId << ": " << bid.title << " | " << bid.amount
         << " | " << bid.fund << endl;
    return;
}

/**
 * Prompt user for bid information
 *
 * @return Bid struct containing the bid info
 */
Bid getBid() {
    Bid bid;

    cout << "Enter Id: ";
    cin.ignore();
    getline(cin, bid.bidId);

    cout << "Enter title: ";
    getline(cin, bid.title);

    cout << "Enter fund: ";
    cin >> bid.fund;

    cout << "Enter amount: ";
    cin.ignore();
    string strAmount;
    getline(cin, strAmount);
    bid.amount = strToDouble(strAmount, '$');

    return bid;
}

/**
 * Load a CSV file containing bids into a LinkedList
 *
 * @return a LinkedList containing all the bids read
 */
void loadBids(string csvPath, LinkedList *list) {
    cout << "Loading CSV file " << csvPath << endl;

    // initialize the CSV Parser
    csv::Parser file = csv::Parser(csvPath);

    try {
        // Loop to read rows of a CSV file
    	// Changed int i to unsigned int i to remove warning.
        for (unsigned int i = 0; i < file.rowCount(); i++) {

            // initialize a bid using data from current row (i)
            Bid bid;
            bid.bidId = file[i][1];
            bid.title = file[i][0];
            bid.fund = file[i][8];
            bid.amount = strToDouble(file[i][4], '$');

            //cout << bid.bidId << ": " << bid.title << " | " << bid.fund << " | " << bid.amount << endl;

            // add this bid to the end
            list->Append(bid);
        }
    } catch (csv::Error &e) {
        std::cerr << e.what() << std::endl;
    }
}

/**
 * Simple C function to convert a string to a double
 * after stripping out unwanted char
 *
 * credit: http://stackoverflow.com/a/24875936
 *
 * @param ch The character to strip out
 */
double strToDouble(string str, char ch) {
    str.erase(remove(str.begin(), str.end(), ch), str.end());
    return atof(str.c_str());
}

/**
 * The one and only main() method
 *
 * @param arg[1] path to CSV file to load from (optional)
 * @param arg[2] the bid Id to use when searching the list (optional)
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
        csvPath = "eBid_Monthly_Sales_Dec_2016.csv";
        bidKey = "98109";
    }

    clock_t ticks;

    LinkedList bidList;

    Bid bid;

    int choice = 0;
    while (choice != 9) {
        cout << "Menu:" << endl;
        cout << "  1. Enter a Bid" << endl;
        cout << "  2. Load Bids" << endl;
        cout << "  3. Display All Bids" << endl;
        cout << "  4. Find Bid" << endl;
        cout << "  5. Remove Bid" << endl;
        cout << "  6. Prepend Bid" << endl;
        cout << "  9. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            bid = getBid();
            bidList.Append(bid);
            displayBid(bid);

            break;

        case 2:
            ticks = clock();

            loadBids(csvPath, &bidList);

            cout << bidList.Size() << " bids read" << endl;

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks
            cout << "time: " << ticks << " milliseconds" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 3:
            bidList.PrintList();

            break;

        case 4:
            ticks = clock();
            cout << "Enter bid key:" << endl;
            cin >> bidKey;
            bid = bidList.Search(bidKey);

            ticks = clock() - ticks; // current clock ticks minus starting clock ticks

            if (!bid.bidId.empty()) {
                displayBid(bid);
            } else {
            	cout << "Bid Id " << bidKey << " not found." << endl;
            }

            cout << "time: " << ticks << " clock ticks" << endl;
            cout << "time: " << ticks * 1.0 / CLOCKS_PER_SEC << " seconds" << endl;

            break;

        case 5:
        	cout << "Enter bid key to remove:" << endl;
        	cin >> bidKey;
            bidList.Remove(bidKey);

            break;

        // Case added to test Prepend() method.
        case 6:
        	bid = getBid();
        	bidList.Prepend(bid);
        	displayBid(bid);

        	break;
        }
    }

    cout << "Good bye." << endl;

    return 0;
}
