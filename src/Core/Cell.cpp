/*
 * Copyright (c) 2025 Philippe Pefaur
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */
#include "Cell.h"

#include <SFML/Graphics/Texture.hpp>

// Variables to store rendering textures
sf::Texture Cell::live_texture;
sf::Texture Cell::dead_texture;
sf::Texture Cell::over_texture;

// Initialize a dead cell with position i, j
Cell::Cell(const int i, const int j) {
    this->i = i;
    this->j = j;

    this->live = false; // indicates that the cell is dead
    this->stored = false; // indicates that the cell is not stored
    this->tb_rendered = false; // indicates that the cell shouldn't be rendered

    this->sprite.setPosition(j*10, i*10); // setup sprite for rendering
}

// Override << for terminal printing purposes
std::ostream & operator<<(std::ostream &os, const Cell &cell) {
    std::string state; // string to indicate the cells state
    if (cell.is_live()) { // if cell is live set the string to "live"
        state = "live";
    }
    else { // if cell is dead set the string to "dead"
        state = "dead";
    }
    return os // return a line that contains cell information in format (i, j): state
           << "(" << cell.get_i()
           << ", " << cell.get_j()
           << "): " << state;
}

int Cell::get_i() const {
    return i;
}
int Cell::get_j() const {
    return j;
}

sf::Sprite &Cell::get_sprite() {
    return sprite;
}

bool Cell::is_live() const {
    return live;
}

bool Cell::is_stored() const {
    return stored;
}

bool Cell::is_tb_rendered() const {
    return tb_rendered;
}

void Cell::set_stored(const bool stored) {
    this->stored = stored;
}

void Cell::set_tb_rendered(bool tb_rendered) {
    this->tb_rendered = tb_rendered;
}

void Cell::set_live(const bool live) {
    this->live = live;
    if (live) { // update sprite an render state
        set_sprite(live_texture);
        set_tb_rendered(true);
    }
    else { // update sprite an render state
        set_sprite(dead_texture);
        set_tb_rendered(false);
    }
}

void Cell::set_sprite(const sf::Texture &texture) {
    this->sprite.setTexture(texture);
}

// Function for updating cell sprites without need to give it
void Cell::reset_sprite() {
    if (live) {
        set_sprite(live_texture);
    }
    else {
        set_sprite(dead_texture);
    }
}

bool Cell::load_textures() {
    const bool check_live = Cell::live_texture.loadFromFile("assets/cell_sprites/live_cell.png");
    const bool check_dead = Cell::dead_texture.loadFromFile("assets/cell_sprites/dead_cell.png");
    const bool check_over = Cell::over_texture.loadFromFile("assets/cell_sprites/over_cell.png");

    return check_live && check_dead && check_over; // returns true if all textures were successfully loaded
}
