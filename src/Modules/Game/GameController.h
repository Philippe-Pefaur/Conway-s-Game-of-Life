#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include <string>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>


class GameController {
private:
    sf::Clock timer; // timer used for synchronization of game componentes
    float delay; // delay for game logic loop
    bool paused; // indicates if logic processing is paused
    int draw; // indicates type of cell drawing
    int size; // indicates selection size
    std::string tool; // indicates selection pattern


public:
    explicit GameController();

    void reset_timer();

    [[nodiscard]] float get_time() const;

    [[nodiscard]] float get_delay() const;
    void set_delay(float delay);

    [[nodiscard]] bool is_paused() const;
    void set_paused(bool paused);

    [[nodiscard]] int get_draw() const;
    void set_draw(int draw);

    [[nodiscard]] std::string get_tool() const;
    void set_tool(const std::string &tool);

    [[nodiscard]] int get_size() const;
    [[nodiscard]] int &get_size_ref();
    void set_size(int size);
};

#endif
