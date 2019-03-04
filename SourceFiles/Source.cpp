#include "Grid.h"
#include "UserInput.h"

int main() {
	srand(time(NULL));

	//Create a way for the user to input data, and set the cursor visibility to false
	UserInput userInput = UserInput();
	userInput.SetConsoleCursorVisibility(false);

	bool out = true;
	while (out) {
		system("cls");
		
		//Prompt the user with an arrow-key driven menu, asking to start the the project or exit the scene.
		int selection = userInput.PromptUserMenu("Patrick Hosking's AI Assignment 2.\nImplementation of Greedy and A* search algorithms.\n\nFor some menus you will use ARROW KEYS with SPACE or RETURN to confirm selection.\n\nPlease choose a selection...",
			std::vector<std::string>{"Run Scenario", "Exit Application"});

		switch (selection) {
		case 0:
			userInput.PromptGridInteractionGUI();
			break;
		case 1:
			out = false;
			break;
		default:
			break;
		}
	}

	system("cls");
	system("pause");
	return 0;
}