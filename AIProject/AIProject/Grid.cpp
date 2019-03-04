#ifndef GRID_CPP
#define GRID_CPP
#include "Grid.h"

Grid::Grid() {
	ResizeGrid(10);
	SetRandomStartGoal();
}

Grid::Grid(int x) {
	ResizeGrid(x);
	SetRandomStartGoal();
}

Grid::Grid(int x, int y) {
	ResizeGrid(x, y);
	SetRandomStartGoal();
}

void Grid::ResizeGrid(int x) {
	ResizeGrid(x, x);
}

void Grid::ResizeGrid(int x, int y) {
	gridSizeX = x;
	gridSizeY = y;

	//Clear and resize the vectors to the appropriate length
	grid.clear();
	grid.resize(x);

	for (int i = 0; i < x; i++) {
		//Resize the child vectors to the appropriate lengths
		grid[i].resize(y);

		for (int j = 0; j < y; j++) {
			//Fill the cells with new Cell classes, with correct positioning
			grid[i][j] = Cell(i, j);

			//For aesthetics, make all the cells on the outter edge a wall
			if (i == 0 || i == x - 1 || j == 0 || j == y - 1)
				SetCell(i, j, Tile::wall);
			else
				SetCell(i, j, Tile::floor);
		}
	}
}

void Grid::SetRandomStartGoal() {
	//Constrain random position to be inside of the grid walls by 1 cell
	startPos = Cell((rand() % (gridSizeX - 2)) + 1, (rand() % (gridSizeY - 2)) + 1);
	goalPos = Cell((rand() % (gridSizeX - 2)) + 1, (rand() % (gridSizeY - 2)) + 1);

	//Make sure goal position is not start position
	while (startPos.x == goalPos.x && startPos.y == goalPos.y) {
		goalPos = Cell((rand() % (gridSizeX - 2)) + 1, (rand() % (gridSizeY - 2)) + 1);
	}

	//Make sure the tiles know they're start and end positions
	grid[startPos.x][startPos.y].startCell = true;
	grid[goalPos.x][goalPos.y].goalCell = true;
}

bool Grid::SetCell(int x, int y, Tile tile) {
	if (x<0 || x>gridSizeX || y<0 || y>gridSizeY) //If cell is outside of boundaries, don't set it
		return false;

	grid[x][y].tileType = tile;

	return true;
}

bool Grid::SetCell(Cell cell, Tile tile) {
	if (cell.x<0 || cell.x>gridSizeX || cell.y<0 || cell.y>gridSizeY) //If cell is outside of boundaries, don't set it
		return false;

	grid[cell.x][cell.y].tileType = cell.tileType;

	return true;
}

Cell Grid::GetCell(int x, int y) {
	if (x<0 || x>gridSizeX || y<0 || y>gridSizeY) //If cell is outside of boundaries, return a null cell
		return Cell();

	return grid[x][y];
}

Cell Grid::GetCell(Cell cell) {
	if (cell.x<0 || cell.x>gridSizeX || cell.y<0 || cell.y>gridSizeY) //If cell is outside of boundaries, return a null cell
		return Cell();

	return grid[cell.x][cell.y];
}

int Grid::GetGridX() {
	return gridSizeX;
}

int Grid::GetGridY() {
	return gridSizeY;
}

Cell Grid::GetGoalPos() {
	return goalPos;
}

void Grid::SetGoalPos(int x, int y) {
	if (x<0 || x>gridSizeX || y<0 || y>gridSizeY)
		return;

	grid[goalPos.x][goalPos.y].goalCell = false;
	goalPos = grid[x][y];
	grid[x][y].goalCell = true;
}

Cell Grid::GetStartPos() {
	return startPos;
}

void Grid::SetStartPos(int x, int y) {
	if (x<0 || x>gridSizeX || y<0 || y>gridSizeY)
		return;

	grid[startPos.x][startPos.y].startCell = false;
	startPos = grid[x][y];
	grid[x][y].startCell = true;
}

void Grid::OutputGrid() {
	//If the cell is marked as a goal or start pos, output that character instead of a wall or floor tile
	for (int y = 0; y < gridSizeY; y++) {
		for (int x = 0; x < gridSizeX; x++) {
			std::cout << ((grid[x][y].goalCell) ? (char)Tile::goal : ((grid[x][y].startCell) ? (char)Tile::start : (char)(grid[x][y].tileType)));
		}
		std::cout << std::endl;
	}
}

std::vector<Cell> Grid::GetValidNeighbors(int x, int y) {
	std::vector<Cell> neighbors = std::vector<Cell>();

	//Check all directions to make sure they are in bounds, and either a floor tile or a goal tile.
	//Then return the vector, populated or not
	if (x - 1 >= 0 && (grid[x - 1][y].tileType == Tile::floor || grid[x - 1][y].goalCell)) {
		neighbors.push_back(grid[x - 1][y]);
	}
	if (y - 1 >= 0 && (grid[x][y - 1].tileType == Tile::floor || grid[x][y - 1].goalCell)) {
		neighbors.push_back(grid[x][y - 1]);
	}
	if (x + 1 < gridSizeX && (grid[x + 1][y].tileType == Tile::floor || grid[x + 1][y].goalCell)) {
		neighbors.push_back(grid[x + 1][y]);
	}
	if (y + 1 < gridSizeY && (grid[x][y + 1].tileType == Tile::floor || grid[x][y + 1].goalCell)) {
		neighbors.push_back(grid[x][y + 1]);
	}

	return neighbors;
}

double Grid::GetManhattanDistance(int x1, int y1, int x2, int y2) {
	if ((x1 < 0 || x1 >= gridSizeX || x2 < 0 || x2 >= gridSizeX) || (y1<0 || y1>gridSizeY || y2<0 || y2>gridSizeY))
		return -1;

	return abs(x1 - x2) + (y1 - y2);
}

double Grid::GetEuclidianDistance(int x1, int y1, int x2, int y2) {
	if ((x1 < 0 || x1 >= gridSizeX || x2 < 0 || x2 >= gridSizeX) || (y1<0 || y1>gridSizeY || y2<0 || y2>gridSizeY))
		return -1;

	return sqrt(pow(abs(x2 - x1), 2) + pow(abs(y2 - y1), 2));
}

std::vector<Cell> Grid::DepthFirstSearch() {
	//Fringe serves as the fringe, path is the path to return later
	std::stack<Cell> fringe = std::stack<Cell>();
	std::vector<Cell> path = std::vector<Cell>();

	fringe.push(startPos); //Push the startPos onto the fringe

	int totalTraversedCells = 0;

	//Loop until we have no other possible ways to move
	while (!fringe.empty()) {
		Cell current = grid[fringe.top().x][fringe.top().y]; //Make a copy of the current cell

		//If this cell has already been visited, remove it from the fringe and continue
		if (current.visited) {
			fringe.pop();
			continue;
		}

		grid[current.x][current.y].visited = true; //Mark this cell as visited
		totalTraversedCells++;

		fringe.pop(); //Remove the current cell from the fringe

		//If we have found the goal, we need to return the path
		if (current.goalCell) {
			if (displayAllTraversedCells)
				std::cout << "(" << current.x << ", " << current.y << ")[Goal]";

			std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;

			//If the parent cell is a nullptr, the current cell is also the start position. Simply return the current cell as the path.
			//Otherwise, compile a vector of the path's trail back to the start, and reverse it
			if (current.parentCell == nullptr) {
				path.push_back(current);
				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			} else {
				Cell inwards = grid[goalPos.x][goalPos.y]; //the current cell moving backwards

				//Continue on while the parent cell is not a nullptr
				while (inwards.parentCell!=nullptr) {
					path.push_back(inwards);
					inwards = *inwards.parentCell;
				}

				std::reverse(path.begin(), path.end()); //make sure the vector is in the right order

				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			}
		}

		if (displayAllTraversedCells)
			std::cout << "(" << current.x << ", " << current.y << ") -> ";

		std::vector<Cell> neighbors = GetValidNeighbors(current.x, current.y); //Obtain the neighbors that are valid

		for (int x = 0;x < neighbors.size();x++) {
			//If we haven't visited the cell, set the parent cell to this cell, and push it onto the stack
			if (neighbors[x].visited == false) {
				grid[neighbors[x].x][neighbors[x].y].parentCell = &grid[current.x][current.y];
				fringe.push(neighbors[x]);
			}
		}
	}

	std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;
	ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
	return path;
}

std::vector<Cell> Grid::BreadthFirstSearch() {
	//Fringe serves as the fringe, path is the path to return later
	std::queue<Cell> fringe = std::queue<Cell>();
	std::vector<Cell> path = std::vector<Cell>();

	fringe.push(startPos); //push the start position onto the fringe

	int totalTraversedCells = 0;

	//Loop until we have no other possible ways to move
	while (!fringe.empty()) {
		Cell current = grid[fringe.front().x][fringe.front().y]; //Make a copy of the current cell

		 //If this cell has already been visited, remove it from the fringe and continue
		if (current.visited) {
			fringe.pop();
			continue;
		}

		grid[current.x][current.y].visited = true; //Mark this cell as visited
		totalTraversedCells++;

		fringe.pop(); //Remove the current cell from the fringe

		if (current.goalCell) {
			if (displayAllTraversedCells)
				std::cout << "(" << current.x << ", " << current.y << ")[Goal]";

			std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;

			//If the parent cell is a nullptr, the current cell is also the start position. Simply return the current cell as the path.
			//Otherwise, compile a vector of the path's trail back to the start, and reverse it
			if (current.parentCell == nullptr) {
				path.push_back(current);
				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			} else {
				Cell inwards = grid[goalPos.x][goalPos.y]; //the current cell moving backwards

				//Continue on while the parent cell is not a nullptr
				while (inwards.parentCell != nullptr) {
					path.push_back(inwards);
					inwards = *inwards.parentCell;
				}

				std::reverse(path.begin(), path.end()); //make sure the vector is in the right order

				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			}
		}

		if(displayAllTraversedCells)
			std::cout << "(" << current.x << ", " << current.y << ") -> ";

		std::vector<Cell> neighbors = GetValidNeighbors(current.x, current.y); //Obtain the neighbors that are valid

		for (int x = 0;x < neighbors.size();x++) {
			if (neighbors[x].visited == false) {
				//If we haven't visited the cell, set the parent cell to this cell, and push it onto the stack
				grid[neighbors[x].x][neighbors[x].y].parentCell = &grid[current.x][current.y];
				fringe.push(neighbors[x]);
			}
		}
	}

	std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;
	ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
	return path;
}

std::vector<Cell> Grid::GreedySearch() {
	std::vector<Cell> path;
	std::vector<Cell> greedyStack;
	greedyStack.push_back(startPos);

	int totalTraversedCells = 0;

	while (greedyStack.size() > 0) {
		//Find the lowest h value of the list
		int currentPos = 0;
		for (int x = 1;x < greedyStack.size();x++) {
			if (grid[greedyStack[currentPos].x][greedyStack[currentPos].y].h >= grid[greedyStack[x].x][greedyStack[x].y].h)
				currentPos = x;
		}

		Cell current = grid[greedyStack[currentPos].x][greedyStack[currentPos].y];
		greedyStack.erase(greedyStack.begin() + currentPos);

		if (current.visited)
			continue;

		grid[current.x][current.y].visited = true;
		totalTraversedCells++;

		if (current.goalCell) {
			if (displayAllTraversedCells)
				std::cout << "(" << current.x << ", " << current.y << ")[Goal]";

			std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;

			//If the parent cell is a nullptr, the current cell is also the start position. Simply return the current cell as the path.
			//Otherwise, compile a vector of the path's trail back to the start, and reverse it
			if (current.parentCell == nullptr) {
				path.push_back(current);
				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			} else {
				Cell inwards = grid[goalPos.x][goalPos.y]; //the current cell moving backwards

				//Continue on while the parent cell is not a nullptr
				while (inwards.parentCell != nullptr) {
					path.push_back(inwards);
					inwards = *inwards.parentCell;
				}

				std::reverse(path.begin(), path.end()); //make sure the vector is in the right order

				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			}
		}

		if (displayAllTraversedCells)
			std::cout << "(" << current.x << ", " << current.y << ") -> ";

		std::vector<Cell> neighbors = GetValidNeighbors(current.x, current.y);
		//std::cout << neighbors.size() << std::endl;
		for (int x = 0;x < neighbors.size();x++) {
			if (neighbors[x].visited == false) {
				//If we haven't visited the cell, set the parent cell to this cell, and push it onto the stack
				grid[neighbors[x].x][neighbors[x].y].parentCell = &grid[current.x][current.y];
				grid[neighbors[x].x][neighbors[x].y].h = GetEuclidianDistance(neighbors[x].x, neighbors[x].y, goalPos.x, goalPos.y);
				greedyStack.push_back(grid[neighbors[x].x][neighbors[x].y]);
			}
		}
	}

	std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;
	ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
	return path;
}

std::vector<Cell> Grid::AStarSearch() {
	std::vector<Cell> path;
	std::vector<Cell> starStack;
	starStack.push_back(startPos);

	int totalTraversedCells = 0;

	while (starStack.size() > 0) {
		//Find the lowest f value of the list
		int currentPos = 0;
		for (int x = 1;x < starStack.size();x++) {
			if (grid[starStack[currentPos].x][starStack[currentPos].y].GetF() > grid[starStack[x].x][starStack[x].y].GetF())
				currentPos = x;
		}

		Cell current = grid[starStack[currentPos].x][starStack[currentPos].y];
		starStack.erase(starStack.begin() + currentPos);

		if (current.visited)
			continue;

		grid[current.x][current.y].visited = true;
		totalTraversedCells++;

		if (current.goalCell) {
			if (displayAllTraversedCells)
				std::cout << "(" << current.x << ", " << current.y << ")[Goal]";

			std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;

			//If the parent cell is a nullptr, the current cell is also the start position. Simply return the current cell as the path.
			//Otherwise, compile a vector of the path's trail back to the start, and reverse it
			if (current.parentCell == nullptr) {
				path.push_back(current);
				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			} else {
				Cell inwards = grid[goalPos.x][goalPos.y]; //the current cell moving backwards

				//Continue on while the parent cell is not a nullptr
				while (inwards.parentCell != nullptr) {
					path.push_back(inwards);
					inwards = *inwards.parentCell;
				}

				std::reverse(path.begin(), path.end()); //make sure the vector is in the right order

				ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
				return path;
			}
		}

		if (displayAllTraversedCells)
			std::cout << "(" << current.x << ", " << current.y << ") -> ";

		std::vector<Cell> neighbors = GetValidNeighbors(current.x, current.y);
		//std::cout << neighbors.size() << std::endl;
		for (int x = 0;x < neighbors.size();x++) {
			if (neighbors[x].visited == false) {
				//If we haven't visited the cell, set the parent cell to this cell, and push it onto the stack
				grid[neighbors[x].x][neighbors[x].y].parentCell = &grid[current.x][current.y];
				grid[neighbors[x].x][neighbors[x].y].h = GetEuclidianDistance(neighbors[x].x, neighbors[x].y, goalPos.x, goalPos.y);
				grid[neighbors[x].x][neighbors[x].y].g = grid[current.x][current.y].g+1;
				starStack.push_back(grid[neighbors[x].x][neighbors[x].y]);
			}
		}
	}

	std::cout << "\n\nTotal Visited Cells: " << totalTraversedCells;
	ResetCellSearchSettings(); //Reset all grid cells to be not-visited and have no-parent-pointers.
	return path;
}

/* Reset all cells in the grid to be unvisited and parentless */
void Grid::ResetCellSearchSettings() {
	for (int x = 0;x < gridSizeX;x++) {
		for (int y = 0;y < gridSizeY;y++) {
			grid[x][y].visited = false;
			grid[x][y].parentCell = nullptr;
			grid[x][y].h = 0;
			grid[x][y].g = 0;
		}
	}
}

void Grid::PepperWalls() {
	for (int x = 0;x < gridSizeX;x++) {
		for (int y = 0;y < gridSizeY;y++) {
			if (x == 0 || x == gridSizeX - 1 || y == 0 || y == gridSizeY - 1)
				SetCell(x, y, Tile::wall);
			else
				SetCell(x, y, Tile::floor);
		}
	}

	for (int x = 1;x < gridSizeX - 1;x++) {
		for (int y = 1;y < gridSizeY - 1;y++) {
			if ((rand() % 100) < 25) {
				SetCell(x, y, Tile::wall);
			}
		}
	}
}

void Grid::SetDisplayAllTraversedCells(bool flag) {
	displayAllTraversedCells = flag;
}

bool Grid::GetDisplayAllTraversedCells() {
	return displayAllTraversedCells;
}
#endif