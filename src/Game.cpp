#include "Game.h"

sf::Texture* getTexture(const std::string& file) {
        static std::map<std::string, sf::Texture*> textures;
        sf::Texture*& texture = textures[file];
        if (not texture) {
                texture = new sf::Texture;
                texture->loadFromFile(file);
        }
        return texture;
}

Cell::Cell(): canWalk(false), texture(nullptr), above(nullptr) {}

void Cell::makeGrass() {
        canWalk = true;
        texture = getTexture("assets/grass.png");
}

void Cell::makeWater() {
        canWalk = false;
        texture = getTexture("assets/water.png");
}

World::World() {

        std::ifstream file("assets/world.map");

        file >> width >> height;

        char c;
        for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                        file >> c;
                        at(x, y) = make(static_cast<Cell::Type>(c));
                }
        }

}

Cell* World::make(Cell::Type type) {

        Cell* cell = new Cell;

        switch (type) {

                case Cell::Grass:
                cell->makeGrass();
                break;

                case Cell::Water:
                cell->makeWater();
                break;

        }

        return cell;

}

Cell*& World::at(int x, int y) {
        return cells[y*width+x];
}

Game::Game(): m_window(sf::VideoMode(800, 600), "Tiny World", sf::Style::Default & ~sf::Style::Resize) {
        m_window.setFramerateLimit(60);
}

void Game::run() {
        while (m_window.isOpen()) {
                draw();
                update();
        }
}

void Game::draw() {
        m_window.clear();
        m_window.display();
}

void Game::update() {
        sf::Event event;
        while (m_window.pollEvent(event))
                update(event);
}

void Game::update(const sf::Event& event) {

        if (event.type == sf::Event::Closed)
                m_window.close();

        if (event.type != sf::Event::KeyPressed)
                return;

        switch (event.key.code) {

                case sf::Keyboard::Escape:
                m_window.close();
                break;

                default:
                break;

        }

}

int main() {
        Game().run();
        return 0;
}
