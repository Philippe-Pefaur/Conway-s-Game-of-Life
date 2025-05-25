/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "Cell.h"

// Initialize a dead cell with position i, j
Cell::Cell(const int i, const int j) {
    this->i = i;
    this->j = j;
    this->live = false; // indicates that the cell is dead
    this->stored = false; // indicates that the cell is not stored

}

// Override << for terminal printing purposes
std::ostream & operator<<(std::ostream &os, const Cell &cell) {
    std::string state; // string to indicate the cells state
    if (cell.is_live()) { // if cell is live set the string to "live"
        state = "live";
    }
    else { // if cell is dead set the string to "dead"
        state = "dead";
    }
    return os // return a line that contains cell information in format (i, j): state
           << "(" << cell.get_i()
           << ", " << cell.get_j()
           << "): " << state;
}

int Cell::get_i() const {
    return i;
}
int Cell::get_j() const {
    return j;
}
bool Cell::is_live() const {
    return live;
}

bool Cell::is_stored() const {
    return stored;
}

void Cell::set_stored(const bool stored) {
    this->stored = stored;
}

void Cell::set_live(const bool live) {
    this->live = live;
}
