#ifndef CELL_H
#define CELL_H
#include <ostream>

/**
 * @brief Represents a cell in conway's game of life
 *
 * It contains the position of the Cell in a CellMap and information on the current state of the cell
 */

class Cell {
private:
    int i; // row number
    int j; // column number
    bool live; // cell state (0: Dead, 1:Live)
    bool stored; // indicates the presence of the cell in a CellStorage's cells for processing (0: Not stored, 1: Stored)

public:
    /**
     * @brief Builder for Cell
     *
     * Cells are only to be instanced upon instancing a CellMap
     *
     * @param i Row number in external CellMap
     * @param j Column number in external CellMap
     */
    Cell(int i, int j);

    /**
     * @brief << Operator override for formatted output
     */
    friend std::ostream & operator<<(std::ostream &os, const Cell &cell);

    [[nodiscard]] int get_i() const;
    [[nodiscard]] int get_j() const;
    [[nodiscard]] bool is_live() const;
    [[nodiscard]] bool is_stored() const;
    void set_stored(bool stored);
    void set_live(bool live);
};

#endif
