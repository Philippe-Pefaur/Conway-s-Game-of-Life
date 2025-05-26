#include "MapRenderer.h"

MapRenderer::MapRenderer(sf::RenderWindow &window, CellStorage &storage): window(window), storage(storage) {}

void MapRenderer::render_map() const {
    for (auto i_cell : storage.get_tb_rendered()) {
        window.draw(i_cell.get().get_sprite());
    }
}