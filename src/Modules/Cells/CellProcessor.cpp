/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "CellProcessor.h"

// Initializes a CellProcessor with a given CellMap and CellStorage
CellProcessor::CellProcessor(CellMap &map, CellStorage &storage): map(map), storage(storage) {}

// Finds a Cell's adjacent Cells
std::vector<std::reference_wrapper<Cell>> CellProcessor::get_adjacent(const Cell &cell) const {
    std::vector<std::reference_wrapper<Cell>> adjacent_cells; // instance a vector to store the adjacent
    for (int i = cell.get_i()-1; i <= cell.get_i()+1; i++) { // iterates through the Cell's row, the one on top and below
        if (i < 0 || i > map.getRows()-1) continue; // skips the iteration if the row index is out of range
        for (int j = cell.get_j()-1; j <= cell.get_j()+1; j++) { // iterates through the Cell's column, the one preceding it and the one next
            if (j < 0 || j > map.getColumns()-1) continue; // skip the iteration if the column index is out of range
            if (!(i==cell.get_i() && j==cell.get_j())) { // if the position is not the given Cell's position
                adjacent_cells.push_back(std::ref(map[i][j])); // add the Cell to the vector
            }
        }
    }
    return adjacent_cells;
}

// Analyzes possible state modifications on a given Cell
int CellProcessor::check_state(const Cell &cell) const {
    int live_counter = 0; // create a counter to add the number of live Cells surrounding the Cell
    for (auto i_cell : this->get_adjacent(cell)) { // iterates through every adjacent Cell
        if (i_cell.get().is_live()) { // if the cell is live increase the counter by 1
            live_counter++;
        }
    }

    if (cell.is_live()) { // if the given Cell is live
        if (live_counter < 2 || live_counter > 3) { // if the Cell is surrounded by less than two or more than three
            return 0; // indicate that the Cell should be killed
        }
    }
    else { // if the given Cell is dead
        if (live_counter == 3) { // if the Cell is surrounded by exactly three live Cells
            return 1; // indicate that the Cell should be revived
        }
    }
    return -1; // if none of the conditions are met indicate that the Cell shouldn't be modified
}

// Stores Cells subject to a given Cell's modification
void CellProcessor::store_subjects(Cell &cell) const {
    if (!cell.is_stored()) { // if the given Cell isn't already stored
        storage.add_cell(cell); // store the Cell
        cell.set_stored(true); // indicate that it is stored
    }

    for (auto i_cell : this->get_adjacent(cell)) { // iterate through adjacent Cells
        if (auto &cell_ref = i_cell.get(); !cell_ref.is_stored()) { // if the Cell isn't already stored
            storage.add_cell(cell_ref); // store the Cell
            cell_ref.set_stored(true); // indicate that it is stored
        }
    }
}

// Processes all Cells stored in the storer's cells
void CellProcessor::process_subjects() const {
    for (auto i_cell : storage.get_cells()) { // process cells subject to modification
        switch (this->check_state(i_cell.get())) { // check if cells should be revived or killed
            case 0: // cell must be killed
                storage.add_kill(i_cell); // store on kill vector
            break;
            case 1: // cell must be revived
                storage.add_revive(i_cell); // store on revive vector
            break;
            default: // cell isn't modified
                break;
        }
        i_cell.get().set_stored(false); // mark the cell as not stored
    }
    storage.clear_cells(); // clear all cells stored for processing
}

// Revives Cells to be revived
void CellProcessor::revive_cells() const {
    for (auto i_cell : storage.get_tb_revived()) { // iterates through every Cell in the storer's tb_revived
        i_cell.get().set_live(true); // set the Cell's state to live
        this->store_subjects(i_cell); // store Cells subject to the modification
    }
    storage.clear_tb_revived(); // clear the Vector
}

// Kills Cells to be killed
void CellProcessor::kill_cells() const {
    for (auto i_cell : storage.get_tb_killed()) { // iterates through every Cell in the storer's tb_killed
        i_cell.get().set_live(false); // set the Cell's state to dead
        this->store_subjects(i_cell); // store Cells subject to the modification
    }
    storage.clear_tb_killed(); // clear the Vector
}
