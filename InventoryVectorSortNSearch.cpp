// Source(s) of logic assistance:  
// Gaddis, T. (2018). Starting out with C++:
// From control structures through objects. Pearson. 

#include <iostream> // To use cin/cout
#include <iomanip>  // To access setw, setprecision for output width
#include <string>   // To implement to_string, getline functions
#include <fstream>  // To utilize ifstream, ofstream, fstream
#include <sstream>  // To use ostringstream to convert double to string
#include <vector>   // To use vector (dynamically sized form of array)
using namespace std;

// Global Constant Initialization
const string TITLE = "Inventory Part Manager Sort/Search Program";
const string AUTHOR_LINE = "By Forrest Moulin";

// Global input file stream accessible by all functions
ifstream fin;
// Global output file stream to save inventory received
ofstream fout;

// Declare struct for part object
struct Part
{	// part structure variables
	string number;
	char category;
	int quantity;
	double unitCost;
};

// Function prototypes (identifiers) to notify compiler
bool getData(vector<Part>& inventory);
void insertionSort(vector<Part>& inventory);
string getSearchTerm();
int binarySearch(string searchTerm, const vector<Part>& inventory);
bool buildPart(string createdPartNumber, char& createdPartClass,
	int& createdQuantity, double& createdUnitCost);
void insertPart(vector<Part>& inventory, Part invPart);
void printSearchStats(int numSearches, int goodSearches, int badSearches);
void writeDataToFile(vector<Part> inventory);
string partToLineStr(Part p);
string partDetailToStr(Part p);

// Template to swap objects of any matching data type
template <class FlexType>
void swapObjects(FlexType& a, FlexType& b)
{	// Create temporary object of the same type as a/b
	FlexType temp;
	temp = a;
	a = b;
	b = temp;
}

int main()
{
	// Dynamic variable initialization
	// Windows files directory
	string fileDirectory = "C:\\Users\\UserName\\Path\\To\\Directory\\";
	// File names appended to directory
	string inputFilePath = fileDirectory + "InventoryParts.txt";
	string outputFilePath = fileDirectory + "UpdatedInventoryParts.txt";

	string tempNumber;
	char tempCategory;
	int tempQuantity, numSearches = 0, 
		goodSearches = 0, badSearches = 0, vectorIndex;
	double tempUnitCost;

	Part tempPart;

	// Create inventory object, a vector of Part structs
	vector<Part> inventory;
	bool keepLooping = true, doneSearching = false;
	char keepLoopChar = ' ';
	bool dataRead;

	// Open both input & output file streams w/ file paths
	fin.open(inputFilePath);
	fout.open(outputFilePath);
	// End program in event of file read/write failures
	if (fin.fail())
	{
		cout << "File open failure:" << endl
			<< inputFilePath << endl;
		fin.close();
		fout.close();
		return 0;
	}
	if (fout.fail())
	{
		cout << "File output failure:" << endl
			<< outputFilePath << endl;
		fout.close();
		fin.close();
		return 0;
	}
	cout << fixed << setprecision(2) << TITLE << endl
		<< AUTHOR_LINE << endl << endl;

	fout << fixed << setprecision(2) << TITLE << endl
		<< AUTHOR_LINE << endl << endl;

	cout << "Reading InventoryPart.txt file..." << endl << endl;

	// Read data from txt file to vector of part structs
	dataRead = getData(inventory);
	cout << endl << "InventoryParts.txt read successfully!" << endl << endl;
	// Sort the vector by part name
	insertionSort(inventory);

	// Keep looping until bool changed to false/program exit
	while (keepLooping)
	{	// Gets part number after prompting user
		tempNumber = getSearchTerm();
		// Track search tally
		numSearches++;
		// Set vector index according to binary search result
		vectorIndex = binarySearch(tempNumber, inventory);
		// If string exists in part number subvector, 
		// index will not be -1
		if (vectorIndex != -1)
		{
			cout << partDetailToStr(inventory[vectorIndex])
				<< endl << endl;
			// Add to good search tally
			goodSearches++;
		}
		// Otherwise, index is -1 (not found)
		else
		{	// Part number already provided by user as search term
			tempPart.number = tempNumber;
			// Prompt user to complete the rest of the part details
			if (buildPart(tempPart.number, tempPart.category,
				tempPart.quantity, tempPart.unitCost) == true)
			{	// Only insert part if object is fully built
				insertPart(inventory, tempPart);
			}
			// Add to bad search tally (not found initially)
			badSearches++;
		}

		while (doneSearching == false)
		{
			cout << "Would you like to search for another part?" << endl
				<< "(Y for yes, N for no)" << endl << endl;

			cin >> keepLoopChar;
			if (keepLoopChar == 'n' || keepLoopChar == 'N')
			{
				cout << endl;
				doneSearching = true;
				keepLooping = false;
			}
			else if (keepLoopChar == 'y' || keepLoopChar == 'Y')
			{   // Reset value
				/*keepLoopChar = ' ';*/
				// Break inner loop to return to beginning of outer loop
				cout << endl;
				break;
			}
			else
			{
				cout << endl << "Invalid entry." << endl;
			}
			cin.ignore(1000, '\n');
		}
	}
	// Use inventory vector to write updated txt file
	writeDataToFile(inventory);	
	// Print search stats to console
	printSearchStats(numSearches, goodSearches, badSearches);

	cout << "Thanks for using Forrest's Greener Inventory Programs"
		<< endl << "Until next time! :)" << endl;
	// Exit program by returning 0 to main function
	return 0;
}
// Reads data from txt file into inventory vector of part structs
bool getData(vector<Part>& inventory)
{	// Temp variables to read parts from each line
	string readPartNumber;
	char readPartCat;
	int readQuantity;
	double readUnitCost;
	Part readPart;
	// While file input stream object contains the 4 variables on each line
	while (fin >> readPartNumber >> readPartCat >> readQuantity >> readUnitCost)
	{	// Build the part struct with data read from each line
		readPart.number = readPartNumber;
		readPart.category = readPartCat;
		readPart.quantity = readQuantity;
		readPart.unitCost = readUnitCost;
		// Add each part to back of inventory vector
		inventory.push_back(readPart);
		// Print formatted data to console for visibility of system status
		cout << left << setw(9) << readPartNumber
			<< left << setw(3) << readPartCat
			<< right << setw(3) << readQuantity
			<< right << setw(8) << readUnitCost << endl;
	}
	// Close file once complete
	fin.close();
	return true;
}
// Insertion sort algorithm to sort vector of parts by part number
void insertionSort(vector<Part>& inventory)
{
	cout << "Sorting vector via insertion sort algorithm..."
		<< endl << endl;
	// Inserts unsorted elements into sub array in outter loop
	for (int i = 0; i < inventory.size() - 1; i++)

	{	// Size sub array row incrementally in inner loop
		for (int j = i + 1; j > 0; j--)
		{	// Compares part numbers & swaps parts if order incorrect
			if (inventory[j].number < inventory[j - 1].number)
			{	// Call swap objects function
				swapObjects(inventory[j], inventory[j - 1]);
			}
		}
	}

	for (int i = 0; i < inventory.size(); i++)
	{
		cout << partToLineStr(inventory[i]) << endl;
	}
	cout << endl << "Insertion sort completed!" << endl << endl;
}
// Prompts user for part number str, returns str to calling function
string getSearchTerm()
{
	string searchTerm;
	cout << "Please enter the part number to search for"
		<< endl << "(Example: P-10000)" << endl << endl;
	cin >> searchTerm;
	searchTerm[0] = toupper(searchTerm[0]);
	cout << endl;
	return searchTerm;
}
// Binary searches vector w/ a string, returns subscript/position index
int binarySearch(string searchTerm, const vector<Part>& inventory)
{
	int startingIndex = 0, middleIndex, 
		lastIndex = inventory.size() - 1, position = -1;
	bool foundValue = false;
	while (foundValue == false && startingIndex <= lastIndex)
	{
		middleIndex = (startingIndex + lastIndex) / 2;
		// Searched val equals midpoint val
		if (inventory[middleIndex].number == searchTerm)
		{
			foundValue = true;
			position = middleIndex;
		}
		else if (inventory[middleIndex].number > searchTerm)
		{
			lastIndex = middleIndex - 1;
		}
		else
		{
			startingIndex = middleIndex + 1;
		}
	}
	if (position == -1)
	{
		cout << searchTerm << " was not found in the inventory."
			<< endl;
	}
	else
	{
		cout << searchTerm << " was found in the inventory vector"
			<< " at position " << position + 1 << "."
			<< endl << endl;
	}
	return position;
}
// Builds a part object with part variables
bool buildPart(string createdPartNumber, char& createdPartCat,
	int& createdQuantity, double& createdUnitCost)
{
	char addPart = ' ';
	while (addPart == ' ')
	{
		cout << "Would you like to add this part?" << endl
				<< "(Y for yes, N for no)" << endl << endl;
		cin >> addPart;
		cout << endl;

		if (addPart == 'y' || addPart == 'Y')
		{	// Build the rest of part object by prompting user entry
			cout << "Please enter a character for the part class" << endl
				<< "(Character: A-F)" << endl << endl;

			cin >> createdPartCat;

			string partCatStr = "";
			partCatStr += createdPartCat;
			createdPartCat = toupper(partCatStr[0]);

			cout << endl << "Please enter a positive integer for the part quantity"
				<< endl << "(Example: 100)" << endl << endl;

			cin >> createdQuantity;

			cout << endl << "Please enter a positive double for the part's unit cost"
				<< endl << "(Example: 22.50)" << endl << endl;

			cin >> createdUnitCost;

			// Create copy of part and convert to string
			Part createdpartCopy;
			createdpartCopy.number = createdPartNumber;
			createdpartCopy.category = createdPartCat;
			createdpartCopy.quantity = createdQuantity;
			createdpartCopy.unitCost = createdUnitCost;

			cout << endl << "part successfully added to inventory!"
				<< endl << endl << partDetailToStr(createdpartCopy) 
				<< endl << endl;
			break;
		}
		else if (addPart == 'n' || addPart == 'N')
		{	// Return false to break loop and exit function
			return false;
		}
		else
		{
			cout << "Invalid entry.";
			// Return to beginning of loop
			addPart = ' ';
		}
		cin.ignore(1000, '\n');
	}
	return true;
}
void insertPart(vector<Part>& inventory, Part invPart)
{	// Push the new part to the back of the vector
	inventory.push_back(invPart);
}
// Prints search results overview to console
void printSearchStats(int numSearches, int goodSearches, int badSearches)
{
	cout << endl << "Search Overview  -  Current Session" << endl
		<< "Total searches: " << numSearches << endl
		<< "Successful searches: " << goodSearches << endl
		<< "Unsuccessful searches (not found): " << badSearches
		<< endl << endl;
}
// Writes data to file and mirrors output in console
void writeDataToFile(vector<Part> inventory)
{
	for (int i = 0; i < inventory.size(); i++)
	{	// Console visibility
		cout << inventory[i].number
			<< right << setw(3) << inventory[i].category 
			<< right << setw(5) << inventory[i].quantity
			<< right << setw(8) << inventory[i].unitCost << endl;
		// Output file mirrors console output
		fout << inventory[i].number
			<< right << setw(3) <<inventory[i].category
			<< right << setw(5) << inventory[i].quantity
			<< right << setw(8) << inventory[i].unitCost << endl;
	}
	cout << endl << "Data successfully written to "
		<< "UpdatedInventoryParts.txt file!" << endl;
	fout.close();
}
// Returns part as str on one line
string partToLineStr(Part p)
{
	// Read the double to string using ostringstream
	// for formatting/building the string
	ostringstream strStream;

	strStream << left << setw(9) << p.number
		<< left << setw(3) << p.category
		<< right << setw(3) << p.quantity
		<< right << setw(8) << p.unitCost;
	// Print string to console
	/*cout << strStream.str();*/
	strStream.clear();
	// Return the string to calling function from string stream object
	return strStream.str();
}
// Returns detailed part overview as str
string partDetailToStr(Part p)
{
	// Read the double to string using ostringstream
	// for formatting/building the string
	ostringstream strStream;
	double valuation = p.unitCost * p.quantity;
	// Concatenate the string stream object string
	strStream << "part Overview" << endl
		<< left << setw(19) << "Part number:"
		<< right << setw(8) << p.number << endl
		<< left << setw(26) << "Part class:"
		<< p.category << endl
		<< left << setw(22) << "Quantity:"
		<< right << setw(5) << p.quantity << endl
		<< left << setw(17) << "Unit cost:" << "$"
		<< setprecision(5) << right << setw(9) << p.unitCost
			<< endl
		<< left << setw(17) << "Total valuation:" << "$"
		<< setprecision(10) << right << setw(9) << 1.00 * valuation;
	strStream.clear();
	// Return the string to calling function from string stream object
	return strStream.str();
}
/*
* CONSOLE OUTPUT SAMPLE
* Inventory Part Manager Sort/Search Program
* By Forrest Moulin
*
* Reading InventoryPart.txt file...
*
* P-39457  A   16  102.14
* P-11702  B   21   24.74
* P-11754  B   27   15.23
* ...
* P-43378  A   46    6.38
* P-43622  A   10  155.36
* P-43672  A   38   17.97
* 
* InventoryParts.txt read successfully!
* 
* Sorting vector via insertion sort algorithm...
* 
* P-10659  B   17   88.97
* P-11191  A   10   34.29
* P-11196  B   39   59.04
* ...
* P-43582  B    3   13.18
* P-43622  A   10  155.36
* P-43672  A   38   17.97
*
* Insertion sort completed!
* 
* Please enter the part number to search for
* (Example: P-10000)
*
* p-43672
*
* P-43672 was found in the inventory vector at position 304.
*
* Part Overview
* Part number:        P-43672
* Part class:               A
* Quantity:                38
* Unit cost:       $    17.97
* Total valuation: $   682.86
*
* Would you like to search for another part?
* (Y for yes, N for no)
*
* y
* 
* Please enter the part number to search for
* (Example: P-10000)
*
* p-10022
*
* P-10022 was not found in the inventory.
* Would you like to add this part?
* (Y for yes, N for no)
*
* y
*
* Please enter a character for the part class
* (Character: A-F)
*
* c
*
* Please enter a positive integer for the part quantity
* (Example: 100)
*
* 123
*
* Please enter a positive double for the part's unit cost
* (Example: 22.50)
*
* 45.00
*
* Part successfully added to inventory!
*
* Part Overview
* Part number:        P-10022
* Part class:               C
* Quantity:               123
* Unit cost:       $       45
* Total valuation: $     5535
*
* Would you like to search for another part?
* (Y for yes, N for no)
*
* n
* ...
* P-43672  A   38   17.97
* P-10022  C  123   45.00
* P-31977  A   12  156.00
* P-32344  D   88   12.00
*
* Data successfully written to UpdatedInventoryParts.txt file!
*
* Search Overview  -  Current Session
* Total searches: 6
* Successful searches: 3
* Unsuccessful searches (not found): 3
*
* Thanks for using Forrest's Greener Inventory Programs
* Until next time! :)
*/
