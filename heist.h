// =====================================================================
//  heist.h  -  THE LOCKER HEIST  (beginner-friendly, NO templates)
// =====================================================================
//
//  WHAT IS A HEADER FILE?
//  A header file is a "list of promises". It tells the compiler:
//  "these classes exist, and they have these functions".
//  The real work (the code inside the functions) is written later
//  in heist.cpp. main.cpp then uses everything.
//
//      heist.h    -> declares  (the menu card)
//      heist.cpp  -> defines   (the kitchen that cooks the food)
//      main.cpp   -> runs      (the customer who orders)
//
//  NO TEMPLATES IN THIS PROJECT
//  Arrays are made by hand with "new" and freed with "delete[]".
//  (TileRack, Scrabble and Cottage each manage their own memory.)
//
//  NOTE ABOUT "using namespace std;"
//  The line "using namespace std;" is written below the #includes.
//  Because of it we can write string, cout and ostream directly,
//  with no prefix in front of them.
//  heist.cpp and main.cpp get this line automatically, because they
//  include this file.
//
//  HOW THE 7 SUSPECTS ARE NUMBERED (index 0 to 6)
//  This is the same order as the lines in clue.txt:
//      0 = Cook      1 = Guard     2 = Servant   3 = Partner 1
//      4 = Partner 2 5 = PA        6 = Child
//
//  POSITIONS AND NUMBERS
//  - Board positions in the text files start at 1  (Word 1: 1 to 7)
//  - Menu choices start at 1                       ([1] Visit a Location)
//  - C++ array indexes start at 0
//    So position 3 is stored at index 2.  (position - 1 = index)
// =====================================================================


// ---------------------------------------------------------------------
// INCLUDE GUARD
// If two files both include heist.h, the compiler would see every class
// twice and complain. These 3 lines (#ifndef / #define / #endif) make
// sure the file is read only ONE time.
// ---------------------------------------------------------------------
#ifndef HEIST_H
#define HEIST_H

#include <iostream>   // cout, cin (screen and keyboard)
#include <fstream>    // ifstream, ofstream (reading and writing text files)
#include <string>     // string (text)

// Lets us write  string, cout, cin, ostream ...  instead of putting the
// scope resolution prefix in front of every name.
using namespace std;


// =====================================================================
//  MODULE 1 : ENUMERATIONS
//  An enum is a list of named choices. Behind the scenes each name is
//  a number (the first is 0, then 1, 2 ...). Names are easier to read
//  than numbers:  "if (state == STATE_WON)"  is clearer than  "== 2".
// =====================================================================

// Which of the two words a letter belongs to.
enum WordRow {
    WORD_1 = 0,       // the 7-letter word (letters from locations)
    WORD_2 = 1        // the 8-letter word (letters from people)
};

// What the game is doing right now.
enum GameState {
    STATE_MENU,       // showing the main menu
    STATE_PLAYING,    // a game is running
    STATE_WON,        // the detective caught the culprit
    STATE_LOST,       // out of turns or credibility
    STATE_EXIT        // the program should close
};

// The options of the main menu. The numbers match what the user types.
enum MenuChoice {
    MENU_NEW_GAME   = 1,
    MENU_SCOREBOARD = 2,
    MENU_ABOUT      = 3,
    MENU_RULES      = 4,
    MENU_EXIT       = 5
};


// =====================================================================
//  MODULE 7 : EXCEPTIONS (error handling)
//
//  An exception is how the game says "something went wrong".
//  A function "throws" it, and main() "catches" it so the program
//  does not crash.
//
//  How it is used in the .cpp file:
//      throw RackFullException("The rack is full");   <- raise the error
//      try { ... } catch (GameException& e) { ... }   <- handle it in main()
// =====================================================================

// The parent of every game error. All the others inherit from it, so
// ONE catch for GameException catches them all.
class GameException {
protected:
    string message;                 // the error text

public:
    GameException(const string& msg = "Game error");   // saves the text
    virtual ~GameException();            // virtual destructor (needed for inheritance)
    const char* what() const;            // gives the text back so we can print it
};

// Bad menu number, letters instead of a number, empty or spaced name ...
class InvalidInputException : public GameException {
public:
    InvalidInputException(const string& msg = "Invalid input");
};

// The typed word is wrong (wrong length, wrong letters, not the answer).
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

// Trying to take a tile from an empty rack (or a letter that is not there).
class RackEmptyException : public GameException {
public:
    RackEmptyException(const string& msg = "Rack is empty");
};

// Trying to form a word before the whole row is filled.
class WordIncompleteException : public GameException {
public:
    WordIncompleteException(const string& msg = "Word is not complete");
};

// Interviewing someone whose minimum credibility you do not have yet.
class LockedCharacterException : public GameException {
public:
    LockedCharacterException(const string& msg = "This person cannot be interviewed yet");
};

// A text file is missing, cannot be opened, or has bad data.
class FileException : public GameException {
public:
    FileException(const string& msg = "File error");
};


// =====================================================================
//  MODULE 1 : STRUCTS
//  A struct is a small box that groups related data together.
//  (Structs are used to improve code reusability: one definition, used
//   by many classes.)
//  A struct's members are public by default, so other code can read
//  them directly, like  tile.tileLetter
// =====================================================================

// ---- Tile: ONE square of the Scrabble board, or ONE tile in a rack ----
struct Tile {
    char tileLetter;     // the letter, for example 'A'
    bool filled;         // true = a letter is placed, false = empty slot

    Tile();                                  // makes an EMPTY tile
    Tile(char letter, bool isFilled);        // makes a tile with values
    bool operator==(const Tile& other) const;   // compares two tiles by letter

    // "friend" functions can print / read a Tile with cout / cin:
    //      cout << myTile;
    friend ostream& operator<<(ostream& out, const Tile& t);
    friend istream& operator>>(istream& in, Tile& t);
};

// ---- Clue: a small "delivery package" for one letter ----
// When you search a location or interview a person you get a Clue.
// It carries the letter to the Scrabble board.
struct Clue {
    char clueLetter;     // the letter found
    int cluePosition;    // where it goes on the board (starts at 1)
    WordRow clueRow;     // WORD_1 or WORD_2

    Clue();                                          // empty clue
    Clue(char letter, int position, WordRow row);    // clue with values
};

// ---- Location: ONE room or place in the cottage ----
struct Location {
    string locName;     // for example "Kitchen"
    char letterClueLoc;      // the letter hidden here
    int boardPosLoc;         // where that letter goes in Word 1 (1 to 7)
    WordRow locRow;          // always WORD_1 for locations
    bool locSearchFlag;      // true after the detective searched it

    Location();                                                  // empty location
    Location(const string& name, char letter, int position);  // with values

    friend ostream& operator<<(ostream& out, const Location& loc);
    friend istream& operator>>(istream& in, Location& loc);
};


// =====================================================================
//  MODULE 5 : TileRack
//  The rack holds the letters the detective has collected so far,
//  like the little holder in real Scrabble.
//
//  DYNAMIC MEMORY (DMA)
//  The rack makes its own array with "new":
//      constructor     : tiles = new Tile[maxTiles];
//      destructor      : delete[] tiles;
//      copy constructor: makes a NEW array and copies each tile (deep copy)
//      operator=       : same, but first deletes the old array
// =====================================================================
class TileRack {
private:
    Tile* tiles;        // pointer to the array of tiles on the heap
    int count;          // how many tiles are in the rack right now
    int maxTiles;       // size of the array: 7 for Word 1, 8 for Word 2

public:
    TileRack(int maxSize = 8);                       // makes an empty rack
    TileRack(const TileRack& other);                 // deep copy
    TileRack& operator=(const TileRack& other);      // deep copy
    ~TileRack();                                     // frees the array

    void addTile(char letter);       // add a letter (throws RackFullException)
    void removeLetter(char letter);  // remove a letter (throws RackEmptyException)
    int getCount() const;            // how many letters are in the rack
    int getMaxTiles() const;         // the largest size allowed
    bool isFull() const;             // true if no space left
    bool isEmpty() const;            // true if no letters inside
    char getLetterAt(int index) const;   // the letter at a position (0-based)
    void display() const;            // print the rack on screen

    // MODULE 11 - Operator +
    // rack1 + rack2 makes ONE new rack: letters of rack1, then a
    // space, then letters of rack2. This is the final "clue".
    TileRack operator+(const TileRack& other) const;

    friend ostream& operator<<(ostream& out, const TileRack& rack);
};


// =====================================================================
//  MODULE 4 : Scrabble (the game board)
//
//  The board has TWO rows (a "jagged" array: rows of different length):
//      Row 1 : 7 slots for Word 1
//      Row 2 : 8 slots for Word 2
//  Each slot is a Tile. The position of a tile is its index.
//
//  DYNAMIC MEMORY (DMA)
//  A jagged array is a "pointer to pointers":
//      grid = new Tile*[2];             // 2 rows
//      grid[0] = new Tile[7];           // row 1 has 7 slots
//      grid[1] = new Tile[8];           // row 2 has 8 slots
//  The destructor must delete each row first, then delete grid itself.
//  Because WORD_1 is 0 and WORD_2 is 1, grid[WORD_1] is row 1 and
//  grid[WORD_2] is row 2.
// =====================================================================
class Scrabble {
private:
    Tile** grid;                     // pointer to the 2 rows
    string targetWord1;         // the CORRECT Word 1 (from clue.txt)
    string targetWord2;         // the CORRECT Word 2 (from clue.txt)

    // Helper: how many slots a row has (7 for WORD_1, 8 for WORD_2).
    int rowLength(WordRow row) const;

public:
    static const int WORD1_LENGTH = 7;   // shared fixed numbers
    static const int WORD2_LENGTH = 8;

    Scrabble();                                  // makes the two empty rows
    Scrabble(const Scrabble& other);             // deep copy
    Scrabble& operator=(const Scrabble& other);  // deep copy
    ~Scrabble();                                 // frees both rows and the grid

    // Save the two correct words for the chosen culprit.
    void setTargetWords(const string& w1, const string& w2);
    string getTargetWord(WordRow row) const;

    // Put a letter on the board using a Clue.
    // Throws InvalidPositionException or SlotFilledException.
    void placeLetter(const Clue& clue);

    bool isRowComplete(WordRow row) const;       // true when every slot is filled
    string getRowString(WordRow row) const; // the row as text, empty slot = '_'

    // Check the player's typed word against the correct word.
    // Throws WordIncompleteException if the row is not full,
    // InvalidWordException if the length or letters are wrong.
    // Returns true only when the word is exactly right.
    bool checkWord(WordRow row, const string& guess) const;

    void displayRow(WordRow row) const;          // print one row
    void displayBoard() const;                   // print both rows

    friend ostream& operator<<(ostream& out, const Scrabble& board);
    friend istream& operator>>(istream& in, Scrabble& board);   // reads both target words
};


// =====================================================================
//  MODULE 4 : Cottage (the 7 places the detective can search)
//  Aggregation: the Cottage HAS an array of Location.
//
//  DYNAMIC MEMORY (DMA)
//      constructor : locations = new Location[LOCATION_COUNT];
//      destructor  : delete[] locations;
//  addLocation() fills the next free slot.
// =====================================================================
class Cottage {
private:
    Location* locations;     // pointer to the array of locations on the heap
    int locationCount;       // how many locations are filled in so far

public:
    static const int LOCATION_COUNT = 7;

    Cottage();                                   // makes space for 7 empty locations
    Cottage(const Cottage& other);               // deep copy
    Cottage& operator=(const Cottage& other);    // deep copy
    ~Cottage();                                  // frees the array

    void addLocation(const Location& loc);       // fill the next slot (throws if all 7 are full)
    void clearLocations();                       // forget all rooms (count becomes 0)
    int getLocationCount() const;                // how many rooms are filled in
    const Location& getLocation(int index) const;    // one room (0-based)
    bool isSearched(int index) const;            // was it searched before?

    Clue searchLocation(int index);              // first search: marks it searched, gives the Clue
    Clue revisitLocation(int index) const;       // search again: gives the Clue, changes nothing

    void displayLocations() const;               // prints  [1] Bedroom 1 - NOT Searched
};


// =====================================================================
//  MODULES 3 and 9 : CHARACTER (base class for all people)
//
//  "abstract" = you can NOT make a plain Character object. It is only a
//  blueprint for the real people (Victim, Cook, Guard ...).
//  What makes it abstract:  display() = 0   (a "pure virtual" function).
//  Every child class MUST write its own display().
//
//  POLYMORPHISM: we can hold any person in a Character pointer and call
//      person->display();
//  and C++ automatically runs the right version for that person.
// =====================================================================
class Character {
protected:      // protected = children can use these, outsiders cannot
    string characterName;     // for example "Mr.Enzo"
    string nationality;       // for example "Italian"
    string characterRole;     // for example "Cook"
    bool isCulprit;                // true for the chosen culprit

public:
    Character(const string& name = "", const string& role = "",
              const string& nat = "");
    virtual ~Character();          // "virtual" so the right destructor runs

    // Getters (read a value) and setters (change a value).
    string getName() const;
    string getRole() const;
    string getNationality() const;
    bool getIsCulprit() const;
    void setName(const string& name);
    void setRole(const string& role);
    void setNationality(const string& nat);
    void setIsCulprit(bool value);

    // Every child must write its own display(). "= 0" makes it abstract.
    virtual void display() const = 0;

    // Interview information. These have simple default versions.
    // Victim and Suspect replace them with the real values (overriding).
    virtual char getLetter() const;             // the letter this person gives (Word 2)
    virtual int getPosition() const;            // where it goes on the board
    virtual WordRow getRow() const;             // which row (WORD_2)
    virtual int getMinCredibility() const;      // credibility needed to interview
    virtual bool isInterviewed() const;         // already interviewed?
    virtual void markInterviewed();             // remember "interviewed"

    Clue getClue() const;                       // pack letter + position + row into a Clue
    bool canInterview(int credibility) const;   // true if credibility >= minimum

    // GameEngine is our friend: it may use protected data directly,
    // for example to set isCulprit.
    // ("friend class" also tells the compiler that GameEngine exists.)
    friend class GameEngine;

    friend ostream& operator<<(ostream& out, const Character& c);
    friend istream& operator>>(istream& in, Character& c);   // reads name, role, nationality
};


// ---------------------------------------------------------------------
//  DETECTIVE  -  the player
// ---------------------------------------------------------------------
class Detective : public Character {
private:
    int credibility;            // starts at 100, the game ends at 0
    int turns;                  // turns used (GameEngine counts, Detective stores)
    TileRack rackWord1;         // letters for Word 1 (7 max)
    TileRack rackWord2;         // letters for Word 2 (8 max)
    string solvedWords;    // words solved so far

public:
    Detective(const string& name = "Detective", int startingCredibility = 100);
    virtual ~Detective();

    virtual void display() const;               // prints name, credibility, turns

    int getCredibility() const;
    int getTurns() const;
    void setCredibility(int value);
    void loseCredibility(int amount);           // lowers credibility, never below 0
    void useTurn();                             // turns = turns + 1

    TileRack& getRack(WordRow row);                 // the rack for Word 1 or Word 2
    const TileRack& getRack(WordRow row) const;
    void collectClue(const Clue& clue);         // puts the letter in the right rack

    void addSolvedWord(const string& word);
    string getSolvedWords() const;
};


// ---------------------------------------------------------------------
//  VICTIM  -  the French business owner.
//  Gets interviewed like the others but is NOT a suspect.
// ---------------------------------------------------------------------
class Victim : public Character {
private:
    WordRow clueRowChar;        // which row his letter belongs to (WORD_2)
    char letterClueChar;        // his letter
    int boardPosChar;           // where it goes (1 to 8)
    bool interviewed;           // already interviewed?

public:
    Victim(const string& name, const string& nat, char letter, int position);
    virtual ~Victim();

    virtual void display() const;
    virtual char getLetter() const;
    virtual int getPosition() const;
    virtual WordRow getRow() const;
    virtual int getMinCredibility() const;      // returns 80
    virtual bool isInterviewed() const;
    virtual void markInterviewed();
};


// ---------------------------------------------------------------------
//  SUSPECT  -  parent of the seven suspects.
//  Still abstract, because display() is not written here.
// ---------------------------------------------------------------------
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


// ---------------------------------------------------------------------
//  BUSINESS SIDE:  BusinessRelations -> Partner, PA
//
//  THE DIAMOND PROBLEM
//  PA inherits from BusinessRelations AND CloseRelations, and both of
//  those inherit from Suspect:
//
//      Suspect <-- BusinessRelations <-- PA
//      Suspect <-- CloseRelations    <-- PA
//
//  (read "A <-- B" as "B inherits from A")
//
//  Without help, PA would get TWO copies of Suspect (confusing!).
//  The fix is "virtual public": PA gets only ONE shared Suspect.
//  Because of this, the PA constructor must call the Suspect
//  constructor itself (see PA below).
// ---------------------------------------------------------------------
class BusinessRelations : virtual public Suspect {
public:
    BusinessRelations(const string& name, const string& role,
                      const string& nat, char letter, int position, int minCred);
    virtual ~BusinessRelations();
};

// Partner 1 and Partner 2 (minimum credibility 50)
class Partner : public BusinessRelations {
private:
    int partnerNumber;          // 1 or 2
public:
    Partner(const string& name, const string& nat, char letter,
            int position, int partnerNo);
    virtual ~Partner();
    virtual void display() const;
    int getPartnerNumber() const;
};


// ---------------------------------------------------------------------
//  CLOSE RELATIONS:  CloseRelations -> OwnersChild, PA
// ---------------------------------------------------------------------
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


// ---------------------------------------------------------------------
//  PA  -  the owner's personal assistant (minimum credibility 35)
//  Inherits from TWO parents. This is "multiple inheritance".
// ---------------------------------------------------------------------
class PA : public BusinessRelations, public CloseRelations {
public:
    PA(const string& name, const string& nat, char letter, int position);
    virtual ~PA();
    virtual void display() const;
};


// ---------------------------------------------------------------------
//  HOUSE HELP  -  Cook, Guard, Servant (can be interviewed any time)
// ---------------------------------------------------------------------
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


// =====================================================================
//  MODULE 6 : SCORE BOARD and RULES (working with text files)
// =====================================================================

// ScoreBoard saves and shows past games in leader/scoreBoard.txt
// (the folder "leader" must already exist next to your program).
class ScoreBoard {
private:
    string fileName;       // where the scores are stored

public:
    ScoreBoard(const string& file = "leader/scoreBoard.txt");

    // Add one line at the end of the file:
    //   Detective|Result|Credibility|Turns_Left|Culprit|Date
    // Throws FileException if the file cannot be opened.
    void addScore(const string& detective, const string& result,
                  int credibility, int turnsLeft, const string& culprit);

    // Print every saved line. Throws FileException if the file is missing.
    void displayAll() const;

    // Today's date as text, for example "07-11-26".
    static string todayDate();
};

// GameRules keeps the numbers from rules.txt.
// If the file is missing or broken, the default numbers are used.
// Penalties are stored as positive numbers (the file has -5 and -10).
struct GameRules {
    int maxTurns;               // 25
    int startingCredibility;    // 100
    int wrongTurnPenalty;       // 5  (lost for a wrong step)
    int wrongWordPenalty;       // 10 (lost for a wrong word)
    string winCondition;
    string loseCondition;

    GameRules();                                     // sets the default values
    bool loadFromFile(const string& fileName);  // false = file failed, defaults kept
    void display() const;                            // prints the rules
};


// =====================================================================
//  MODULE 12 : GameEngine  -  the boss of the whole game
//
//  SINGLETON PATTERN: only ONE GameEngine can ever exist.
//    - the constructor is private (nobody can write GameEngine g;)
//    - getInstance() creates it the first time and gives the same one
//      back every time after that.
//
//  RELATIONSHIPS (for your class diagram):
//    Composition : GameEngine OWNS Detective, Cottage, Scrabble, TileRack
//                  (they live and die with the engine)
//    Association : GameEngine USES ScoreBoard (it does not own it)
//    Dependency  : GameEngine uses GameException
//    Friend      : GameEngine is a friend of Character
// =====================================================================
class GameEngine {
private:
    static GameEngine* instance;      // the one and only engine

    // ---- parts the engine OWNS (composition) ----
    GameState state;                  // menu / playing / won / lost / exit
    GameRules rules;                  // numbers from rules.txt
    Detective detective;              // the player
    Cottage cottage;                  // the 7 locations
    Scrabble scrabble;                // the board
    TileRack mergedRack;              // Word 1 rack + Word 2 rack (using operator+)

    // ---- part the engine only USES (association) ----
    ScoreBoard* scoreBoard;

    // ---- the 8 people (made with "new" and deleted in the destructor) ----
    // people[0] = Victim
    // people[1..7] = Cook, Guard, Servant, Child, Partner 1, Partner 2, PA
    // This is the order of the interview menu.
    // WARNING: this is NOT the same order as the culprit index (0 to 6)!
    static const int PEOPLE_COUNT = 8;
    static const int SUSPECT_COUNT = 7;
    Character* people[PEOPLE_COUNT];  // a Character pointer can hold ANY child class

    // ---- data of the game that is running now ----
    int culpritIndex;                 // 0 to 6 (see the numbering at the top of this file)
    string culpritRole;          // for example "Servant"
    string word1;                // the correct 7-letter word
    string word2;                // the correct 8-letter word
    bool word1Solved;                 // did the player type Word 1 correctly?
    bool word2Solved;                 // did the player type Word 2 correctly?

    // ---- Singleton: lock the doors ----
    GameEngine();                                 // private: only getInstance() may call it
    GameEngine(const GameEngine&);                // declared but NEVER written: blocks copying
    GameEngine& operator=(const GameEngine&);     // same: blocks "a = b"
    ~GameEngine();                                // frees the 8 people

    // ---- STEP 1 of a new game: load files (Module 2) ----
    void loadRules();                 // read rules.txt (use defaults if it fails)
    void pickCulprit();               // choose a random line of clue.txt
    void loadClueWords();             // get Word 1 and Word 2 of that culprit
    void loadLocations();             // read locations.txt, using the culprit's column
    void loadCharacters();            // read characters.txt, using the culprit's column
    void clearCharacters();           // delete every person (no memory leak)
    void resetGame(const string& detectiveName);   // start fresh

    // ---- small helpers for screen and keyboard ----
    static void clearScreen();                    // clears the console
    static void pauseScreen();                    // "press Enter to continue"
    static int readInt(int low, int high);        // reads a number, throws if outside low..high
    static bool readYesNo(const string& prompt);   // asks Y/N
    static string readDetectiveName();       // name with no spaces ('_' or '.' allowed)

    // ---- menus (Module 13) ----
    void showMainMenu() const;
    void showGameMenu() const;
    void showStatus() const;          // credibility, turns, rack, board
    void showAbout() const;           // prints about.txt
    void showRules() const;           // prints the rules

    // ---- what the player can do ----
    void handleMainMenu();            // reacts to the main menu choice
    void handleGameTurn();            // shows the game menu and reacts
    void visitLocation();             // Game menu [1] (uses a turn)
    void interviewPerson();           // Game menu [2] (uses a turn)
    void viewBoard() const;           // Game menu [3] (FREE, no turn)
    void formWord();                  // Game menu [4] (wrong word = -10 credibility)
    void accuseSuspect();             // Game menu [5]
    void quitGame();                  // Game menu [6]
    void spendTurn();                 // adds 1 turn, then checks if the game is over

    // ---- end of the game (Module 14) ----
    void checkEndConditions();        // out of turns or credibility?
    void finishGame();                // show WIN / LOSE and save to the score board

public:
    static GameEngine& getInstance();   // get the one engine (creates it the first time)
    static void destroyInstance();      // delete the engine at the end of main()

    void attachScoreBoard(ScoreBoard* board);   // tell the engine which score board to use

    // Does ONE action (one menu choice or one turn).
    // main() calls it again and again inside try/catch, so every
    // GameException is caught in main() as the brief requires.
    void step();

    // Called from main()'s catch block. Prints the error and, while a
    // game is running, takes the -5 wrong-step credibility.
    void handleException(const GameException& e);

    GameState getState() const;       // main() uses this to know when to stop
};

#endif   // end of the include guard (HEIST_H)
