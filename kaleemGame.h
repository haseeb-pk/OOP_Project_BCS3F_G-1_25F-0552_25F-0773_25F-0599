#ifndef Game_H
#define Game_H

#include <iostream>   
#include <fstream>    
#include <string>    

using namespace std;


//  MODULE 1 : Enumerations

enum WordRow {
    WORD_1 = 0,       // 7-letter from locations
    WORD_2 = 1        // 8-letter from people
};

// What the game is doing right now.
enum GameState {
    STATE_menu,      
    STATE_playing,   
    STATE_won,       
    STATE_lost,      
    STATE_exit       
};

// The options of the main menu
enum MenuChoice {
    MENU_new_game   = 1,
    MENU_scoreboard = 2,
    MENU_about      = 3,
    MENU_rules      = 4,
    MENU_exit       = 5
};


/*
  MODULE 7 : EXCEPTIONS (error handling) 
   An exception is how the game says "something went wrong".
  A function "throws" it, and main() "catches" it so the program
  does not crash.
*/

class GameException {
protected:
    string message;                

public:
    GameException(const string& msg = "Game error");   
    virtual ~GameException();          
    const char* what() const;          
};

// Bad menu number
class InvalidInputException : public GameException {
public:
    InvalidInputException(const string& msg = "Invalid input");
};

// The typed word is wrong 
class InvalidWordException : public GameException {
public:
    InvalidWordException(const string& msg = "Invalid word");
};

// A position or array index outside the allowed range.
class InvalidPositionException : public GameException {
public:
    InvalidPositionException(const string& msg = "Invalid position");
};

// A board slot already has a letter in it.
class SlotFilledException : public GameException {
public:
    SlotFilledException(const string& msg = "Slot already filled");
};

// Trying to add a tile to a rack that has no space left.
class RackFullException : public GameException {
public:
    RackFullException(const string& msg = "Rack is full");
};

// Trying to take a tile from an empty rack
class RackEmptyException : public GameException {
public:
    RackEmptyException(const string& msg = "Rack is empty");
};

// Trying to form a word before the whole row is filled.
class WordIncompleteException : public GameException {
public:
    WordIncompleteException(const string& msg = "Word is not complete");
};

// Interviewing someone whose minimum credibility you do not have yet
class LockedCharacterException : public GameException {
public:
    LockedCharacterException(const string& msg = "This person cannot be interviewed yet");
};

// A text file is missing, cannot be opened
class FileException : public GameException {
public:
    FileException(const string& msg = "File error");
};

//  MODULE 1 : STRUCTS
//  A struct is a small box that groups related data together.
//  (Structs are used to improve code reusability: one definition, used
//   by many classes.)
// 

struct Tile {
    char tileLetter;     
    bool filled;         

    Tile();                                  
    Tile(char letter, bool isFilled);       
    bool operator==(const Tile& other) const;   

  
    friend ostream& operator<<(ostream& out, const Tile& t);
    friend istream& operator>>(istream& in, Tile& t);
};

// Clue: a small "delivery package" for one letter 

struct Clue {
    char clueLetter;     
    int cluePosition;   
    WordRow clueRow;     

    Clue();                                          
    Clue(char letter, int position, WordRow row);    
};

//  Location: ONE room or place in the cottage 
struct Location {
    string locName;     
    char letterClueLoc;     
    int boardPosLoc;         
    WordRow locRow;         
    bool locSearchFlag;    

    Location();                                                  
    Location(const string& name, char letter, int position);

    friend ostream& operator<<(ostream& out, const Location& loc);
    friend istream& operator>>(istream& in, Location& loc);
};




//  MODULE 5 : TileRack
//  The rack holds the letters the detective has collected so far,like the little holder in real Scrabble.

class TileRack {
private:
    Tile* tiles;        
    int count;         
    int maxTiles;      

public:
    TileRack(int maxSize = 8);                       
    TileRack(const TileRack& other);                 
    TileRack& operator=(const TileRack& other);    
    ~TileRack();                                     

    void addTile(char letter);       
    void removeLetter(char letter);  
    int getCount() const;           
    int getMaxTiles() const;       
    bool isFull() const;             
    bool isEmpty() const;         
    char getLetterAt(int index) const;   
    void display() const;           

    // MODULE 11 - Operator +
    TileRack operator+(const TileRack& other) const;

    friend ostream& operator<<(ostream& out, const TileRack& rack);
};

// Kaleem Part :
//  MODULE 4 : Scrabble (the game board)
//
//  The board has TWO rows (a "jagged" array: rows of different length):
//      Row 1 : 7 slots for Word 1
//      Row 2 : 8 slots for Word 2

class Scrabble {
private:
    Tile** grid;
    string targetWord1;
    string targetWord2;

    int rowLength(WordRow row) const;

public:
    static const int WORD1_LENGTH = 7;
    static const int WORD2_LENGTH = 8;

    Scrabble();
    Scrabble(const Scrabble& other);
    Scrabble& operator=(const Scrabble& other);
    ~Scrabble();


    void setTargetWords(const string& w1, const string& w2);
    string getTargetWord(WordRow row) const;


    void placeLetter(const Clue& clue);

    bool isRowComplete(WordRow row) const;
    string getRowString(WordRow row) const;


    bool checkWord(WordRow row, const string& guess) const;

    void displayRow(WordRow row) const;
    void displayBoard() const;

    friend ostream& operator<<(ostream& out, const Scrabble& board);
    friend istream& operator>>(istream& in, Scrabble& board);
};



// MODULE 4 : Cottage (the 7 places the detective can search)



class Cottage {
private:
    Location* locations;
    int locationCount;

public:
    static const int LOCATION_COUNT = 7;

    Cottage();
    Cottage(const Cottage& other);
    Cottage& operator=(const Cottage& other);
    ~Cottage();

    void addLocation(const Location& loc);
    void clearLocations();
    int getLocationCount() const;
    const Location& getLocation(int index) const;
    bool isSearched(int index) const;

    Clue searchLocation(int index);
    Clue revisitLocation(int index) const;

    void displayLocations() const;
};


//  MODULES 3 and 9 : CHARACTER (base class for all people)
class Character {
protected:
    string characterName;
    string nationality;
    string characterRole;
    bool isCulprit;

public:
    Character(const string& name = "", const string& role = "",
        const string& nat = "");
    virtual ~Character();


    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    void setName(const string& name);
    void setRole(const string& role);
    void setNationality(const string& nat);
    void setIsCulprit(bool value);


    virtual void display() const = 0;

    virtual char getLetter() const;
    virtual int getPosition() const;
    virtual WordRow getRow() const;
    virtual int getMinCredibility() const;
    virtual bool isInterviewed() const;
    virtual void markInterviewed();

    Clue getClue() const;
    bool canInterview(int credibility) const;


    friend class GameEngine;

    friend ostream& operator<<(ostream& out, const Character& c);
    friend istream& operator>>(istream& in, Character& c);   // reads name, role, nationality
};




// Hannan Part :



#endif 