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
