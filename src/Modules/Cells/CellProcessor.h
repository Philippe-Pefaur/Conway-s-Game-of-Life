#ifndef CELLPROCESSOR_H
#define CELLPROCESSOR_H
#include <SFML/Graphics/RenderWindow.hpp>

#include "CellMap.h"
#include "Cell.h"
#include "CellStorage.h"

/**
 * @brief Processes Cells
 *
 * It contains references to a CellMap and CellStorage to manage and process Cells inside the map
 */

class CellProcessor {
private:
    CellMap &map; // CellMap where the processor operates, modifying its Cells
    CellStorage &storage; // CellStorage for processor's Cell management

public:
    /**
     * @brief Builder for CellProcessor
     *
     * Processors operate only over a CellMap, as they require context to
     * process the cells that they are indicated.
     *
     * @param map Reference to the overseen CellMap
     * @param storage Reference to the program's CellStorage
     */
    explicit CellProcessor(CellMap &map, CellStorage &storage);

    /**
     * @brief Finds all Cells that are adjacent on the map grid to the given Cell
     *
     * Finding a Cells adjacent Cells is fundamental to the processing procedure,
     * as, when a singular Cell's state is modified, every Cell surrounding it is
     * subject to have it's state modified.
     *
     * @param cell Indicates the Cell for which adjacent Cells are to be found
     * @return Vector with Reference Wrappers to the found Cells
     */
    [[nodiscard]] std::vector<std::reference_wrapper<Cell>> get_adjacent(const Cell &cell) const;
    /**
     * @brief Checks adjacent Cells to a given cell and defines if it's stage should be changed
     *
     * Counts the numer of live Cells surrounding the indicated Cell to determine
     * how it should be modified following Conway's Game of Life's rules.
     *
     * @param cell Indicates the Cell for which to analyze possible state modifications
     * @return 0 if Cell should be killed, 1 if Cell should be revived, -1 if Cell shouldn't be modified
     */
    [[nodiscard]] int check_state(const Cell &cell) const;

    /**
     * @brief Stores Cells subject to modification upon pre-identified changes to a given Cell
     *
     * Stores the given Cell, and it's adjacent Cells, to the storage's Cells
     * to be processed. The given Cells is included as no Cell is ever instantly
     * modified when a state change is recognized.
     */
    void store_subjects(Cell &cell) const;
    /**
     * @brief Processes a new state for the CellMap without realizing any modifications
     *
     * Interprets the output of the check_state function for every Cell stored
     * in the storage's main cell Vector, and adds the Cells to the storage's
     * tb_revived or tb_killed Vector accordingly. Then, it clears the storage's
     * cells Vector.
     */
    void process_subjects() const;

    /**
     * @brief Revives all Cells in the Storage's tb_revived vector
     */
    void revive_cells() const;
    /**
     * @brief Kills all Cells in the Storage's tb_revived vector
     */
    void kill_cells() const;
};

#endif
