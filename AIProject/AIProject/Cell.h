#ifndef CELL_H
#define CELL_H

/* Tile type of a cell, represented by a character */
enum class Tile : char {
	floor = (char)(177),
	wall = (char)(219),
	start = 'S',
	goal = 'G'
};

class Cell {
public:
	Cell(); /* Initialize the cell with coordinates (0, 0) */
	Cell(int x, int y); /* Initialize a cell with coordinates (x, y) */
	int GetF(); /* Returns g + h */

	Cell *parentCell = nullptr; /* The previous cell used in the searching algorithm */

	int x; /* X Coordinate of the cell */
	int y; /* Y Coordinate of the cell */
	int g; /* The cumulative cost to move based on title */
	int h; /* The estimated cost to the goal */
	Tile tileType = Tile::floor; /* Tile type of this cell */
	bool startCell = false; /* Flag for if this cell is a start cell or not */
	bool goalCell = false; /* Flag for if this cell is a goal clel or not */
	bool visited = false; /* Flag for if this cell has been visited */
};
#endif