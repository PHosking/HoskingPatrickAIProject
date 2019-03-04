#ifndef USERINPUT_CPP
#define USERINPUT_CPP
#include "UserInput.h"

UserInput::UserInput() {
	inputHandle = GetStdHandle(STD_INPUT_HANDLE);
	outputHandle = GetStdHandle(STD_OUTPUT_HANDLE);
}

void UserInput::SetConsoleCursorVisibility(bool visible) {
	if (outputHandle) {
		//Obtain the cursor information, change cursor visibility, and apply the changes
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(outputHandle, &cursorInfo);
		cursorInfo.bVisible = visible;
		SetConsoleCursorInfo(outputHandle, &cursorInfo);
	}
}

bool UserInput::GetConsoleCursorVisibility() {
	if (outputHandle) {
		//Obtain the cursorInfo for the cursor visibilty
		CONSOLE_CURSOR_INFO cursorInfo;
		GetConsoleCursorInfo(outputHandle, &cursorInfo);
		return cursorInfo.bVisible;
	}

	//If the cursor hasn't been modified, it's assumed it's visible
	return true;
}

/* Summary: Outputs a prompt, as well as X responses, where X is the size of <responses>.
				Uses the arrow keys to choose a response.
				Returns: INT based on chosen response's element position in <response> vector */
int UserInput::PromptUserMenu(std::string prompt, std::vector<std::string> responses) {
	//Disables cursor visibility at the start for aesthetics
	bool cursorVisibleAtStart = GetConsoleCursorVisibility();
	if (cursorVisibleAtStart)
		SetConsoleCursorVisibility(false);

	int chosenResponse = 0; //The response the user chose
	std::cout << std::endl << prompt << std::endl; //Output the prompt to the user

	//A list of coordinates to reference for moving the "cursor"
	std::vector<COORD> responseCoordinates = std::vector<COORD>();

	for (int x = 0; x < responses.size(); x++) {
		//Obtain and store the cursor position for later reference
		CONSOLE_SCREEN_BUFFER_INFO newCoord;
		GetConsoleScreenBufferInfo(outputHandle, &newCoord);
		responseCoordinates.push_back(newCoord.dwCursorPosition);

		//Outputs either the cursor, or spaces to fill the cursors absence, and then the respetive response.
		std::cout << ((x == 0) ? menuCursor : std::string(menuCursor.length(), ' ')) << " " << responses[x] << std::endl;
	}

	bool done = false;
	while (!done) {
		//Waits for a key press while not outputing any message. Cursor should be disabled to not have it look weird.
		system("pause>nul");

		//Both up and down move the menuCursor up or down, while keeping track of the chosen response
		if (GetAsyncKeyState(VK_UP)) {
			MoveCursor(responseCoordinates[chosenResponse].X, responseCoordinates[chosenResponse].Y);

			//output spaces where the cursor should have been, essentially 'erasing' it
			std::cout << std::string(menuCursor.length(), ' ');

			if (chosenResponse <= 0) {
				chosenResponse = responses.size() - 1;
			} else {
				chosenResponse--;
			}

			MoveCursor(responseCoordinates[chosenResponse].X, responseCoordinates[chosenResponse].Y);
			std::cout << menuCursor;

		} else if (GetAsyncKeyState(VK_DOWN)) {
			MoveCursor(responseCoordinates[chosenResponse].X, responseCoordinates[chosenResponse].Y);

			//output spaces where the cursor should have been, essentially 'erasing' it
			std::cout << std::string(menuCursor.length(), ' ');

			if (chosenResponse >= responses.size() - 1) {
				chosenResponse = 0;
			} else {
				chosenResponse++;
			}

			MoveCursor(responseCoordinates[chosenResponse].X, responseCoordinates[chosenResponse].Y);
			std::cout << menuCursor;

		} else if (GetAsyncKeyState(VK_SPACE) || GetAsyncKeyState(VK_RETURN)) {
			done = true;
		}
	}

	//re-enable the cursor if necessary
	if (cursorVisibleAtStart)
		SetConsoleCursorVisibility(true);

	return chosenResponse;
}

/* Return: -999999999 if forcefully exiting the function */
int UserInput::PromptUserInputInt(std::string prompt, std::string errorPrompt) {
	//If the cursor is visible at the start, keep track of that
	bool cursorVisibleAtStart = GetConsoleCursorVisibility();
	if(!cursorVisibleAtStart)
		SetConsoleCursorVisibility(true);

	//Keep the user in an infinite loop
	for(;;) {
		try {
			std::cout << std::endl << prompt << std::endl << " > "; //Prompt the user with the prompt

			std::string response;
			std::getline(std::cin, response); //Get the entire line, including all spaces, etc.

			std::transform(response.begin(), response.end(), response.begin(), ::tolower); //changes string to lowercase for comparisons

			//exit clause to exit a loop if stuck
			if (response == "exit" || response == "quit") {
				if (!cursorVisibleAtStart)
					SetConsoleCursorVisibility(true);

				return -999999999;
			}

			int intResponse = std::stoi(response); //change string input to int. If this errors, input was not an int, loop back around

			//Re-enable cursor if disabled at start
			if (!cursorVisibleAtStart)
				SetConsoleCursorVisibility(true);

			return intResponse;
		} catch(std::invalid_argument){ //Error thrown by std::stoi
			std::cout << std::endl << errorPrompt << std::endl; //simply output the error message and keep moving
		}
	}

	//Re-enable cursor if disabled at start
	if (!cursorVisibleAtStart)
		SetConsoleCursorVisibility(true);

	return -999999999;
}

/* Summary: Outputs an interactive GUI for the user to edit a grid. Function ends once the ESCAPE key is pressed,
	returning the user. */
bool UserInput::PromptGridInteractionGUI() {
	system("cls"); //Clear the screen before starting

	//Disables cursor visibility at the start for aesthetics
	bool cursorVisibleAtStart = GetConsoleCursorVisibility();
	if (cursorVisibleAtStart)
		SetConsoleCursorVisibility(false);

	Grid grid = Grid(10,10); //Instantiate a new 10x10 grid
	COORD gridOrigin = GetConsoleCursorCoord(); //Keeps track of where to move the cursor for outputing grid characters

	COORD tileStat; //Reference coordinate 

	COORD cursorGridPosition; //The grid position of the cursor, constrained to the bounds of the grid
	cursorGridPosition.X = 0;
	cursorGridPosition.Y = 0;

	COORD searchOutputLocation; //The location of the cursor for outputing the diagnostics for search alogirthms

	bool reprintGrid = true; //If true, will clear the screen and re-output the grid (used for making drastic changes to the grid)
	bool done = false; //If true, exits the loop and returns true

	while (!done) {
		//If reprintGrid is true, it will reprint the entire grid. Used for resizing, or clearing the screen.
		if (reprintGrid) {
			reprintGrid = false;
			system("cls");

			gridOrigin = GetConsoleCursorCoord(); //Grab the cursor position right before
			grid.OutputGrid();						//the grid is printed

			//Output the tile stats, and grab the tileStat position for later updating.
			std::cout << "CURRENT TILE: ";
			tileStat = GetConsoleCursorCoord();
			std::cout << "(" << gridOrigin.X << ", " << gridOrigin.Y << ") | Type: " <<
				((grid.GetCell(0, 0).tileType == Tile::wall) ? "Wall" : "Floor")
				<< " | " << ((grid.GetCell(0, 0).goalCell) ? "Goal  " : ((grid.GetCell(0, 0).startCell) ? "Start " : "-----  ")) << std::endl;

			//Output the commands to the user
			std::cout << std::endl << "ARROW KEYS: Move cursor\nSPACE or ENTER: Toggle floor / wall (obstacle)\n"
				<<"S: Set start position\nG: Set goal position\nR: Resize the grid (clears all placed objects)\n"
				<< "D: Run a depth-first search on the current grid\nB: Run a breadth-first search on the current grid\n"
				<< "Y: Run a greedy search on the current grid\nA: Run an A* search on the current grid\n"
				<<"K: Add randomized walls\nV: Toggle cell visitation output (Currently: "<<(std::string)(grid.GetDisplayAllTraversedCells()?"ENABLED":"DISABLED")<<")\n"
				<<"ESCAPE: EXIT this interaction and return to Main Menu" << std::endl;

			//Grab the cursor position after everything is printed. This is used to print diagnostics on the search algorithms.
			searchOutputLocation = GetConsoleCursorCoord();

			//Set cursorPosition back to (0, 0)
			//cursorGridPosition.X = 0;
			//cursorGridPosition.Y = 0;

			//Make the cyan-cursor visible on the newly-printed grid
			OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
				GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 11);

			SetConsoleColor(7); //Make sure the color of the text is back at grey
		}
		//Disables input until necessary. Cursor should be disabled to not have it look weird.
		system("pause>nul");

		if (GetAsyncKeyState(VK_UP)) { //Up arrow key
			if (cursorGridPosition.Y - 1 >= 0) {
				//Reprint the character at the current position
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 7);

				cursorGridPosition.Y--; //Update to the new position

				//Output the same tile, but with the cyan-cursor overlay
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(VK_DOWN)) { //Down arrow key
			if (cursorGridPosition.Y + 1 <= grid.GetGridY() - 1) {
				//Reprint the character at the current position
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 7);

				cursorGridPosition.Y++; //Update to the new position

				//Output the same tile, but with the cyan-cursor overlay
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(VK_LEFT)) { //Left arrow key
			if (cursorGridPosition.X - 1 >= 0) {
				//Reprint the character at the current position
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 7);

				cursorGridPosition.X--; //Update to the new position

				//Output the same tile, but with the cyan-cursor overlay
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(VK_RIGHT)) { //Right arrow key
			if (cursorGridPosition.X + 1 <= grid.GetGridX() - 1) {
				//Reprint the character at the current position
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 7);

				cursorGridPosition.X++; //Update to the new position

				//Output the same tile, but with the cyan-cursor overlay
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					GetCharTileAtPosition(cursorGridPosition.X, cursorGridPosition.Y, grid), 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(VK_SPACE) & 0x8000 || GetAsyncKeyState(VK_RETURN) & 0x8000) { //Space or enter key
			//If the cursor is in bounds
			if (cursorGridPosition.X > 0 || cursorGridPosition.X < grid.GetGridX() - 2 || cursorGridPosition.Y > 0 || cursorGridPosition.Y < grid.GetGridY() - 2) {

				//Toggle the wall to a floor, and vise versa
				if (grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).tileType != Tile::wall) {
					grid.SetCell(cursorGridPosition.X, cursorGridPosition.Y, Tile::wall);
				} else if (grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).tileType != Tile::floor) {
					grid.SetCell(cursorGridPosition.X, cursorGridPosition.Y, Tile::floor);
				}

				//Output the new character where the cursor is, in the cursor's color
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					(char)(grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).tileType), 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(83) || GetAsyncKeyState(115)) { //S or s key
			if (cursorGridPosition.X > 0 || cursorGridPosition.X < grid.GetGridX() - 2 || cursorGridPosition.Y > 0 || cursorGridPosition.Y < grid.GetGridY() - 2) {
				//Set the previous start positions character to floor or wall, based on that locations tileType
				OutputNewCharacter(gridOrigin.X + grid.GetCell(grid.GetStartPos()).x, gridOrigin.Y + grid.GetCell(grid.GetStartPos()).y,
					GetCharTileAtPosition(grid.GetStartPos().x, grid.GetStartPos().y, grid, true), 7);

				grid.SetStartPos(cursorGridPosition.X, cursorGridPosition.Y); //Sets the new start position

				//Outputs the start character to the new location
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					(char)Tile::start, 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(71) || GetAsyncKeyState(103)) { //G or g key
			if (cursorGridPosition.X > 0 || cursorGridPosition.X < grid.GetGridX() - 2 || cursorGridPosition.Y > 0 || cursorGridPosition.Y < grid.GetGridY() - 2) {
				//Set the previous start positions character to floor or wall, based on that locations tileType
				OutputNewCharacter(gridOrigin.X + grid.GetCell(grid.GetGoalPos()).x, gridOrigin.Y + grid.GetCell(grid.GetGoalPos()).y,
					GetCharTileAtPosition(grid.GetGoalPos().x, grid.GetGoalPos().y, grid, true), 7);

				grid.SetGoalPos(cursorGridPosition.X, cursorGridPosition.Y); //Sets the new start position

				//Outputs the start character to the new location
				OutputNewCharacter(gridOrigin.X + cursorGridPosition.X, gridOrigin.Y + cursorGridPosition.Y,
					(char)Tile::goal, 11);

				SetConsoleColor(7); //reset the color
			}

		} else if (GetAsyncKeyState(82) || GetAsyncKeyState(114)) { //R or r key
			system("cls");
			bool decidingSize = true;
			while (decidingSize) {
				//Output the prompt, and obtain the new width and height ints
				std::cout << "Resizing grid. Please keep input between 3 and 100, inclusively.\n\n";
				int width = PromptUserInputInt("Enter the desired WIDTH of the grid:", "Please enter an integer...");
				int height = PromptUserInputInt("Enter the desired HEIGHT of the grid:", "Please enter an integer...");

				//constrain the bounds of the grid, and loop back if necessary
				if (width < 3 || height < 3 || width > 100 || height > 100) {
					std::cout << "You entered an invalid number for either the HEIGHT or WIDTH.\nPlease constrain your values to anything between 3 or 100!" << std::endl;
					system("pause");
					system("cls");
					continue;
				}

				//Ask them if they want to confirm, redo, cancel, or exit to main menu
				int answer = PromptUserMenu(("Are you sure you want a new grid with the dimensions (" + std::to_string(width) + ", " + std::to_string(height) + ")?"), 
					std::vector<std::string>{"Yes!", "No.","Cancel", "Exit to Main Menu"});

				switch (answer) {
				case 0: //confirm, resizeGrid, update start and goal, and flag correct flags
					grid.ResizeGrid(width, height);
					grid.SetRandomStartGoal();
					cursorGridPosition.X = 0;
					cursorGridPosition.Y = 0;
					reprintGrid = true;
					decidingSize = false;
					break;
				case 1: //no, they weren't right, try again
					system("cls");
					break;
				case 2: //cancel, don't change anything
					reprintGrid = true;
					decidingSize = false;
					break;
				case 3: //exit to main menu
					done = true;
					decidingSize = false;
					break;
				default:
					break;
				}
			}

			SetConsoleCursorVisibility(false); //Make sure the cursor is disabled

		} else if (GetAsyncKeyState(68) || GetAsyncKeyState(100)) { //D or d key
			MoveCursor(searchOutputLocation.X, searchOutputLocation.Y);

			SetConsoleColor(14);
			std::cout << "\nTesting DEPTH-FIRST Search:\n";
			SetConsoleColor(7);
			std::cout << "Cell Visitation Order:\n";

			//Obtain the path to the goal using a depth-first search, and output traversed nodes
			std::vector<Cell> path = grid.DepthFirstSearch();
			if (path.size() > 0) { //path found
				std::cout << "\n\nPath Found!\nPath Length: " << path.size() << "\nPath: ";

				//Output the path to the user
				for (int x = 0;x < path.size();x++) {
					std::cout << "(" << path[x].x << ", " << path[x].y << ((x == path.size() - 1) ? ")[Goal]\n\n" : ") -> ");
				}

				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";

				//outputs dashes signifying the path in the grid
				for (int x = 0;x < path.size() - 1;x++) {
					OutputNewCharacter(gridOrigin.X + path[x].x, gridOrigin.Y + path[x].y, '-', 10);
				}
			} else { //path not found
				std::cout << "\n\nUnable to find a path to goal...\n";

				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";
			}

			//reset color value, wait for user input, and flag for reprinting the grid to clear diagnostics
			SetConsoleColor(7);
			system("pause>nul");
			reprintGrid = true;

		} else if (GetAsyncKeyState(66) || GetAsyncKeyState(98)) { //B or b key
			MoveCursor(searchOutputLocation.X, searchOutputLocation.Y);

			SetConsoleColor(14);
			std::cout << "\nTesting BREADTH-FIRST Search:\n";
			SetConsoleColor(7);
			std::cout << "Cell Visitation Order:\n";

			//Obtain the path to the goal using a breadth-first search, and output traversed nodes
			std::vector<Cell> path = grid.BreadthFirstSearch();
			if (path.size() > 0) { //path found
				std::cout << "\n\nPath Found!\nPath Length: " << path.size() << "\nPath: ";

				//Output the path to the user
				for (int x = 0;x < path.size();x++) {
					std::cout << "(" << path[x].x << ", " << path[x].y << ((x == path.size() - 1) ? ")[Goal]\n\n" : ") -> ");
				}

				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";

				//outputs dashes signifying the path in the grid
				for (int x = 0;x < path.size() - 1;x++) {
					OutputNewCharacter(gridOrigin.X + path[x].x, gridOrigin.Y + path[x].y, '-', 10);
				}
			} else { //path not found
				std::cout << "\n\nUnable to find a path to goal...\n";
				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";
			}

			//reset color value, wait for user input, and flag for reprinting the grid to clear diagnostics
			SetConsoleColor(7);
			system("pause>nul");
			reprintGrid = true;

		} else if (GetAsyncKeyState(89) || GetAsyncKeyState(121)) { //Y or y key
			MoveCursor(searchOutputLocation.X, searchOutputLocation.Y);

			SetConsoleColor(14);
			std::cout << "\nTesting GREEDY Search:\n";
			SetConsoleColor(7);
			std::cout << "Cell Visitation Order:\n";

			//Obtain the path to the goal using a breadth-first search, and output traversed nodes
			std::vector<Cell> path = grid.GreedySearch();
			if (path.size() > 0) { //path found
				std::cout << "\n\nPath Found!\nPath Length: " << path.size() << "\nPath: ";

				//Output the path to the user
				for (int x = 0;x < path.size();x++) {
					std::cout << "(" << path[x].x << ", " << path[x].y << ((x == path.size() - 1) ? ")[Goal]\n\n" : ") -> ");
				}

				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";

				//outputs dashes signifying the path in the grid
				for (int x = 0;x < path.size() - 1;x++) {
					OutputNewCharacter(gridOrigin.X + path[x].x, gridOrigin.Y + path[x].y, '-', 10);
				}
			} else { //path not found
				std::cout << "\n\nUnable to find a path to goal...\n";
				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";
			}

			//reset color value, wait for user input, and flag for reprinting the grid to clear diagnostics
			SetConsoleColor(7);
			system("pause>nul");
			reprintGrid = true;

		} else if (GetAsyncKeyState(65) || GetAsyncKeyState(97)) { //A or a key
			MoveCursor(searchOutputLocation.X, searchOutputLocation.Y);

			SetConsoleColor(14);
			std::cout << "\nTesting A* Search:\n";
			SetConsoleColor(7);
			std::cout << "Cell Visitation Order:\n";

			//Obtain the path to the goal using a breadth-first search, and output traversed nodes
			std::vector<Cell> path = grid.AStarSearch();
			if (path.size() > 0) { //path found
				std::cout << "\n\nPath Found!\nPath Length: " << path.size() << "\nPath: ";

				//Output the path to the user
				for (int x = 0;x < path.size();x++) {
					std::cout << "(" << path[x].x << ", " << path[x].y << ((x == path.size() - 1) ? ")[Goal]\n\n" : ") -> ");
				}

				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";

				//outputs dashes signifying the path in the grid
				for (int x = 0;x < path.size() - 1;x++) {
					OutputNewCharacter(gridOrigin.X + path[x].x, gridOrigin.Y + path[x].y, '-', 10);
				}
			} else { //path not found
				std::cout << "\n\nUnable to find a path to goal...\n";
				SetConsoleColor(12);
				std::cout << "PRESS ANY KEY TO CONTINUE";
			}

			//reset color value, wait for user input, and flag for reprinting the grid to clear diagnostics
			SetConsoleColor(7);
			system("pause>nul");
			reprintGrid = true;

		} else if (GetAsyncKeyState(75) || GetAsyncKeyState(107)) { //K or k key
			grid.PepperWalls();
			reprintGrid = true;
		} else if (GetAsyncKeyState(86) || GetAsyncKeyState(118)) {//V or v key
			system("cls");
			int response = PromptUserMenu("Do you want to " + (std::string)(grid.GetDisplayAllTraversedCells() ? "DISABLE" : "ENABLE") + " displaying all traversed cells in detail?",
				std::vector<std::string>{ "Yes", "No" });

			//Simply figure out if we need to enable or disable displaying all traversed cells
			if ((grid.GetDisplayAllTraversedCells() && response == 0) || (!grid.GetDisplayAllTraversedCells() && response == 1)) {
				grid.SetDisplayAllTraversedCells(false);
			} else if ((!grid.GetDisplayAllTraversedCells() && response == 0) || (grid.GetDisplayAllTraversedCells() && response == 1)) {
				grid.SetDisplayAllTraversedCells(true);
			}

			reprintGrid = true; //make sure we reprint the grind after going into a prompt
		} else if (GetAsyncKeyState(VK_ESCAPE)) {
			done = true; //Get out of this grid interaction
		}

		//Always make sure the cursor is disabled while interacting via mouse keys
		SetConsoleCursorVisibility(false);

		//reprint the tile statistics
		MoveCursor(tileStat.X, tileStat.Y);
		std::cout << "(" << cursorGridPosition.X << ", " << cursorGridPosition.Y << ") | Type: " <<
			((grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).tileType == Tile::wall) ? "Wall" : "Floor")
			<< " | " << ((grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).goalCell) ? "Goal    " : ((grid.GetCell(cursorGridPosition.X, cursorGridPosition.Y).startCell) ? "Start  " : "-----   "));
	} //end while(!done)

	//re-enable the cursor if necessary
	if (cursorVisibleAtStart)
		SetConsoleCursorVisibility(true);

	return true; //exited successfully
}

void UserInput::MoveCursor(int x, int y) {
	cursorCoord.X = x;
	cursorCoord.Y = y;
	SetConsoleCursorPosition(outputHandle, cursorCoord);
}

COORD UserInput::GetConsoleCursorCoord() {
	CONSOLE_SCREEN_BUFFER_INFO gridOriginInfo;
	GetConsoleScreenBufferInfo(outputHandle, &gridOriginInfo);

	return gridOriginInfo.dwCursorPosition;
}

void UserInput::OutputNewCharacter(int x, int y, char newChar, int newColor) {
	SetConsoleColor(newColor);
	MoveCursor(x, y);
	std::cout << newChar;
}

char UserInput::GetCharTileAtPosition(int x, int y, Grid grid, bool disregardStartGoal) {
	//If flagged, will return the goal or start tile regardless
	if (!disregardStartGoal) {
		return ((grid.GetCell(x, y).goalCell) ? (char)Tile::goal
			: ((grid.GetCell(x, y).startCell) ? (char)Tile::start
				: (char)(grid.GetCell(x, y).tileType)));
	} else {
		return (char)(grid.GetCell(x, y).tileType);
	}
};

void UserInput::SetConsoleColor(int color) {
	if (outputHandle) {
		SetConsoleTextAttribute(outputHandle, color);
	}
}
#endif