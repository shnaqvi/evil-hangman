/* NAME: Evil Hangman
 * AUTHOR: Salman Naqvi
 * DATE: May 10th, 2013
 * 
 * DESCRIPTION: It plays the Hangman game, while constantly cheating the user
 * by changing his initial choice of word 
 */

/* include all the libraries */
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <set>
#include <map>
#include <algorithm>
#include <typeinfo>
using namespace std;

struct WordFamily {
	int freq;
	set<string> family;
};

typedef map< string, WordFamily>  wordFamilyMap;

/* Function Prototypes */
void populateYesNoSets( set<string>& boolYes, set<string>& boolNo );
int getNonNegInteger( string& prompt);
bool getChoice( string& prompt, set<string> boolYes, set<string> boolNo);
char getAlphabet( string prompt); 
bool isLetterUsed( char nextCh, set<char> lettersUsed );

void readDictionary( ifstream& infile, set<string>& english);
void trimDictionary( size_t wordLen, set<string>& master, set<string>& english,
		string prompt);

void formWordFamilies( char nextCh, set<string> masterList, wordFamilyMap& wordFamilies);
bool mapCompare(const pair<string, WordFamily >& lhs, 
		const pair<string, WordFamily >& rhs);

string badGuessString( int wordLen );
int updateWord( string wordCode, string badGuess, char nextCh, vector<char>& word);

void printVector( vector<char> word ); 
void printSet( set<char> setOfChar );
void printSetOfStrings( set<string>& setOfString ); 
void printWordFamilyOthers( vector<char> word, int noGuesses, bool showWords,
		set<char> lettersUsed, set<string>& masterList );
/* Template/Generic Function to Print the contents of a set of Chars or Strings 
template <typename containerType>
void printSet( containerType const& mySet )
{
  typename containerType::const_iterator itr;
  typename containerType::const_iterator end( mySet.end() );
  for ( itr = mySet.begin(); itr != end; ++itr ){
    cout << *itr << ", ";
  }
  cout << "\b\b " << endl;
} */


/* Main Function */
int main() {

	// Load Dictionary to Set
	ifstream infile;
	infile.open("dictionary.txt");

	if( !infile.is_open() ) {
		cout << "Unable to Load File" << endl;
		return 0;
	}

	set<string> english;
	readDictionary( infile, english);

	// Keep Playing until Stopped
	bool reTry = 1;

	while( reTry ) {

		// Initializing Sets used to check for correct Yes/No Answer
		set<string> boolYes, boolNo;
		populateYesNoSets( boolYes, boolNo);
		//    for (set<string>::iterator itr = boolYes.begin(); itr != boolYes.end(); ++itr)
		//    	cout << *itr << endl;

		// Prompt for the Number of Guesses
		string prompt = "Enter the Number of Guesses Desired:  ";
		int noGuesses = getNonNegInteger( prompt);

		if (!noGuesses) {
			prompt = "No Guesses, No Game! Would You Like To Play Again (Y/N)? ";
			reTry = getChoice( prompt, boolYes, boolNo);

			if (reTry)
				continue;
			else
				break;
		}

		// Prompt whether User wants to see Remaining Word List
		prompt = "Do you want to see the remaining words list (Y/N)?  ";
		bool showWords = getChoice( prompt, boolYes, boolNo);

		// Prompt User for the Word Length
		prompt = "Enter the Word Length for the Hangman: ";
		size_t wordLen = getNonNegInteger( prompt);
		cout << endl;

		// string of all 0's indicating incorrect guess
		string badGuess = badGuessString( wordLen );

		// Trim the Dictionary to Match the Word Length &
		// Reprompt if no word exists of the desired length
		set<string> masterList;
		trimDictionary( wordLen, masterList, english, prompt);

		// PLAY HANGMAN:
		// Initialize vector of chars containing dashes to be replaced with letters
		vector<char> word(wordLen, '-');

		// Initialize vector of All Letter Used
		set<char> lettersUsed;

		// Print Word
		printVector( word);

		// Repeat until word Length is Reached or the no. of Guesses Expire
		while( wordLen > 0 && noGuesses > 0 ) {

			// Prompt for Next Guess and Update Results
			prompt = "Enter Your Next Guess: ";
			char nextCh = getAlphabet( prompt);

			// Check if Letter is Not Used Already
			bool isBadCh = isLetterUsed( nextCh, lettersUsed );
			if (isBadCh) {
				cout << "Letter Used Already, Try Again!" << endl;
				continue;
			} else
				lettersUsed.insert( nextCh );

			// word families master map updated everytime user enters a guess
			wordFamilyMap wordFamilies;

			// Form a Map of All Word Families
			formWordFamilies( nextCh, masterList, wordFamilies);
			//*debug* cout << "Map Size: " << wordFamilies.size() << endl;

			// Check which Word Family has the highest frequency
			pair<string, WordFamily> maxWordFamily = *max_element( wordFamilies.begin(),
					wordFamilies.end(), mapCompare);

			// Change the contents of the master list to the current highest freq. family
			masterList.clear();
			masterList = maxWordFamily.second.family;
			//*debug* cout << "Max Freq: " << maxWordFamily.second.freq << endl;

			// Change the "word" vector accordingly
			string wordCode = maxWordFamily.first;
			int lettersGuessed = updateWord( wordCode, badGuess, nextCh,  word);

			// Update wordLen & noGuesses
			if( lettersGuessed == 0 )
				noGuesses--;
			else
				wordLen -= lettersGuessed;

			// Print Word
			printWordFamilyOthers( word, noGuesses, showWords, lettersUsed, masterList );

			/* Procedure: */
			// 1. Get Input, Ensure its a single Alphabet,
			// 2. Make Word Families, Store in a Map,
			// 3. Pick the Word Family with Highest Count,
			// 4. Use the position of Character in that family to update "word" vector,
			// 5. Display "word" vector, Prompt for the Next Character,
			// 6. If no. of guesses or Word Length expires, Output the Result and
			// 7. State You Lose, Prompt whether User wants to try again!

			/* Side Note: (pair< > is a part of standard library and is used for
			 * maps b/c maps store information in the form of pairs, key-value pair) */

		}

		// State the Result: Win or Lose
		if ( noGuesses == 0 ) {
			cout << "Better Luck Next Time bro, but Loser is a Loser!" << endl;
			cout << "The Word was: " << *masterList.begin() << "\n" << endl;
		} else
			cout << "You' the Killer Mate! Beat the Computer, eh! \n" << endl;

		// Prompt for Another Try
		prompt = "Would you like to play again (Y/N)? ";
		reTry = getChoice( prompt, boolYes, boolNo);
		cout << endl;
	}

	return 0;
}

/* Outputs the result after Each Turn: Word, No. of Guesses, Letters Used */
void printWordFamilyOthers( vector<char> word, int noGuesses, bool showWords,
		set<char> lettersUsed, set<string>& masterList )
{
	printVector( word);
	cout << "Wrong Guesses Left: " << noGuesses << endl;
	cout << "Letters Used: " ;
	printSet( lettersUsed );
	cout << endl;

	if ( showWords ) {
		cout << "Master List Length: " << masterList.size() << endl;
		cout << "Words contained: ";
		printSetOfStrings( masterList);
		cout << endl;
	}
}

/* Update the Blank Word with the Guessed Letter */
int updateWord( string wordCode, string badGuess, char nextCh, vector<char>& word)
{
	if ( wordCode == badGuess )
		return 0;

	int count = 0;
	for (int i = 0; i < wordCode.length(); i++){
		if ( wordCode[i] == '1') {
			word[i] = nextCh;
			count++;
		}
	}
	return count;
}

/* Forms the Map of word Families with their Frequencies */
void formWordFamilies( char nextCh, set<string> masterList, wordFamilyMap& wordFamilies)
{ 
	// Loop over each word in the masterList
	string tempWord;
	for (set<string>::iterator itr = masterList.begin(); itr != masterList.end(); ++itr) {
		tempWord = *itr;
		//*debug* cout << tempWord << endl;

		// Replace word with the position locator of nextCh
		for (int i = 0; i < tempWord.length(); i++) {
			if (tempWord[i] == nextCh)
				tempWord[i] = '1';
			else tempWord[i] = '0';
		}
		//*debug* cout << tempWord << endl;

		// Add the word to some Family in the Map and increment Family's frequency
		if ( wordFamilies.find(tempWord) == wordFamilies.end() ) {
			// Object of the Struct WordFamily containing a family & its frequency
			WordFamily wordFamily;

			wordFamily.freq = 1;
			wordFamily.family.insert( *itr );
			wordFamilies.insert( pair<string, WordFamily> (tempWord, wordFamily) );
		} else {
			wordFamilies[tempWord].freq++;
			wordFamilies[tempWord].family.insert( *itr);
		}
	}

	/*debug:* Print out Freq. of All Families
  for ( map<string, WordFamily>::iterator itr = wordFamilies.begin();
      itr != wordFamilies.end(); ++itr) {
    cout << "Freq is " << (*itr).second.freq << endl;
    cout << "Family is ";
    printSetOfStrings( (*itr).second.family);
    cout << endl;
  } */

}

/* Comparator Function to be Used with max_element for set */
bool mapCompare(const pair<string, WordFamily >& lhs, 
		const pair<string, WordFamily >& rhs)
{
	return lhs.second.freq < rhs.second.freq;
}

/* Prints the contents of a set of Strings */
void printSetOfStrings( set<string>& setOfString ) 
{
	for ( set<string>::iterator itr = setOfString.begin(); itr != setOfString.end(); ++itr ){
		cout << *itr << ", ";
	}
	cout << "\b\b " << endl;
}

/* Prints the contents of a set of chars */
void printSet( set<char> setOfChar )
{
	for ( set<char>::iterator itr = setOfChar.begin(); itr != setOfChar.end(); ++itr ){
		cout << *itr << ", ";
	}
	cout << "\b\b " << endl;
}

/* Prints a Vector on Screen */
void printVector( vector<char> word) 
{
	for (size_t i = 0; i < word.size(); i++) {
		cout << word[i];
	}
	cout << '\n' << endl;
}

/* Checks whether the Letter Entered by User was Used Already */
bool isLetterUsed( char nextCh, set<char> lettersUsed )
{
	if ( lettersUsed.find(nextCh) == lettersUsed.end() ) return 0;
	else return 1;
}

/* Trims the Dictionary to Keep only the specific Word Length Words */
void trimDictionary( size_t wordLen, set<string>& masterList, set<string>& english,
		string prompt)
{
	while( masterList.empty() ) {
		for( set<string>::iterator itr = english.begin(); itr != english.end(); ++itr) {
			if ( (*itr).length() == wordLen ) {   // must include paran.s around #itr b/c . has higher precendence than *
				masterList.insert(*itr);
				//cout << *itr << endl;
			}
		}

		if( masterList.empty()) {
			cout << "No word that long in the Dictionary, Try Again!" << endl;
			wordLen = getNonNegInteger( prompt);
		}
	}
	return;
}

/* Generates a Word of All zeros to represent an Incorrect Guess */
string badGuessString( int wordLen )
{
	string zeros;
	for (int i = 0; i < wordLen; i++ ) {
		zeros += '0';
	}
	return zeros;
}

/* Read Dictionary Words into a set */
void readDictionary( ifstream& infile, set<string>& english)
{
	string data;
	while( getline(infile, data))
		english.insert( data);
}

/* Unnecessary Function to ensure all possible combinations of y/n are detected */
void populateYesNoSets( set<string>& boolYes, set<string>& boolNo) 
{
	string tempList[] =  {"y", "yes", "yah", "ya", "yup", "yae"};
	int tempListSize = sizeof tempList / sizeof(string);
	boolYes.insert( tempList, tempList+ tempListSize);

	// array cannot be reinitialized or resized, just copied into elementwise, so
	// either create another array or a better way is to use a vector BUT vector
	// cannot be initialized (unless using C++11) in a list way, so its useless here
	string tempList1[] = {"n", "no", "nah", "na", "nope", "nae"};
	tempListSize = sizeof tempList1 / sizeof(string);
	boolNo.insert( tempList1, tempList1+ tempListSize);
}

/* Prompt User for an Alphabet */
char getAlphabet( string prompt) 
{
	string data;

	while( true){
		cout << prompt;
		getline(cin, data);

		// Account for \r character attached to the end of standard input under Windows Platform
		if (data[data.size() - 1] == '\r') data.resize(data.size() - 1);

		if (data.length()>1 || !isalpha(data[0])) {
			cout << "Not a Single Alphabet, Try Again!" << endl;
		} else {
			cout<< endl;
			return data[0];
		}
	}
}

/* Prompt User for a y/n Answer */
bool getChoice( string& prompt, set<string> boolYes, set<string> boolNo ) 
{
	string data;
	while (true) {
		cout << prompt;
		getline( cin, data);
		// Account for \r character attached to the end of standard input under Windows Platform
		if (data[data.size() - 1] == '\r') data.resize(data.size() - 1);

		//cout << typeid(data).name() << endl; //find the type of input read by getline

		for (string::iterator itr = data.begin(); itr != data.end(); ++itr)
			*itr = tolower(*itr);

		if (boolYes.find(data) != boolYes.end()) //find if element is in set by matching its iterator position to the end position
			return 1;
		else if ( boolNo.find(data) != boolNo.end() )
			return 0;
		cout << "Incorrect Yes/No Answer, Try Again!" << endl;
	}
}

/* Prompt User for an Integer */
int getNonNegInteger( string& prompt) 
{
	string data; int number;
	while (true) {
		cout << prompt;

		getline( cin, data);
		istringstream sender(data);
		sender >> number;

		if (sender.fail() || number< 0)
			cout << "Not a Non-Negative Integer, Try Again!" << endl;
		else
			return number;
	}
}

/* HELPFUL RESOURCES USED */
// http://stackoverflow.com/questions/12769776/get-the-size-of-string-returned-by-iterator
// http://stackoverflow.com/questions/9393291/pointer-to-const-char-vs-char-array-vs-stdstring
// http://stackoverflow.com/questions/2281168/quick-vector-initialization-c


/* ISSUES FOUND AND RESOLVED WHEN USING ECLIPSE CDT UNDER WINDOWS TO EXECUTE LINUX .CPP FILE */
// cin reads input correctly, comparison happens correctly in general
// --> MAIN ISSUE: getline(cin, str) always reads atleast 1 chars from input under Windows, that being \r
// -> Eclipse Standard ouptut doesn't display \b character properly i.e. doesn't remove the previous character:
