#ifndef GRID_H
#define GRID_H

#include "Cell.h"

#include <time.h>
#include <vector>
#include <iostream>
#include <stack>
#include <queue>

/* A 2d grid filled with Cell */
class Grid {
public:
	Grid(); /* Sets up the grid with size (2, 2) boundaries */
	Grid(int x); /* Sets up the grid with size (x, x) boundaries */
	Grid(int x, int y); /* Sets up the grid with size (x, y) boundaries */

	void ResizeGrid(int x); /* Resize the grid with (x, x) boundaries */
	void ResizeGrid(int x, int y); /* Resize the grid with (x, y) boundaries */

	void SetRandomStartGoal(); /* Set a random start and goal position */

	bool SetCell(int x, int y, Tile tile); /* Attempts to set the value of a cell. Returns true if successful, false if fail */
	bool SetCell(Cell cell, Tile tile); /* Attempts to set the value of a cell. Returns true if successful, false if fail */
	Cell GetCell(int x, int y); /* Attempts to get the cell reference */
	Cell GetCell(Cell cell); /* Attempts to get the cell reference */

	Cell GetGoalPos(); /* Returns the currently marked goal cell */
	void SetGoalPos(int x, int y); /* Reallocates the goal cell to be (x, y) */
	Cell GetStartPos(); /* Returns the currently marked start cell */
	void SetStartPos(int x, int y); /* Reallocates the start cell to be (x, y) */

	int GetGridX(); /* Returns the size of the current grid's X value [gridSizeX] */
	int GetGridY(); /* Returns the size of the current grid's Y value [gridSizeY] */

	void OutputGrid(); /* Outputs a generic grid with images to help depict tile types */

	std::vector<Cell> GetValidNeighbors(int x, int y); /* Get all neighbors that are either floor or goal tiles*/
	double GetManhattanDistance(int x1, int y1, int x2, int y2); /* Returns the manhattan distance between two points */
	double GetEuclidianDistance(int x1, int y1, int x2, int y2); /* Returns the euclidian distance between two points */

	std::vector<Cell> DepthFirstSearch(); /* Uses DFS to search the grid from start point to goal point */
	std::vector<Cell> BreadthFirstSearch(); /* Uses BFS to search the grid from start point to goal point */
	std::vector<Cell> GreedySearch(); /* Uses a Greedy search to find the goal point */
	std::vector<Cell> AStarSearch(); /* Uses a Greedy search to find the goal point */

	void ResetCellSearchSettings(); /* Resets the visited and parentCell variables for cells */

	void PepperWalls(); /* Pepper random walls in the grid for testing purposes */

	void SetDisplayAllTraversedCells(bool flag); /* Sets the display all traversed cells flag */
	bool GetDisplayAllTraversedCells(); /* Retrieves the display all traversed cells flag */
private:
	int gridSizeX = 2; /* Size of the grid in the X direction */
	int gridSizeY = 2; /* Size of the grid in the Y direction */
	std::vector<std::vector<Cell>> grid = std::vector<std::vector<Cell>>(); /* A 2d grid filled with Cell */
	Cell startPos; /* The currently marked start cell */
	Cell goalPos;/* The currently marked goal cell */
	bool displayAllTraversedCells = false;
};

#endif