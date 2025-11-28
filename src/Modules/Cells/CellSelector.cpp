#include "CellSelector.h"

#include <cmath>
#include <iostream>

CellSelector::CellSelector(CellMap &map, CellStorage &storage): map(map), storage(storage) {}

std::vector<std::reference_wrapper<Cell>> CellSelector::get_selection(const std::string &tool, const sf::Vector2i mouse, int &size) const {
    // get mouse coordinates in grid
    const int i_pos = mouse.y / 10;
    const int j_pos = mouse.x / 10;

    // generate selection
    std::vector<std::reference_wrapper<Cell>> selection;
    if (i_pos < 0 || i_pos > map.get_rows()-1 || j_pos < 0 || j_pos > map.get_columns()-1) { // in case the coordinates are out of bounds
        return selection; // return empty selection
    }
    if (tool == "pencil") { // selects only the cell directly below the cursor
        selection.emplace_back(map[i_pos][j_pos]);
    }
    else if (tool == "linex") { // selects all cells in a horizontal line that crosses the cursor
        for (int j = 0; j < map.get_columns(); j++) {
            selection.emplace_back(map[i_pos][j]);
        }
    }
    else if (tool == "liney") { // selects all cells in a vertical line that crosses the cursor
        for (int i = 0; i < map.get_rows(); i++) {
            selection.emplace_back(map[i][j_pos]);
        }
    }
    else if (tool == "circle") { // select all cells within a radius (size) originating from the cursor
        // iterates though all cells in a square around the cursor
        for (int i = i_pos - size; i <= i_pos + size; i++) {
            if (i < 0 || i > map.get_rows() - 1) { // skip if out of bounds
                continue;
            }
            for (int j = j_pos - size; j <= j_pos + size; j++) {
                if (j < 0 || j > map.get_columns() - 1) { // skip if out of bounds
                    continue;
                }
                // if the cell is within the radius, add it to selection
                if (std::sqrt(pow(i - i_pos, 2) + pow(j - j_pos, 2)) < size) {
                    selection.emplace_back(map[i][j]);
                }
            }
        }
    }
    else if (tool == "circumference") {
        // iterates though all cells in a square around the cursor
        for (int i = i_pos - size; i <= i_pos + size; i++) {
            if (i < 0 || i > map.get_rows() - 1) { // skip if out of bounds
                continue;
            }
            for (int j = j_pos - size; j <= j_pos + size; j++) {
                if (j < 0 || j > map.get_columns() - 1) { // skip if out of bounds
                    continue;
                }
                // if the cell is between the radius and the next lower radius, add it to selection
                auto distance = std::sqrt(pow(i - i_pos, 2) + pow(j - j_pos, 2));
                if (distance < size && distance >= size-1) {
                    selection.emplace_back(map[i][j]);
                }
            }
        }
    }
    return selection;
}
