#ifndef CELLMAP_H
#define CELLMAP_H
#include "Cell.h"
#include <vector>

/**
 * @brief Represents a grid of Cells
 *
 * It contains e vector of vectors full of Cell instances
 */

class CellMap {
private:
    std::vector<std::vector<Cell>> cells; // Cells grid
    int rows; // number of vectors stored in the cells vector
    int columns; // number of cells per vector in the cells vector

public:
    /**
     * @brief Builder for CellMap
     *
     * Upon instancing, a CellMap is filled with dead Cells
     *
     * @param rows Row number
     * @param columns Column number
     */
    CellMap(int rows, int columns);

    /**
     * @brief << Operator override to print map formatted
     */
    friend std::ostream & operator<<(std::ostream & os,CellMap & map);
    /**
     * @brief [] Operator override to directly access the indicated vector in cells
     */
    std::vector<Cell> & operator[](int index);

    [[nodiscard]] const std::vector<std::vector<Cell>> & getCells() const;
    [[nodiscard]] int get_rows() const;
    [[nodiscard]] int get_columns() const;
};



#endif //CELLMAP_H
