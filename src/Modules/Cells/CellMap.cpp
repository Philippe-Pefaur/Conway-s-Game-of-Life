/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "CellMap.h"

// Initialize a CellMap full of dead Cells
CellMap::CellMap(const int rows, const int columns) {
    this->rows = rows;
    this->columns = columns;
    this->cells.resize(rows, std::vector<Cell>(columns, Cell(-1, -1))); // resizes the vectors to fit the CellMap's dimensions and save a default Cells in the grid
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) { // for every space in the grid
            cells[i][j] = Cell(i, j); // overwrite the default cell for a new cell with its corresponding position
        }
    }
}

// Override [] for Cell accessing purposes
std::vector<Cell> & CellMap::operator[](const int index) {
    return this->cells[index]; // returns the Cell Vector stored in the corresponding index
}

// Override << for terminal printing purposes
std::ostream & operator<<(std::ostream & os,CellMap & map) {
    for (int i = 0; i < map.getRows(); i++) {
        for (int j = 0; j < map.getColumns(); j++) { // for every space in the map's grid
            if (map[i][j].is_live()) { // print 1 if cell is live
                os << 1 << " ";
            }
            else { // print a blank space if cell is dead
                os << "  ";
            }
        }
        os << '\n'; // line break when advancing to next row
    }
    return os;
}

const std::vector<std::vector<Cell>> & CellMap::getCells() const {
    return cells;
}
int CellMap::getRows() const {
    return rows;
}
int CellMap::getColumns() const {
    return columns;
}
