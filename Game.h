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





// Hannan Part :

//  DETECTIVE  -  the player

class Detective : public Character {
private:
    int credibility;            
    int turns;                 
    TileRack rackWord1;        
    TileRack rackWord2;       
    string solvedWords;   

public:
    Detective(const string& name = "Detective", int startingCredibility = 100);
    virtual ~Detective();

    virtual void display() const;            

    int getCredibility() const;
    int getTurns() const;
    void setCredibility(int value);
    void loseCredibility(int amount);           
    void useTurn();                             

    TileRack& getRack(WordRow row);                
    const TileRack& getRack(WordRow row) const;
    void collectClue(const Clue& clue);         

    void addSolvedWord(const string& word);
    string getSolvedWords() const;
};



//  VICTIM  -  the French business owner.

class Victim : public Character {
private:
    WordRow clueRowChar;       
    char letterClueChar;      
    int boardPosChar;          
    bool interviewed;         

public:
    Victim(const string& name, const string& nat, char letter, int position);
    virtual ~Victim();

    virtual void display() const;
    virtual char getLetter() const;
    virtual int getPosition() const;
    virtual WordRow getRow() const;
    virtual int getMinCredibility() const;      
    virtual bool isInterviewed() const;
    virtual void markInterviewed();
};



//  SUSPECT  -  parent of the seven suspects.

class Suspect : public Character {
protected:
    char letterClueChar;        // the letter this suspect gives
    int boardPosChar;           // where it goes (1 to 8)
    WordRow clueRowChar;        // WORD_2
    bool interviewed;           // already interviewed?
    int minCredibility;         // credibility needed to interview

public:
    Suspect(const string& name, const string& role, const string& nat,
            char letter, int position, int minCred);
    virtual ~Suspect();

    virtual char getLetter() const;
    virtual int getPosition() const;
    virtual WordRow getRow() const;
    virtual int getMinCredibility() const;
    virtual bool isInterviewed() const;
    virtual void markInterviewed();
};



//  BUSINESS SIDE:  BusinessRelations -> Partner, PA

class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& name, const string& role,
                      const string& nat, char letter, int position, int minCred);
    virtual ~BusinessRelations();
};

// Partner 1 and Partner 2 (minimum credibility 50)
class Partner : public BusinessRelations {
private:
    int partnerNumber;          
public:
    Partner(const string& name, const string& nat, char letter,
            int position, int partnerNo);
    virtual ~Partner();
    virtual void display() const;
    int getPartnerNumber() const;
};



//  CLOSE RELATIONS:  CloseRelations -> OwnersChild, PA

class CloseRelations : virtual public Suspect {
public:
    CloseRelations(const string& name, const string& role,
                   const string& nat, char letter, int position, int minCred);
    virtual ~CloseRelations();
};

// The owner's teenage child (age 15 to 19, minimum credibility 75)
class OwnersChild : public CloseRelations {
private:
    int age;
public:
    OwnersChild(const string& name, const string& nat, char letter,
                int position, int childAge = 17);
    virtual ~OwnersChild();
    virtual void display() const;
    int getAge() const;
};



//  PA  -  the owner's personal assistant (minimum credibility 35)

class PA : public BusinessRelations, public CloseRelations {
public:
    PA(const string& name, const string& nat, char letter, int position);
    virtual ~PA();
    virtual void display() const;
};



//  HOUSE HELP  -  Cook, Guard, Servant (can be interviewed any time)

class HouseHelp : public Suspect {
public:
    HouseHelp(const string& name, const string& role,
              const string& nat, char letter, int position);
    virtual ~HouseHelp();
};

class Cook : public HouseHelp {
public:
    Cook(const string& name, const string& nat, char letter, int position);
    virtual ~Cook();
    virtual void display() const;
};

class Guard : public HouseHelp {
public:
    Guard(const string& name, const string& nat, char letter, int position);
    virtual ~Guard();
    virtual void display() const;
};

class Servant : public HouseHelp {
public:
    Servant(const string& name, const string& nat, char letter, int position);
    virtual ~Servant();
    virtual void display() const;
};

#endif 
