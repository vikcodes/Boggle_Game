/*
 * File: Boggle.cpp
 * ----------------
 * Name: [TODO: Vikram Sreedhar]
 * Section: [TODO: Nick Dupoux]
 * This file is the main starter file for Assignment #4, Boggle.
 * [TODO: The program sets up the letter cubes, shakes them up, and
 * lay them out on the board. The human player gets to go first.
 * The player enters words one by one. After verifying that a word
 * is legitimate, the program highlights its letters on the board,
 * adds it to the player’s word list, and awards the player points according
 * to the word’s length. Once the player has found as many words as he or she
 * can, the computer takes a turn. The computer searches through the board
 * to find all the remaining words and awards itself points for those words.
 */
 
#include <iostream>
#include "gboggle.h"
#include "grid.h"
#include "gwindow.h"
#include "lexicon.h"
#include "random.h"
#include "simpio.h"
using namespace std;

/* Constants */

const int BOGGLE_WINDOW_WIDTH = 650;
const int BOGGLE_WINDOW_HEIGHT = 350;

const string STANDARD_CUBES[16]  = {
   "AAEEGN", "ABBJOO", "ACHOPS", "AFFKPS",
   "AOOTTW", "CIMOTU", "DEILRX", "DELRVY",
   "DISTTY", "EEGHNW", "EEINSU", "EHRTVW",
   "EIOSST", "ELRTTY", "HIMNQU", "HLNNRZ"
};
 
const string BIG_BOGGLE_CUBES[25]  = {
   "AAAFRS", "AAEEEE", "AAFIRS", "ADENNN", "AEEEEM",
   "AEEGMU", "AEGMNN", "AFIRSY", "BJKQXZ", "CCNSTW",
   "CEIILT", "CEILPT", "CEIPST", "DDLNOR", "DDHNOT",
   "DHHLOR", "DHLNOR", "EIIITT", "EMOTTT", "ENSSSU",
   "FIPRSY", "GORRVW", "HIPRRY", "NOOTUW", "OOOTTU"
};

const int STANDARD_ROW_NUMBER = 4;
const int BIG_ROW_NUMBER = 5;
const int MAX_TIME = 400;

/* Function prototypes */

void welcome();
void giveInstructions();
Grid<string> fillingOutBoard(bool big);
Grid<string> makePlayerBoard(bool big);
Grid<string> makeBoard(bool big);
Vector<string> swap(Vector<string> &vec, int i, int r);
Vector<string> playerTurn(Lexicon &english, Grid<string> &board, Vector<string> &playerWords );
void computersTurn(Lexicon &usedWords, Grid<string> &board, Lexicon &english, string word, int rowNum, int colNum);
bool findWordInGrid(string word, int rowNum, int colNum, Grid<string> &board, int numb, Vector<Point> points);
void highlightCubes(Vector<Point> points);
bool checkWordPreReqs(string word, Lexicon &english, Vector<string> &playerWords);
Lexicon convertVectorToLexicon(Vector<string> & theVec);
void recordWordForComputer(Lexicon &english, string word, Lexicon &usedWords);
void addAdjacentCubesToPossibleWord(Lexicon & usedWords, Grid<string> & board, Lexicon & english,
                                    string word, int rowNum, int colNum);
bool bigBoggle();

/* Main program */

int main() {
   Lexicon english("EnglishWords.dat");
   Vector<string> playerWords;
   Vector<string> computerWords;

   GWindow gw(BOGGLE_WINDOW_WIDTH, BOGGLE_WINDOW_HEIGHT);
   initGBoggle(gw);
   welcome();
   giveInstructions();
   bool big = bigBoggle();
   Grid<string> board = fillingOutBoard(big);
   playerWords = playerTurn(english, board, playerWords);
   Lexicon lex = convertVectorToLexicon(playerWords);
   for(int i = 0; i<board.numRows(); i++){
       for (int j = 0; j<board.numCols(); j++){
            computersTurn(lex, board, english, "", i, j);
       }
   }
   return 0;
}

/*
 * Function: welcome
 * Usage: welcome();
 * -----------------
 * Print out a cheery welcome message.
 */

void welcome() {
   cout << "Welcome!  You're about to play an intense game ";
   cout << "of mind-numbing Boggle.  The good news is that ";
   cout << "you might improve your vocabulary a bit.  The ";
   cout << "bad news is that you're probably going to lose ";
   cout << "miserably to this little dictionary-toting hunk ";
   cout << "of silicon.  If only YOU had a gig of RAM..." << endl << endl;
}

/*
 * Function: giveInstructions
 * Usage: giveInstructions();
 * --------------------------
 * Print out the instructions for the user.
 */

void giveInstructions() {
   cout << endl;
   cout << "The boggle board is a grid onto which ";
   cout << "I will randomly distribute cubes. These ";
   cout << "6-sided cubes have letters rather than ";
   cout << "numbers on the faces, creating a grid of ";
   cout << "letters on which you try to form words. ";
   cout << "You go first, entering all the words you can ";
   cout << "find that are formed by tracing adjoining ";
   cout << "letters. Two letters adjoin if they are next ";
   cout << "to each other horizontally, vertically, or ";
   cout << "diagonally. A letter can only be used once ";
   cout << "in each word. Words must be at least four ";
   cout << "letters long and can be counted only once. ";
   cout << "You score points based on word length: a ";
   cout << "4-letter word is worth 1 point, 5-letters ";
   cout << "earn 2 points, and so on. After your puny ";
   cout << "brain is exhausted, I, the supercomputer, ";
   cout << "will find all the remaining words and double ";
   cout << "or triple your paltry score." << endl << endl;
   cout << "Hit return when you're ready...";
   getLine();
   cout << "I'll give you a chance to set up the board to ";
   cout << "your specification, which makes it easier to ";
   cout << "confirm your boggle program is working. " << endl;
}

/* This function asks the user is they want to have a Boggle
 * board created for them by the computer or if they would
 * like to create their own Boggle board themselves. This function
 * repeats until the user has entered either yes or no.
 */
Grid<string> fillingOutBoard(bool big){
    string response = "";
    string answer = getLine("Do you want to force the board configuration?");
    for(int i = 0; i<answer.length(); i++){
        response += tolower(answer[i]);
    }
    if(response == "yes" || response == "no"){
        if(response == "yes"){
           return makePlayerBoard(big);
        } else {
            return makeBoard(big);
        }
    } else {
        cout << "Please answer YES or NO" << endl;
        return fillingOutBoard(big);
    }
}

/* This function asks the user if they want to use a 5x5 board instead
 * of a 4x4 board. If the answer is yes, then the function returns true. If
 * no then it returns false.
 */
bool bigBoggle(){
    string response = "";
    string answer = getLine("Would you like Big Boggle? ");
    for(int i = 0; i<answer.length(); i++){
        response += tolower(answer[i]);
    }
    if(response == "yes" || response == "no"){
        if(response == "yes"){
           return true;
        } else {
            return false;
        }
    } else {
        cout << "Please answer YES or NO" << endl;
        return bigBoggle();
    }
}

/* This function creates a Boggle board based on a user's
 * 16-character input. If the user's input isn't 16 characters,
 * the computer will prompt the user to enter a new 16 characters.
 * The board is drawn and each character is put into a cube and copied
 * into a grid as well.
 */
Grid<string> makePlayerBoard(bool big){
    Grid<string> grid (STANDARD_ROW_NUMBER,STANDARD_ROW_NUMBER);
    if(big){
    grid.resize(BIG_ROW_NUMBER,BIG_ROW_NUMBER);
    }
    int letterNumb = 0;
    cout << "Enter a 16-character string to identify ";
    cout << "which letters you want on the cubes.  The ";
    cout << "first 4 letters are the cubes on the top row ";
    cout << "from left to right, the next 4 letters are the ";
    cout << "second row, and so on. " << endl;
    string letters = getLine("Enter the string: ");
    while(letters.length() !=  (grid.numCols()*grid.numRows())){
        cout << "Enter "<< (grid.numCols()*grid.numRows()) << " characters. " << endl;
        letters = getLine("Enter the string: ");
    }

    drawBoard(grid.numCols(), grid.numRows());
    for(int l = 0; l<grid.numCols(); l++){
        for(int h = 0; h<grid.numCols(); h++){
            char ch =  letters[letterNumb];
            labelCube(l, h, toupper(ch));
            grid.set(l,h, charToString(tolower(ch)));
            letterNumb++;
        }
    }
return grid;
}

/* This method creates a Boggle board from a vector of strings.
 * It randomizes the vector of strings and then randomly picks
 * one character from each string to fill the board. A grid is
 * also created to contain all the characters from the Boggle board.
 */
Grid<string> makeBoard(bool big){
    int letterNumber = 0;
    Grid<string> grid (STANDARD_ROW_NUMBER,STANDARD_ROW_NUMBER);
    if(big){
    grid.resize(BIG_ROW_NUMBER,BIG_ROW_NUMBER);
    }
    drawBoard(grid.numCols(),grid.numRows());
    Vector<string> vec;

    for(int j = 0; j < (grid.numCols()*grid.numRows()); j++){
        if(big){
        vec.add(BIG_BOGGLE_CUBES[j]);
        } else {
            vec.add(STANDARD_CUBES[j]);
        }
    }

    for(int k = 0; k < vec.size()-1; k++){
        int r = randomInteger(k+1,vec.size() - 1);
        vec = swap(vec,k,r);
    }

    for(int l = 0; l<grid.numCols(); l++){
        for(int h = 0; h<grid.numCols(); h++){
            string str = vec[letterNumber];
            char ch = str[randomInteger(0,5)];
            labelCube(l, h, ch);
            grid.set(l,h, charToString(tolower(ch)));
            letterNumber++;
    }
    }

    cout << "Ok, take all the time you want and find all the ";
    cout << "words you can!  Signal that you're finished by entering an empty line." << endl;
    return grid;
}

/* This method swap two elements within a given vector. */
Vector<string> swap(Vector<string> &vec, int i, int r){
    string temp = vec[i];
    vec[i] = vec[r];
    vec[r] = temp;
    return vec;
}

/* This function allows the user to guess a word on the Boggle board.
 * The base case is if the user hits RETURN when asked to enter a word,
 * this function will return all the words the user guessed correctly.
 * This function uses a helper function to check if the word meets the
 * requirements to be considered a valid Boggle word. If the word is a
 * valid Boggle word, it is checked to see if the first letter of the word
 * appears on the board. If it is, its position on the grid is added to a
 * vector of points and a helper function called findWordInGrid is called.
 * If this returns true, then word is added to the user's correct words list and
 * the user's score is updated as well.
 *
 */
Vector<string> playerTurn(Lexicon &english, Grid<string> &board,
                          Vector<string> &playerWords){
    string word = getLine("Enter a word: ");
    if(word == ""){ // base case
        return playerWords;
    }

    if(!checkWordPreReqs(word, english, playerWords)){
        return playerTurn(english,board, playerWords);
    }
        
            for(int f = 0; f<board.numRows(); f++){
                for(int s = 0; s<board.numCols(); s++){
                    if(board.get(f,s) == word.substr(0,1)){
                        Vector<Point> points;
                        Point pt (f,s);
                        points.add(pt);
                        if(findWordInGrid(word, f, s, board, 1, points)){
                            playerWords.add(word);
                            recordWordForPlayer(word, HUMAN);
                            return playerTurn(english,board, playerWords);
                        }
                }
                }

    }
    cout << "You can't make that word!" << endl;
    return playerTurn(english,board,playerWords);
}

/* This function checks if the user's word meets the pre-req's to be considered
 * a valid word for Boggle. If the word is less than the standard Boggle word
 * length or the word has been used already by the user, then this function will
 * return false.
 */
bool checkWordPreReqs(string word, Lexicon &english, Vector<string> &playerWords){
    if(word.length() < STANDARD_ROW_NUMBER){
        cout<< "I'm sorry, but we have our standards. ";
        cout << "That word doesn't meet the minimum word length." << endl;
        return false;
    }
    if(english.contains(word)) {
        for(int d = 0; d < playerWords.size(); d++){
            if(word == playerWords[d]){
                cout << "You've already guessed that!" << endl;
                return false;
        }
        }
        return true;
}
    cout << "You can't make that word silly goose!" << endl;
    return false;
}

/* This function performs the computer's exhuastive search to find all the possible
 * words that haven't been found by the user. It checks the word passed in its parameters
 * to see if it meets the standard Boggle length for a word. If it does, it calls another
 * function to record the word and score for the computer. This same word is also checked
 * to see if it is a prefix for another word in the English dictionary. If it is, a
 * helper function is called to add potential adjacent letters of the current cube to
 * the word to make it a longer valid word.
 */
void computersTurn(Lexicon & usedWords, Grid<string> & board, Lexicon & english,
                   string word, int rowNum, int colNum){
    
    if(word.length()>=STANDARD_ROW_NUMBER){ // base case
        recordWordForComputer(english, word, usedWords);
    }

    if(english.containsPrefix(word)){
        addAdjacentCubesToPossibleWord(usedWords, board, english, word, rowNum, colNum);
            } else {
        return;
    }
        }

/* This function gets the letter from the current cube and appendages it to
 * the current word. Then for every adjacent cube that is inbounds and isn't the
 * current cube, it sets the current cube as a used cube by setting the value
 * of it to "_" and calling the computersTurn function with the new cube coordinates.
 * It then sets the current cube back to its orginal letter so it could be reused
 * for the next adjacent cube.
 */
void addAdjacentCubesToPossibleWord(Lexicon & usedWords, Grid<string> & board, Lexicon & english,
                                    string word, int rowNum, int colNum){
    string letter = board.get(rowNum, colNum);
    for(int row = -1; row <= 1; row++){
        for(int col = -1; col<= 1; col++){
            if(!(row == 0 && col == 0) && board.inBounds(rowNum + row, colNum + col)
                    && board.get(rowNum + row, colNum + col) != "_"){
                        board.set(rowNum, colNum, "_");
                        computersTurn(usedWords, board, english, word + letter,
                                      rowNum + row, colNum + col);
                        board.set(rowNum, colNum, letter);
                }
        }
            }
}

/* This function finds the user's word in the grid and once it has been found, it highlights
 * the cubes that make up the word. For each adjacent cube, its letter is checked
 * to see if it is the same letter for the correct position in the word. If it is, then its
 * coordinates are checked to see if this cube hasn't already been used. It recursively calls
 * itself until the word length has been reached and highlights the cubes and returns true.
 */
bool findWordInGrid(string word, int rowNum, int colNum, Grid<string> &board, int numb, Vector<Point> points){
    if(numb == word.length()){ //base case
        highlightCubes(points);
        return true;
    }
    
    for(int row = -1; row <= 1; row++){
        for(int col = -1; col<= 1; col++){
            if(board.inBounds(rowNum + row, colNum + col) &&
                    board.get(rowNum + row, colNum + col) == word.substr(numb,1)){
                Point pt (rowNum + row, colNum + col);
                for(int i = 0; i<points.size(); i++){
                    if(points[i] == pt) return false;
                }
                points.add(pt);
                if(findWordInGrid(word, rowNum + row, colNum + col, board, numb+1, points) == true){
                    return true;
                } else {
                    points.remove(points.size()-1);
                }
            }
        }
    }
    return false;
}

/*This function highlights each cube that corresponds to a letter
 * in the user's guessed word. It pauses for a short period of time,
 * and then the cubes are unhighlighted.
 */
void highlightCubes(Vector<Point> points){
    for(int a = 0; a<points.size(); a++){
        int x = points[a].getX();
        int y = points[a].getY();
        highlightCube(x,y, true);
    }
    pause(MAX_TIME);
    for(int a = 0; a<points.size(); a++){
        int x = points[a].getX();
        int y = points[a].getY();
        highlightCube(x,y, false);
    }
}

/*This helper function converts a Vector of strings into a Lexicon
 * by iterating through the vector and adding each value to the lexicon.
 */
Lexicon convertVectorToLexicon(Vector<string> & theVec){
    Lexicon lex;
    for(int i = 0; i<theVec.size(); i++){
        lex.add(theVec[i]);
    }
    return lex;
}

/* This function checks if the computer's word is a valid english word
 * and isn't a word that has been already used. If it passes these checks
 * then it is recorded on the GWindow and the computer's score is updated.
 */
void recordWordForComputer(Lexicon &english, string word, Lexicon &usedWords){
    if(english.contains(word)) {
                if(!usedWords.contains(word)){
                    usedWords.add(word);
                    recordWordForPlayer(word, COMPUTER);
        }
}
}
