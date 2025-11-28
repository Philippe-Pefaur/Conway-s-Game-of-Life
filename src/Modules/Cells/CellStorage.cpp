/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "CellStorage.h"

// Initialize a CellStorage
CellStorage::CellStorage() {
    last_render_size = 0;
};

// Add a Cell for general processing purposes
void CellStorage::add_cell(Cell &cell) {
    cells.emplace_back(cell); // Creates and adds a Reference Wrapper with the Cell reference into the cells Vector
}

void CellStorage::clear_cells() {
    cells.clear();
}

// Add a Cell for reviving purposes
void CellStorage::add_revive(Cell &cell) {
    tb_revived.emplace_back(cell); // Creates and adds a Reference Wrapper with the Cell reference into the tb_revived Vector
}
void CellStorage::clear_tb_revived() {
    tb_revived.clear();
}

// Add a Cell for killing purposes
void CellStorage::add_kill(Cell &cell) {
    tb_killed.emplace_back(cell); // Creates and adds a Reference Wrapper with the Cell reference into the tb_killed Vector
}
void CellStorage::clear_tb_killed() {
    tb_killed.clear();
}

// Add a Cell for rendering purposes
void CellStorage::add_render(Cell &cell) {
    tb_rendered.emplace_back(cell);
}

// Erase cells that are no longer queued for rendering
void CellStorage::clean_tb_rendered() {
    std::erase_if(tb_rendered,
                  [](const std::reference_wrapper<Cell> &cell){return !cell.get().is_tb_rendered(); });
}

// Add Cell for tracking purposes
void CellStorage::add_live(Cell &cell) {
    live_cells.emplace_back(cell);
}

// Checks live_cells for dead cells and removes them
void CellStorage::clean_live_cells() {
    std::erase_if(live_cells,
                  [](const std::reference_wrapper<Cell> &cell){return !cell.get().is_live(); });
}

void CellStorage::clear_live_cells() {
    live_cells.clear();
}

// Add Cell for drawing purposes
void CellStorage::add_selection(Cell &cell) {
    selection.emplace_back(cell);
}

void CellStorage::clear_selection() {
    selection.clear();
}

// Add cell for rendering purposes
void CellStorage::add_last_selection(Cell &cell) {
    last_selection.emplace_back(cell);
}

void CellStorage::clear_last_selection() {
    last_selection.clear();
}

std::vector<std::reference_wrapper<Cell>> & CellStorage::get_cells() {
    return cells;
}
std::vector<std::reference_wrapper<Cell>> & CellStorage::get_tb_revived() {
    return tb_revived;
}
std::vector<std::reference_wrapper<Cell>> & CellStorage::get_tb_killed() {
    return tb_killed;
}

std::vector<std::reference_wrapper<Cell>> & CellStorage::get_tb_rendered() {
    return tb_rendered;
}

std::vector<std::reference_wrapper<Cell>> & CellStorage::get_live_cells() {
    return live_cells;
}

std::vector<std::reference_wrapper<Cell>> & CellStorage::get_selection() {
    return selection;
}

std::vector<std::reference_wrapper<Cell> > &CellStorage::get_last_selection() {
    return last_selection;
}
