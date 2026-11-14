#include "Game.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>

using namespace std;

// MODULE 7: EXCEPTION HANDLING

GameException::GameException(const string& msg) {
    this->message = msg;
}

GameException::~GameException() {
}

const char* GameException::what() const { 
    return this->message.c_str(); 
}

// Derived exceptions just pass the message to the base class
InvalidInputException::InvalidInputException(const string& msg) : GameException(msg) {}
InvalidWordException::InvalidWordException(const string& msg) : GameException(msg) {}
InvalidPositionException::InvalidPositionException(const string& msg) : GameException(msg) {}
SlotFilledException::SlotFilledException(const string& msg) : GameException(msg) {}
RackFullException::RackFullException(const string& msg) : GameException(msg) {}
RackEmptyException::RackEmptyException(const string& msg) : GameException(msg) {}
WordIncompleteException::WordIncompleteException(const string& msg) : GameException(msg) {}
LockedCharacterException::LockedCharacterException(const string& msg) : GameException(msg) {}
FileException::FileException(const string& msg) : GameException(msg) {}


// MODULE 1: STRUCTS

Tile::Tile() {
    this->tileLetter = ' ';
    this->filled = false;
}

Tile::Tile(char letter, bool isFilled) {
    this->tileLetter = letter;
    this->filled = isFilled;
}

bool Tile::operator==(const Tile& other) const { 
    if (this->tileLetter == other.tileLetter && this->filled == other.filled) {
        return true;
    } else {
        return false;
    }
}

ostream& operator<<(ostream& out, const Tile& t) {
    if (t.filled == true) {
        out << "[" << t.tileLetter << "]";
    } else {
        out << "[ ]";
    }
    return out;
}

istream& operator>>(istream& in, Tile& t) {
    in >> t.tileLetter;
    t.filled = true;
    return in;
}

Clue::Clue() {
    this->clueLetter = ' ';
    this->cluePosition = 0;
    this->clueRow = WORD_1;
}

Clue::Clue(char letter, int position, WordRow row) {
    this->clueLetter = letter;
    this->cluePosition = position;
    this->clueRow = row;
}

Location::Location() {
    this->locName = "";
    this->letterClueLoc = ' ';
    this->boardPosLoc = 0;
    this->locRow = WORD_1;
    this->locSearchFlag = false;
}

Location::Location(const string& name, char letter, int position) {
    this->locName = name;
    this->letterClueLoc = letter;
    this->boardPosLoc = position;
    this->locRow = WORD_1;
    this->locSearchFlag = false;
}

ostream& operator<<(ostream& out, const Location& loc) { 
    out << loc.locName; 
    return out; 
}

istream& operator>>(istream& in, Location& loc) {
    in >> loc.locName >> loc.letterClueLoc >> loc.boardPosLoc;
    return in;
}

// MODULE 5: TILERACK


TileRack::TileRack(int maxSize) {
    this->count = 0;
    this->maxTiles = maxSize;
    this->tiles = new Tile[maxTiles]; 
}

// Copy constructor for deep copy
TileRack::TileRack(const TileRack& other) {
    this->count = other.count;
    this->maxTiles = other.maxTiles;
    this->tiles = new Tile[maxTiles];
    
    // loop and copy each tile
    for (int i = 0; i < count; i++) {
        this->tiles[i] = other.tiles[i];
    }
}

// Assignment operator for deep copy
TileRack& TileRack::operator=(const TileRack& other) {
    if (this != &other) {
        delete[] this->tiles; // free old memory first
        
        this->count = other.count;
        this->maxTiles = other.maxTiles;
        this->tiles = new Tile[maxTiles];
        
        for (int i = 0; i < count; i++) {
            this->tiles[i] = other.tiles[i];
        }
    }
    return *this;
}

TileRack::~TileRack() { 
    delete[] tiles;
}

void TileRack::addTile(char letter) {
    if (isFull() == true) {
        throw RackFullException("Rack is full.");
    }
    tiles[count] = Tile(letter, true);
    count++;
}

void TileRack::removeLetter(char letter) {
    if (isEmpty() == true) {
        throw RackEmptyException("Rack is empty.");
    }
    
    int foundIndex = -1;
    
    for (int i = 0; i < count; i++) {
        if (tiles[i].tileLetter == letter) { 
            foundIndex = i; 
            break; 
        }
    }
    
    // Shift elements left to remove it
    if (foundIndex != -1) {
        for (int i = foundIndex; i < count - 1; i++) {
            tiles[i] = tiles[i + 1];
        }
        count--;
    }
}

int TileRack::getCount() const { return count; }
int TileRack::getMaxTiles() const { return maxTiles; }

bool TileRack::isFull() const { 
    if (count == maxTiles) return true;
    return false;
}

bool TileRack::isEmpty() const { 
    if (count == 0) return true;
    return false;
}

char TileRack::getLetterAt(int index) const {
    if (index < 0 || index >= count) {
        throw InvalidPositionException("Invalid Rack Index");
    }
    return tiles[index].tileLetter;
}

void TileRack::display() const {
    if (count == 0) { 
        cout << "(empty)"; 
    } else {
        for (int i = 0; i < count; i++) {
            cout << tiles[i].tileLetter << " ";
        }
    }
}

TileRack TileRack::operator+(const TileRack& other) const {
    int newSize = this->maxTiles + other.maxTiles;
    TileRack mergedRack(newSize);
    

    for (int i = 0; i < this->count; i++) {
        mergedRack.addTile(this->tiles[i].tileLetter);
    }

    for (int i = 0; i < other.count; i++) {
        mergedRack.addTile(other.tiles[i].tileLetter);
    }
    
    return mergedRack;
}

ostream& operator<<(ostream& out, const TileRack& rack) { 
    rack.display(); 
    return out; 
}

// MODULE 4: SCRABBLE & COTTAGE (BOARDS)

int Scrabble::rowLength(WordRow row) const { 
    if (row == WORD_1) {
        return WORD1_LENGTH;
    } else {
        return WORD2_LENGTH;
    }
}

Scrabble::Scrabble() {
    this->targetWord1 = "";
    this->targetWord2 = "";
    this->grid = new Tile*[2];
    this->grid[0] = new Tile[WORD1_LENGTH];
    this->grid[1] = new Tile[WORD2_LENGTH];
}

Scrabble::Scrabble(const Scrabble& other) {
    this->targetWord1 = other.targetWord1; 
    this->targetWord2 = other.targetWord2;
    this->grid = new Tile*[2];
    this->grid[0] = new Tile[WORD1_LENGTH];
    this->grid[1] = new Tile[WORD2_LENGTH];
    
    for (int i = 0; i < WORD1_LENGTH; i++) {
        this->grid[0][i] = other.grid[0][i];
    }
    for (int j = 0; j < WORD2_LENGTH; j++) {
        this->grid[1][j] = other.grid[1][j];
    }
}

Scrabble& Scrabble::operator=(const Scrabble& other) {
    if (this != &other) {
        this->targetWord1 = other.targetWord1; 
        this->targetWord2 = other.targetWord2;
        
        for (int i = 0; i < WORD1_LENGTH; i++) {
            this->grid[0][i] = other.grid[0][i];
        }
        for (int j = 0; j < WORD2_LENGTH; j++) {
            this->grid[1][j] = other.grid[1][j];
        }
    }
    return *this;
}

Scrabble::~Scrabble() { 
    delete[] grid[0]; 
    delete[] grid[1]; 
    delete[] grid; 
}

void Scrabble::setTargetWords(const string& w1, const string& w2) { 
    this->targetWord1 = w1; 
    this->targetWord2 = w2; 
}

string Scrabble::getTargetWord(WordRow row) const { 
    if (row == WORD_1) {
        return targetWord1;
    } else {
        return targetWord2;
    }
}

void Scrabble::placeLetter(const Clue& clue) {
    int rowIdx;
    if (clue.clueRow == WORD_1) {
        rowIdx = 0;
    } else {
        rowIdx = 1;
    }
    
    int pos = clue.cluePosition - 1; 
    int maxLen = rowLength(clue.clueRow);
    
    if (pos < 0 || pos >= maxLen) {
        throw InvalidPositionException("Position out of bounds.");
    }
    if (grid[rowIdx][pos].filled == true) {
        throw SlotFilledException("Slot already filled.");
    }
    
    grid[rowIdx][pos] = Tile(clue.clueLetter, true);
}

bool Scrabble::isRowComplete(WordRow row) const {
    int rowIdx;
    if (row == WORD_1) {
        rowIdx = 0;
    } else {
        rowIdx = 1;
    }
    
    bool isComplete = true;
    int maxLen = rowLength(row);
    
    for (int i = 0; i < maxLen; i++) {
        if (grid[rowIdx][i].filled == false) {
            isComplete = false;
        }
    }
    return isComplete;
}

string Scrabble::getRowString(WordRow row) const {
    int rowIdx = 0;
    if (row == WORD_2) {
        rowIdx = 1;
    }
    
    string result = "";
    int maxLen = rowLength(row);
    
    for (int i = 0; i < maxLen; i++) {
        if (grid[rowIdx][i].filled == true) {
            result += grid[rowIdx][i].tileLetter;
        }
    }
    return result;
}

bool Scrabble::checkWord(WordRow row, const string& guess) const {
    if (isRowComplete(row) == false) {
        throw WordIncompleteException("Row not filled.");
    }
    
    string correctWord = getTargetWord(row);
    if (guess == correctWord) {
        return true;
    } else {
        return false;
    }
}

void Scrabble::displayRow(WordRow row) const {
    int rowIdx = 0;
    if (row == WORD_2) rowIdx = 1;
    
    int maxLen = rowLength(row);
    for (int i = 0; i < maxLen; i++) {
        cout << grid[rowIdx][i] << " ";
    }
}

void Scrabble::displayBoard() const {
    cout << "Word 1: "; 
    displayRow(WORD_1);
    cout << endl;
    
    cout << "Word 2: "; 
    displayRow(WORD_2); 
    cout << endl;
}

ostream& operator<<(ostream& out, const Scrabble& board) {
    out << "Word 1: "; 
    for (int i = 0; i < board.WORD1_LENGTH; i++) {
        out << board.grid[0][i] << " ";
    }
    out << endl << "Word 2: "; 
    for (int j = 0; j < board.WORD2_LENGTH; j++) {
        out << board.grid[1][j] << " ";
    }
    return out;
}

istream& operator>>(istream& in, Scrabble& board) { 
    return in; 
}


// COTTAGE IMPLEMENTATION

Cottage::Cottage() { 
    this->locationCount = 0; 
    this->locations = new Location[LOCATION_COUNT]; 
}

Cottage::Cottage(const Cottage& other) {
    this->locationCount = other.locationCount;
    this->locations = new Location[LOCATION_COUNT];
    for (int i = 0; i < locationCount; i++) {
        this->locations[i] = other.locations[i];
    }
}

Cottage& Cottage::operator=(const Cottage& other) {
    if (this != &other) {
        this->locationCount = other.locationCount;
        for (int i = 0; i < locationCount; i++) {
            this->locations[i] = other.locations[i];
        }
    }
    return *this;
}

Cottage::~Cottage() { 
    delete[] locations; 
}

void Cottage::addLocation(const Location& loc) { 
    if (locationCount < LOCATION_COUNT) {
        locations[locationCount] = loc;
        locationCount++;
    }
}

void Cottage::clearLocations() { 
    locationCount = 0; 
}

int Cottage::getLocationCount() const { 
    return locationCount; 
}

const Location& Cottage::getLocation(int index) const { 
    return locations[index]; 
}

bool Cottage::isSearched(int index) const { 
    return locations[index].locSearchFlag; 
}

Clue Cottage::searchLocation(int index) {
    locations[index].locSearchFlag = true; // Mark as searched
    Clue foundClue(locations[index].letterClueLoc, locations[index].boardPosLoc, WORD_1);
    return foundClue;
}

Clue Cottage::revisitLocation(int index) const { 
    Clue foundClue(locations[index].letterClueLoc, locations[index].boardPosLoc, WORD_1);
    return foundClue;
}

void Cottage::displayLocations() const {
    for (int i = 0; i < locationCount; i++) {
        cout << "[" << i + 1 << "] " << locations[i].locName << " - ";
        if (locations[i].locSearchFlag == true) {
            cout << "Searched" << endl;
        } else {
            cout << "NOT Searched" << endl;
        }
    }
}



// Hanans Part 





// Kaleems Part 







// MODULE 12 & 13: SINGLETON GAME ENGINE & UI

GameEngine* GameEngine::instance = nullptr;

GameEngine::GameEngine() {
    this->state = STATE_menu;
    this->scoreBoard = nullptr;
    this->culpritIndex = 0;
    this->word1Solved = false;
    this->word2Solved = false;
    
    // Seed the random number generator
    srand(static_cast<unsigned int>(time(0)));
    
    // Initialize array to null
    for (int i = 0; i < PEOPLE_COUNT; i++) {
        people[i] = nullptr;
    }
    
    loadRules();
}

GameEngine::~GameEngine() { 
    clearCharacters(); 
}

GameEngine& GameEngine::getInstance() {
    if (instance == nullptr) {
        instance = new GameEngine();
    }
    return *instance;
}

void GameEngine::destroyInstance() { 
    if (instance != nullptr) {
        delete instance; 
        instance = nullptr; 
    }
}

void GameEngine::attachScoreBoard(ScoreBoard* board) { 
    this->scoreBoard = board; 
}

void GameEngine::clearCharacters() {
    for (int i = 0; i < PEOPLE_COUNT; i++) {
        if (people[i] != nullptr) { 
            delete people[i]; 
            people[i] = nullptr; 
        }
    }
}

void GameEngine::loadRules() { 
    bool success = rules.loadFromFile("rules.txt");
    if (success == false) {
        
    }
}

void GameEngine::pickCulprit() {

    culpritIndex = (rand() % SUSPECT_COUNT) + 1;
}

void GameEngine::loadClueWords() {
    
    scrabble.setTargetWords("LOOTERS", "HERITAGE"); 
}

void GameEngine::loadLocations() {
    cottage.clearLocations();
    cottage.addLocation(Location("Bedroom 1", 'L', 1));
    cottage.addLocation(Location("Bedroom 2", 'O', 2));
    cottage.addLocation(Location("Study Room", 'O', 3));
    cottage.addLocation(Location("Garden", 'T', 4));
    cottage.addLocation(Location("Kitchen", 'E', 5));
    cottage.addLocation(Location("Dining Room", 'R', 6));
    cottage.addLocation(Location("Living Room", 'S', 7));
}

void GameEngine::loadCharacters() {
    clearCharacters();
    
    // Instantiate each person
    people[0] = new Victim("Mr.Leonardo", "French", 'S', 1);
    people[1] = new Cook("Mr.Enzo", "Italian", 'P', 2);
    people[2] = new Guard("Mr.Lukas", "German", 'H', 3);
    people[3] = new Servant("Mr.Toby", "British", 'E', 4);
    people[4] = new OwnersChild("Ms.Celine", "French", 'R', 5, 17);
    people[5] = new Partner("Ms.Ruslana", "Ukrainian", 'I', 6, 1);
    people[6] = new Partner("Mr.Dmitry", "Russian", 'T', 7, 2);
    people[7] = new PA("Mr.Jad", "Egyptian", 'A', 8);
    
    people[culpritIndex]->setIsCulprit(true);
    culpritRole = people[culpritIndex]->getRole();
}

void GameEngine::resetGame(const string& detectiveName) {
    detective = Detective(detectiveName, rules.startingCredibility);
    pickCulprit();
    loadClueWords();
    loadLocations();
    loadCharacters();
    
    word1Solved = false;
    word2Solved = false;
    state = STATE_playing;
    
    cout << "A culprit has been secretly chosen. The game begins!" << endl;
}

int GameEngine::readInt(int low, int high) {
    int choice = 0;
    while (true) {
        cin >> choice;
        if (cin.fail() || choice < low || choice > high) {
            cin.clear(); 
            cin.ignore(10000, '\n');
            cout << "Invalid input. Enter " << low << "-" << high << ": ";
        } else {
            break; 
        }
    }
    return choice;
}

bool GameEngine::readYesNo(const string& prompt) {
    char inputChar;
    cout << prompt << " (Y/N): ";
    
    while (true) {
        cin >> inputChar;
        if (toupper(inputChar) == 'Y') {
            return true;
        } else if (toupper(inputChar) == 'N') {
            return false;
        } else {
            cout << "Invalid input. (Y/N): ";
        }
    }
}

string GameEngine::readDetectiveName() {
    string nameInput; 
    cout << "Enter detective name: "; 
    cin >> nameInput; 
    return nameInput;
}

void GameEngine::showMainMenu() const {
    cout << endl << "=== THE LOCKER HEIST - MAIN MENU ===" << endl;
    cout << "[1] New Game" << endl;
    cout << "[2] View Leader/Score Board" << endl;
    cout << "[3] About" << endl;
    cout << "[4] Rules" << endl;
    cout << "[5] Exit" << endl;
    cout << "Choice: ";
}

void GameEngine::showGameMenu() const {
    cout << endl << "==== Game Menu ====" << endl;
    cout << "[1] Visit a Location" << endl;
    cout << "[2] Interview a Person" << endl;
    cout << "[3] View Scrabble Board (free)" << endl;
    cout << "[4] Form a Word" << endl;
    cout << "[5] Accuse a Suspect" << endl;
    cout << "[6] Quit" << endl;
    cout << "TURN " << detective.getTurns() + 1 << endl;
    cout << "Choice: ";
}

void GameEngine::showStatus() const {
    cout << endl << "Credibility: " << detective.getCredibility() << " | Turns used: " << detective.getTurns() << endl;
    cout << "Rack Word 1: " << detective.getRack(WORD_1) << endl;
    cout << "Rack Word 2: " << detective.getRack(WORD_2) << endl;
    cout << scrabble << endl;
}

void GameEngine::showAbout() const {
    cout << endl << "=== ABOUT THE LOCKER HEIST ===" << endl;
    cout << "A detective OOP game developed for a university project." << endl;
}

void GameEngine::handleMainMenu() {
    showMainMenu();
    int choice = readInt(1, 5);
    
    if (choice == MENU_new_game) {
        string name = readDetectiveName();
        resetGame(name);
    } else if (choice == MENU_scoreboard) {
        if (scoreBoard != nullptr) {
            scoreBoard->displayAll();
        }
    } else if (choice == MENU_about) {
        showAbout();
    } else if (choice == MENU_rules) {
        rules.display();
    } else if (choice == MENU_exit) {
        state = STATE_exit;
    }
}

void GameEngine::spendTurn() {
    detective.useTurn();
    checkEndConditions();
}

void GameEngine::visitLocation() {
    cout << endl << "====== Location Menu ======" << endl;
    cottage.displayLocations();
    cout << "Pick: ";
    
    int choice = readInt(1, Cottage::LOCATION_COUNT) - 1;
    
    try {
        if (cottage.isSearched(choice) == false) {
            bool searchChoice = readYesNo("You have NOT Searched this location yet. Search?");
            if (searchChoice == true) {
                Clue c = cottage.searchLocation(choice);
                cout << "Letter " << c.clueLetter << " collected at position " << c.cluePosition << " of Word 1." << endl;
                scrabble.placeLetter(c);
                detective.collectClue(c);
            }
        } else {
            bool searchAgain = readYesNo("Location already searched. Search again?");
            if (searchAgain == true) {
                Clue c = cottage.revisitLocation(choice);
                cout << "Letter " << c.clueLetter << " was at position " << c.cluePosition << " of Word 1." << endl;
            }
        }
    } catch (const GameException& e) { 
        handleException(e); 
        detective.loseCredibility(abs(rules.wrongTurnPenalty)); 
    }
    
    spendTurn(); 
}

void GameEngine::interviewPerson() {
    cout << endl << "====== Character Menu ======" << endl;
    
    for (int i = 0; i < PEOPLE_COUNT; i++) {
        cout << "[" << i + 1 << "] " << people[i]->getName() << " (" << people[i]->getRole() << ") - ";
        if (people[i]->isInterviewed()) {
            cout << "Interviewed";
        } else {
            cout << "NOT Interviewed";
        }
        cout << " [Min Cred: " << people[i]->getMinCredibility() << "]" << endl;
    }
    
    cout << "Pick: ";
    int choice = readInt(1, PEOPLE_COUNT) - 1;
    
    try {
        if (people[choice]->canInterview(detective.getCredibility()) == false) {
            throw LockedCharacterException("Credibility too low to interview this person.");
        }
        
        if (people[choice]->isInterviewed() == false) {
            bool wantInterview = readYesNo("You have NOT Interviewed them yet. Interview?");
            if (wantInterview == true) {
                people[choice]->markInterviewed();
                Clue c = people[choice]->getClue();
                cout << "Letter " << c.clueLetter << " collected at position " << c.cluePosition << " of Word 2." << endl;
                scrabble.placeLetter(c);
                detective.collectClue(c);
            }
        } else {
            bool interviewAgain = readYesNo("Character already interviewed. Interview again?");
            if (interviewAgain == true) {
                Clue c = people[choice]->getClue();
                cout << "Letter " << c.clueLetter << " was at position " << c.cluePosition << " of Word 2." << endl;
            }
        }
    } catch (const GameException& e) { 
        handleException(e); 
        detective.loseCredibility(abs(rules.wrongTurnPenalty)); 
    }
    
    spendTurn();
}

void GameEngine::viewBoard() const {
    scrabble.displayBoard();

}

void GameEngine::formWord() {
    cout << "[1] Word 1" << endl;
    cout << "[2] Word 2" << endl;
    cout << "Pick Row: ";
    
    int rowChoice = readInt(1, 2);
    WordRow row;
    if (rowChoice == 1) {
        row = WORD_1;
    } else {
        row = WORD_2;
    }
    
    try {
        if (scrabble.isRowComplete(row) == false) {
            throw WordIncompleteException("Row not completely filled.");
        }
        
        string guess;
        cout << "Enter guessed word: ";
        cin >> guess;
        
        for (size_t i = 0; i < guess.length(); ++i) {
            guess[i] = toupper(guess[i]);
        }
        
        if (scrabble.checkWord(row, guess) == true) {
            cout << "Correct! Word solved." << endl;
            if (row == WORD_1) {
                word1Solved = true;
            } else if (row == WORD_2) {
                word2Solved = true;
            }
        } else {
            cout << "Incorrect word! Penalty applied." << endl;
            detective.loseCredibility(abs(rules.wrongWordPenalty));
        }
    } catch (const GameException& e) { 
        handleException(e); 
    }
    
    spendTurn();
}

void GameEngine::accuseSuspect() {
    cout << endl << "====== Accuse Menu ======" << endl;
    for (int i = 1; i < PEOPLE_COUNT; i++) { 
        cout << "[" << i << "] " << people[i]->getName() << " (" << people[i]->getRole() << ")" << endl;
    }
    
    cout << "Accuse: ";
    int choice = readInt(1, PEOPLE_COUNT - 1);
    
    if (people[choice]->getIsCulprit() == true) {
        cout << people[choice]->getName() << " is the culprit!" << endl;
        state = STATE_won;
    } else {
        cout << "Wrong accusation!" << endl;
        detective.loseCredibility(detective.getCredibility()); 
        state = STATE_lost;
    }
    
    finishGame();
}

void GameEngine::quitGame() { 
    state = STATE_menu; 
}

void GameEngine::checkEndConditions() {
    if (detective.getCredibility() <= 0 || detective.getTurns() >= rules.maxTurns) {
        state = STATE_lost;
        finishGame();
    }
}

void GameEngine::finishGame() {
    cout << endl << detective.getName();
    if (state == STATE_won) {
        cout << " WIN!" << endl;
    } else {
        cout << " LOSE!" << endl;
    }
    
    cout << "Final Credibility: " << detective.getCredibility() << endl;
    cout << "Turns used: " << detective.getTurns() << endl;
    cout << "Culprit was: " << culpritRole << endl;
         
    if (scoreBoard != nullptr) {
        string resultStr = "";
        if (state == STATE_won) {
            resultStr = "WIN";
        } else {
            resultStr = "LOSE";
        }
        
        int turnsLeft = rules.maxTurns - detective.getTurns();
        scoreBoard->addScore(detective.getName(), resultStr, detective.getCredibility(), turnsLeft, culpritRole);
    }
    
    state = STATE_menu; 
}

void GameEngine::handleException(const GameException& e) {
    cout << "ERROR: " << e.what() << endl;
}

GameState GameEngine::getState() const { 
    return state; 
}

void GameEngine::step() {
    // Main state machine for the game
    if (state == STATE_menu) {
        handleMainMenu();
    } 
    else if (state == STATE_playing) {
        showStatus();
        showGameMenu();
        
        int choice = readInt(1, 6);
        
        if (choice == 1) {
            visitLocation();
        } else if (choice == 2) {
            interviewPerson();
        } else if (choice == 3) {
            viewBoard(); 
        } else if (choice == 4) {
            formWord();
        } else if (choice == 5) {
            accuseSuspect();
        } else if (choice == 6) {
            quitGame();
        }
    }
}