/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <iostream>

#include "Modules/Cells/CellMap.h"
#include "Modules/Cells/CellProcessor.h"
#include "Modules/Cells/CellStorage.h"

int main() {
    auto map = CellMap(10, 20); // create a map full of dead cells
    auto cell_storage = CellStorage(); // create a storage
    const auto cell_processor = CellProcessor(map, cell_storage); // create a processor that works with the current map and storage

    // draw live cells on map (template for a spaceship)
    map[0][1].set_live(true); // set cell to live
    cell_processor.store_subjects(map[0][1]); // store cells subject to change upon modification for processing
    map[1][2].set_live(true);
    cell_processor.store_subjects(map[1][2]);
    map[2][0].set_live(true);
    cell_processor.store_subjects(map[2][0]);
    map[2][1].set_live(true);
    cell_processor.store_subjects(map[2][1]);
    map[2][2].set_live(true);
    cell_processor.store_subjects(map[2][2]);

    while (!cell_storage.get_cells().empty()) { // while there are cells subject to modification
        std::cout << map; // print the current state of the map
        std::cout << "----------------------------------------------------------------------------"<<'\n';

        cell_processor.process_subjects(); // process and clear all stored cells and add them to the storage's revive or kill vector under certain conditions

        cell_processor.revive_cells(); // revive cells in revive vector and add their subjects to the storage
        cell_processor.kill_cells(); // kill cells in kill vector and add their subjects to the storage
    }

    return 0;
}
