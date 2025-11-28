#ifndef CELLSELECTOR_H
#define CELLSELECTOR_H
#include <vector>

#include "Cell.h"
#include "CellMap.h"
#include "CellStorage.h"

class CellSelector {
private:
    CellMap &map;
    CellStorage &storage;

public:
    CellSelector(CellMap &map, CellStorage &storage);

    /**
     * Generates a selection vector given a selection pattern
     * (tool & size) and a position in the map.
     * @param tool
     * @param mouse
     * @param size
     * @return Cell vector
     */
    [[nodiscard]] std::vector<std::reference_wrapper<Cell>> get_selection(const std::string &tool, sf::Vector2i mouse, int &size) const;

};

#endif
