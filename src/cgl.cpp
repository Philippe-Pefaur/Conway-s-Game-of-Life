/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <iostream>
#include <SFML/Graphics.hpp>

#include "Modules/Cells/CellSelector.h"
#include "Modules/Game/MapRenderer.h"
#include "Modules/Cells/CellMap.h"
#include "Modules/Cells/CellProcessor.h"
#include "Modules/Cells/CellStorage.h"
#include "Modules/Game/EventManager.h"
#include "Modules/Game/GameController.h"

int main() {
    // Instance logic processing classes
    auto map = CellMap(72, 128); // create a map full of dead cells
    auto cell_storage = CellStorage(); // create a storage
    auto cell_processor = CellProcessor(map, cell_storage); // create a processor that works with the current map and storage
    auto cell_selector = CellSelector(map, cell_storage); // create a selector that works with the current map and storage

    // Load textures
    if (!Cell::load_textures()) { // if files could not be loaded
        std::cerr << "ERROR: Failed to load texture" << std::endl;
        return EXIT_FAILURE; // end program
    }

    // Strat and configure window
    sf::RenderWindow window(sf::VideoMode(1280,720), "cgl", sf::Style::None);
    sf::Image icon;
    icon.loadFromFile("assets/icon/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setFramerateLimit(144);
    window.clear(sf::Color::Black);
    window.display();

    // Create game processing classes
    auto controller = GameController();
    auto event_manager = EventManager(map, cell_processor, window, controller);
    auto renderer = MapRenderer(cell_storage, window, controller);

    // GAME LOOP
    while (window.isOpen()) {
        if (controller.is_paused()) {
            event_manager.process_paused_events(); // check for window events

            window.clear(sf::Color::Black); // clear last render

            // Draw cells on screen
            sf::Vector2i mouse_pos = sf::Mouse::getPosition(window); // get mouse position
            switch (controller.get_draw()) { // check for draw mode (default 0)
                case 0: // highlight selected cells
                    for (auto i_cell : cell_storage.get_last_selection()) { // for each cell selected beforehand
                        i_cell.get().reset_sprite(); // reset sprite to avoid temporary sprites persisting through the loop
                    }
                    cell_storage.clear_last_selection(); // clear the last selection
                    for (auto i_cell : cell_selector.get_selection(controller.get_tool(), mouse_pos, controller.get_size_ref())) { // for each cell in the current selection
                        i_cell.get().set_sprite(Cell::over_texture); // change sprite to highlight
                        window.draw(i_cell.get().get_sprite()); // draw cell on screen
                        cell_storage.add_last_selection(i_cell); // add cell to last selection
                    }
                    break;
                case 1: // revive selected cells
                    for (auto i_cell : cell_storage.get_last_selection()) { // for every cell in the last selection
                        i_cell.get().reset_sprite(); // reset sprite
                    }
                    cell_storage.clear_last_selection(); // celar the last selection
                    cell_processor.revive(cell_selector.get_selection(controller.get_tool(), mouse_pos, controller.get_size_ref())); // revive cells in the current selection
                    break;
                case 2: // kill selected cells
                    cell_processor.kill(cell_selector.get_selection(controller.get_tool(), mouse_pos, controller.get_size_ref())); // kill cells in the current selection
                    break;
                default:
                    break;
            }

            renderer.render_map(); // render live and selected cells
        }
        else {
            event_manager.process_events(); // check for window events

            // Do game logic
            if (!cell_storage.get_cells().empty() && controller.get_time() > controller.get_delay()) { // if there are cells subject to modification
                cell_processor.logic_step(); // do a cell logic step
                controller.reset_timer(); // reset timer for step delay calculation
            }
            else if (cell_storage.get_cells().empty()) { // otherwise, when no more cells are subject to modification
                controller.set_paused(true); // pause the game
                std::cout << "\nPAUSED" << std::endl;
                }

            window.clear(sf::Color::Black); // clear last render
            renderer.render_map(); // render live cells
        }
    }

    return 0;
}