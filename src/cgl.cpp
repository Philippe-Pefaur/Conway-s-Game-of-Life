/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include <iostream>
#include <SFML/Graphics.hpp>

#include "MapRenderer.h"
#include "Modules/Cells/CellMap.h"
#include "Modules/Cells/CellProcessor.h"
#include "Modules/Cells/CellStorage.h"

int main() {
    auto map = CellMap(72, 128); // create a map full of dead cells
    auto cell_storage = CellStorage(); // create a storage
    const auto cell_processor = CellProcessor(map, cell_storage); // create a processor that works with the current map and storage

    // load textures
    if (!Cell::load_textures()) {
        std::cerr << "ERROR: Failed to load texture" << std::endl;
        return EXIT_FAILURE;
    }
    // strat and configure window
    sf::RenderWindow window(sf::VideoMode(1280,720), "cgl", sf::Style::Titlebar | sf::Style::Close);
    sf::Image icon;
    icon.loadFromFile("assets/icon/icon.png");
    window.setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
    window.setFramerateLimit(0);

    MapRenderer renderer(window, cell_storage);

    window.clear(sf::Color::Black);

    // draw live cells on map (template for a spaceship)
    map[0][1].set_live(true); // set cell to live
    cell_processor.store_subjects(map[0][1]); // store cells subject to change upon modification for processing
    cell_storage.add_render(map[0][1]);
    map[1][2].set_live(true);
    cell_processor.store_subjects(map[1][2]);
    cell_storage.add_render(map[1][2]);
    map[2][0].set_live(true);
    cell_processor.store_subjects(map[2][0]);
    cell_storage.add_render(map[2][0]);
    map[2][1].set_live(true);
    cell_processor.store_subjects(map[2][1]);
    cell_storage.add_render(map[2][1]);
    map[2][2].set_live(true);
    cell_processor.store_subjects(map[2][2]);
    cell_storage.add_render(map[2][2]);

    renderer.render_map();

    window.display();

    // Control variables
    sf::Event event{};
    sf::Clock timer;
    float delay = 0.14f;
    bool paused = true;

    // GAME LOOP
    while (window.isOpen()) {
        if (paused) {
            while (window.pollEvent(event)) { // process window events
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::Space:
                        case sf::Keyboard::P:
                            std::cout << "\nUNPAUSED" << std::endl;
                            paused = false;
                            break;
                        default:
                            break;
                    }
                }
            }
        }
        else {
            while (window.pollEvent(event)) {  // process window events
                if (event.type == sf::Event::Closed) {
                    window.close();
                }
                if (event.type == sf::Event::KeyPressed) {
                    switch (event.key.code) {
                        case sf::Keyboard::P:
                            std::cout << "\nPAUSED" << std::endl;
                            paused = true;
                            break;
                        case sf::Keyboard::Up:
                            delay += 0.01f;
                            break;
                        case sf::Keyboard::Down:
                            if (delay >= 0.01) {
                                delay -= 0.01f;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
            // Game logic processing
            if (!cell_storage.get_cells().empty() && timer.getElapsedTime().asSeconds() > delay) { // if there are cells subject to modification
                cell_processor.process_subjects(); // process and clear all stored cells and add them to the storage's revive or kill vector under certain conditions

                cell_processor.revive_cells(); // revive cells in revive vector and add their subjects to the storage
                cell_processor.kill_cells(); // kill cells in kill vector and add their subjects to the storage

                timer.restart();
            }
            else if (cell_storage.get_cells().empty()) {
                paused = true;
                std::cout << "\nPAUSED" << std::endl;
                }

            window.clear(sf::Color::Black); // clear last render

            renderer.render_map(); // render live cells

            window.display(); // draw render to screen
        }
    }

    return 0;
}
