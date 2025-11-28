#include "MapRenderer.h"

MapRenderer::MapRenderer(CellStorage &storage, sf::RenderWindow &window, GameController &controller): storage(storage), window(window), controller(controller) {}

// Renders all Cells in CellStorage's tb_rendered vector
void MapRenderer::render_map() const {
    for (auto i_cell : storage.get_tb_rendered()) {
        window.draw(i_cell.get().get_sprite());
    }
    window.display();
}
