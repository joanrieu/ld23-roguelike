#pragma once

#include <fstream>
#include <map>
#include <vector>
#include <SFML/Graphics.hpp>

sf::Texture* getTexture();

struct Cell {

        Cell();

        bool canWalk;
        sf::Texture* texture;
        Cell* above;

        enum Type: char {
                Grass = ',',
                Water = '~',
        };

        void makeGrass();
        void makeWater();

};

struct World {

        World();

        int width, height;
        std::vector<Cell*> cells;

        Cell*& at(int x, int y);
        Cell* make(Cell::Type type);

};

class Game {

        public:

        Game();

        void run();
        void draw();
        void update();
        void update(const sf::Event& event);

        private:

        sf::RenderWindow m_window;
        World m_world;

};
