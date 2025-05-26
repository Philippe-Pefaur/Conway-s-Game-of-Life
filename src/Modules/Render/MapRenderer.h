#ifndef MAPRENDERER_H
#define MAPRENDERER_H
#include <SFML/Graphics/RenderWindow.hpp>
#include "CellStorage.h"

class MapRenderer {
private:
    sf::RenderWindow &window;
    CellStorage &storage;
public:
    MapRenderer(sf::RenderWindow &window, CellStorage &storage);

    void render_map() const;
};

#endif //MAPRENDERER_H
