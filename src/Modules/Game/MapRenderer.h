#ifndef MAPRENDERER_H
#define MAPRENDERER_H
#include <SFML/Graphics/RenderWindow.hpp>
#include "CellStorage.h"
#include "GameController.h"

class MapRenderer {
private:
    CellStorage &storage;
    sf::RenderWindow &window; // window into which render Cells
    GameController &controller;

public:
    explicit MapRenderer(CellStorage &storage, sf::RenderWindow &window, GameController &controller);

    void render_map() const;
};

#endif
