#include "EventManager.h"

#include <iostream>
#include <SFML/Window/Event.hpp>

EventManager::EventManager(CellMap &map, CellProcessor &processor, sf::RenderWindow &window, GameController &controller): map(map), processor(processor), window(window), controller(controller) {}

void EventManager::process_paused_events() const { // manage events while game logic is paused
    sf::Event event{};
    while (window.pollEvent(event)) { // process window events
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Space: // resume logic processing
                case sf::Keyboard::P: // resume logic processing
                    std::cout << "\nUNPAUSED" << std::endl;
                    controller.set_paused(false);
                    break;
                case sf::Keyboard::C: // kill all cells in map
                    processor.clear_map();
                    break;
                case sf::Keyboard::Right: // do a single logic cycle
                    processor.logic_step();
                    break;
                case sf::Keyboard::Num1: // switch selection tool to pencil
                    controller.set_tool("pencil");
                    break;
                case sf::Keyboard::Num2: // switch selection tool to horizontal line
                    controller.set_tool("linex");
                    controller.set_size(map.get_columns());
                    break;
                case sf::Keyboard::Num3: // switch selection tool to circle
                    controller.set_tool("circle");
                    controller.set_size(6);
                    break;
                case sf::Keyboard::Tab: { // shift between secondary tools
                    auto tool = controller.get_tool();
                    if (tool == "linex") { // shift to vertical line
                        controller.set_tool("liney");
                    }
                    else if (tool == "liney") { // shift to horizontal line
                        controller.set_tool("linex");
                    }
                    else if (tool == "circle") { // shift to circumference
                        controller.set_tool("circumference");
                    }
                    else if (tool == "circumference") { // shift to circle
                        controller.set_tool("circle");
                    }
                    break;
                }
                default:
                    break;
            }
        }
        if (event.type == sf::Event::MouseWheelScrolled) { // control selection size
            if (event.mouseWheelScroll.delta > 0) { // mouse wheel dow, decrease size
                controller.set_size(controller.get_size() + 1);
            }
            else if (event.mouseWheelScroll.delta < 0) { // mouse wheel up, increase size
                controller.set_size(controller.get_size() - 1);
            }
        }
        if (event.type == sf::Event::MouseButtonPressed) { // draw or delete selection
            switch (event.mouseButton.button) {
                case sf::Mouse::Left: // draw
                    controller.set_draw(1);
                    break;
                case sf::Mouse::Right: // delete
                    controller.set_draw(2);
                    break;
                default:
                    break;
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) { // on release stop drawing
            controller.set_draw(0);
        }
    }
}

void EventManager::process_events() const { // manage events while game logic is running
    sf::Event event{};
    while (window.pollEvent(event)) {  // process window events
        if (event.type == sf::Event::Closed) {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed) {
            switch (event.key.code) {
                case sf::Keyboard::Space: // pause logic processing
                case sf::Keyboard::P: // pause logic processing
                    controller.set_paused(true);
                    std::cout << "\nPAUSED" << std::endl;
                    break;
                case sf::Keyboard::Up: // slow down logic loop
                    controller.set_delay(controller.get_delay() + 0.01f); // decrease logic delay
                    break;
                case sf::Keyboard::Down: // speed up logic loop
                    if (controller.get_delay() >= 0.01) {
                        controller.set_delay(controller.get_delay() - 0.01f); // increase logic delay
                    }
                    break;
                default:
                    break;
            }
        }
    }
}