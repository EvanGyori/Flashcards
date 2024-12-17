/* flashcard 1.0 - by Evan Gyori 8/29/22
* 
* A simple terminal program to help memorizing through
* flashcards, can edit, shuffle, view cards and flip them to
* see the awnsers.
*
* begin by entering: flashcard <filename> to open or create a file,
* then type next to begin studying or use add to add a card.
*
* Documentation:
* Command line flags:
* 	-h or --help	prints documentation
* 
* Runtime Commands:
* 	h or help       prints documentation
* 	q or quit       exit program
* 	shuffle         randomizes order of cards (done at start)
* 	n or next       view the next card, must be called to begin flashcards
* 	f or flip       flips the current card to show the awnser
* 	list #1 #2      lists out cards and awnsers beginning at #1 and by amount #2
* 	add             begin creating a new card
* 	edit #          begin editing card with id #
*
*
* File format: front#back#nextFront#nextBack#...
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>

#define FRONT 0x01
#define BACK 0x02

void printDocumentation()
{
	std::cout << "flashcard 1.0\n\n"
	<< "begin by entering: flashcard <file> to open or create a file,\n"
	<< "then type next to begin studying or use add to add a card.\n\n"
	<< "Command line flags:\n"
 	<< "\t-h or --help	prints documentation\n"
	<< '\n'
	<< "Runtime Commands:\n"
	<< "\th or help       prints documentation\n\n"
	<< "\tq or quit       exit program\n\n"
	<< "\tshuffle         randomizes order of cards (done at start)\n\n"
	<< "\tn or next       view the next card, must be called to begin flashcards\n\n"
	<< "\tf or flip       flips the current card to show the awnser\n\n"
	<< "\tlist #1 #2      lists out cards and awnsers beginning at #1 and by amount #2\n\n"
	<< "\tadd             begin creating a new card\n\n"
	<< "\tedit #          begin editing card with id #\n";
}

struct Card
{
	std::string front;
	std::string back;
	
	// Enter FRONT or/and BACK
	void print(int id, int sides)
	{
		std::cout << "Card #" << id << '\n';
		std::cout << "--------------------\n";
		if (sides & FRONT) {
			std::cout << front << '\n';
		}
		
		if (sides & FRONT && sides & BACK) {
			std::cout << "----------\n";
		}
		
		if (sides & BACK) {
			std::cout << back << '\n';
		}
		std::cout << "--------------------\n\n";
	}
};

class Program
{
public:
	Program(int argc, const char** argv)
	{
		if (argc > 1) {
			if (!processLineArgument(*(argv + 1)))
				return;
		} else {
			std::cout << "no filename found\nadd flag -h or --help for documentation\n";
			return;
		}
		
		srand(time(NULL));
		
		openFile();
		readCards();
	}
	
	~Program()
	{
		if (file.is_open())
			file.close();
	}
	
	void run()
	{
		if (!file.is_open())
			return;
			
		std::string input;
		do {
			std::cout << "(flashcard " << currentCard+1 << "/" << cards.size() << ") ";
			std::cin >> input;
			
			if (input == "h" || input == "help") {
				printDocumentation();
			}
			else if (input == "shuffle") {
				shuffle();
			}
			else if (input == "n" || input == "next") {
				currentSide = FRONT;
				currentCard = nextCard;
				cards.at(order.at(nextCard)).print(order.at(nextCard), FRONT);
				
				//increment and wrap nextCard
				nextCard++;
				if (nextCard >= order.size()) {
					std::cout << "All cards have been viewed\nStarting from beginning\n";
					nextCard = 0;
				}
			}
			else if (input == "f" || input == "flip") {
				if (currentCard == -1) {
					std::cout << "no card being viewed\n";
				} else {
					currentSide = (currentSide == FRONT) ? BACK : FRONT;
					cards.at(order.at(currentCard)).print(order.at(currentCard), currentSide);
				}
			}
			
		} while (input != "q" && input != "quit");
	}
	
private:
	std::fstream file;
	std::string filename;
	
	std::vector<Card> cards;
	std::vector<size_t> order;
	size_t currentCard;
	size_t nextCard;
	int currentSide;
	
	// Returns true if program should continue
	// determines if the input is a filename or flag
	bool processLineArgument(const char* arg)
	{
		if (std::strcmp(arg, "-h") == 0 
			|| std::strcmp(arg, "--help") == 0) {
			printDocumentation();
			return false;
		}
		else {
			filename = arg;
			return true;
		}
	}
	
	// Returns true if program should continue
	// attempts to open a file, if no file exists, create one
	void openFile()
	{
		// attempt to open file, if no file exists, create one
		file.open(filename);
		if (!file.is_open()) {
			std::cout << "file : " << filename << " doesn't exits\n"
			<< "would you like to create one? (y/n) ";
			
			char input;
			std::cin >> input;
			if (input == 'y') {
				std::ofstream openFile(filename);
				openFile.close();
				file.open(filename);
				if (!file.is_open()) {
					std::cout << "failed to create file\n";
				}
			}
		}
	}
	
	void readCards()
	{
		currentSide = FRONT;
		nextCard = 0;
		currentCard = -1;
		while (!file.eof()) {
			Card card;
			std::getline(file, card.front, '#');
			std::getline(file, card.back, '#');
			
			order.push_back(cards.size());
			cards.push_back(card);
		}
		
		cards.pop_back();
		order.pop_back();
	}
	
	void shuffle()
	{
		nextCard = 0;
		std::vector<size_t> indices = order;
		for (size_t& id : order) {
			size_t pos = rand() % indices.size();
			id = indices.at(pos);
			indices.erase(indices.begin() + pos);
		}
	}
};

int main(int argc, const char** argv)
{
	Program program(argc, argv);
	program.run();
	
	return 0;
}




