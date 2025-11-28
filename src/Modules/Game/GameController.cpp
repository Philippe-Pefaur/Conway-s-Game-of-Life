#include "GameController.h"

GameController::GameController() {
    delay = 0.14f; // default delay
    paused = true;
    draw = 0;
    tool = "pencil";
    size = 0;
    timer.restart();
}

void GameController::reset_timer() {
    timer.restart();
}

float GameController::get_time() const {
    return timer.getElapsedTime().asSeconds();
}

float GameController::get_delay() const {
    return delay;
}

void GameController::set_delay(const float delay) {
    this->delay = delay;
}

bool GameController::is_paused() const {
    return paused;
}

void GameController::set_paused(const bool paused) {
    this->paused = paused;
}

int GameController::get_draw() const {
    return draw;
}

void GameController::set_draw(const int draw) {
    this->draw = draw;
}

std::string GameController::get_tool() const {
    return tool;
}

void GameController::set_tool(const std::string &tool) {
    this->tool = tool;
}

int GameController::get_size() const {
    return size;
}

// returns a reference to the size stored
int & GameController::get_size_ref() {
    return size;
}

void GameController::set_size(const int size) {
    this->size = size;
    if (this->size < 1) { // if size change goes below 1 set to 1 (minimum size)
        this->size = 1;
    }
}
