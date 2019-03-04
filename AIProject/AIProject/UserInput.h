#ifndef USERINPUT_H
#define USERINPUT_H

#include "Grid.h"
#include <Windows.h>
#include <vector>
#include <string>
#include <iostream>
#include <limits.h>

/*
Handles any user input, from strings to arrow keys.
WARNING: This currently only supports Windows console. No support or testing has been done on
	unix or mac based enviroments. Tested on Windows 10, 7.
*/
class UserInput {
public:
	UserInput(); /* Initializes the input and output handles */

	void SetConsoleCursorVisibility(bool visible); /* Enables or disables the console cursor. Does not disable input. */
	bool GetConsoleCursorVisibility(); /* Returns the visibility of the cursor currently */

	int PromptUserMenu(std::string prompt, std::vector<std::string> responses); /* Creates a prompt for a user to reply to with arrow keys. Response chose is returned as an int. */
	int PromptUserInputInt(std::string prompt, std::string errorPrompt); /* Prompts the user to enter an integer. Will catch errors until they type an integer, or press escape. */

	bool PromptGridInteractionGUI(); /* Displays an interactable GUI for the user to edit and test with. */

	void MoveCursor(int x, int y); /* Move the cursor to (x, y) on the console screen */
	COORD GetConsoleCursorCoord(); /* Returns the COORD of the console cursor */

	void OutputNewCharacter(int x, int y, char newChar, int newColor=7); /* Set color and output a new one */
	char GetCharTileAtPosition(int x, int y, Grid grid, bool disregardStartGoal=false); /* Returns the Tile that is at the current (x, y) coordinate as a char */

	void SetConsoleColor(int color); /* Sets the text color to a specific color */
private:
	HANDLE inputHandle; /* The current console's input handle */
	HANDLE outputHandle; /* The current console's output handle */
	COORD cursorCoord; /* The current coordinates of the text cursor in the console */
	std::string menuCursor = ">>"; /* The style of cursor to use for */
};

#endif