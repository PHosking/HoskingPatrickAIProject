#ifndef CELL_CPP
#define CELL_CPP

#include "Cell.h"

Cell::Cell() {
	this->x = 0;
	this->y = 0;
}

Cell::Cell(int x, int y) {
	this->x = x;
	this->y = y;
}

int Cell::GetF() {
	return g + h;
}
#endif