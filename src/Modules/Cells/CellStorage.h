#ifndef CELLSTORER_H
#define CELLSTORER_H
#include <functional>

#include "Cell.h"

/**
 * @brief Contains multiple Vectors for storing Cells
 *
 * Every Vector contains Reference Wrappers with Cell references for different purposes
 */

class CellStorage {
private:
    std::vector<std::reference_wrapper<Cell>> cells; // main storage Vector, contains Cells for general processing
    std::vector<std::reference_wrapper<Cell>> tb_revived; // contains Cells to be revived
    std::vector<std::reference_wrapper<Cell>> tb_killed; // contains Cells to be killed

public:
    /**
     * @brief Builder for CellStorage
     *
     * Storages don't have any initializing parameters, they are independent of the CellMap being used
     */
    explicit CellStorage();

    /**
     * @brief Adds a Cell reference to cells
     */
    void add_cell(Cell &cell);
    void clear_cells();

    /**
     * @brief Adds a Cell reference to tb_revived
     */
    void add_revive(Cell &cell);
    void clear_tb_revived();
    /**
     * @brief Adds a Cell reference to tb_killed
     */
    void add_kill(Cell &cell);
    void clear_tb_killed();

    [[nodiscard]] std::vector<std::reference_wrapper<Cell>> & get_cells();
    [[nodiscard]] std::vector<std::reference_wrapper<Cell>> & get_tb_revived();
    [[nodiscard]] std::vector<std::reference_wrapper<Cell>> & get_tb_killed();
};

#endif
