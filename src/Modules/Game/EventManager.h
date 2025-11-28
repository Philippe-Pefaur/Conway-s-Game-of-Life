#ifndef EVENTMANAGER_H
#define EVENTMANAGER_H
#include "CellProcessor.h"
#include "GameController.h"

class EventManager {
private:
    CellMap &map;
    CellProcessor &processor;
    sf::RenderWindow &window; // window to get events
    GameController &controller; // controller to modify variables

public:
    explicit EventManager(CellMap &map, CellProcessor &processor, sf::RenderWindow &window, GameController &controller);

    void process_paused_events() const;

    void process_events() const;
};



#endif //EVENTMANAGER_H
