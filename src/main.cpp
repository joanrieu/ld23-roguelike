#include <cassert>
#include <fstream>
#include <iostream>
#include <set>
#include <SFML/Graphics.hpp>

template <typename V>
unsigned squareLength(const V& vector) {
        return vector.x * vector.x + vector.y * vector.y;
}

enum class Asset {
        Player,
        Enemy,
        Grass,
        Wood,
};

struct Cell {
        sf::Vector2i position;
        Asset asset;
};

struct Character {
        sf::Vector2i position;
        Asset asset;
};

struct Player: Character { Player() { asset = Asset::Player; } };
struct Enemy: Character { Enemy() { asset = Asset::Enemy; } };

class Level {

        public:

        Level(std::string path);

        ~Level() {
                for (const Cell* cell: map)
                        delete cell;
                for (const Enemy* enemy: enemies)
                        delete enemy;
        }

        std::set<Cell*> map;
        sf::Vector2i start;
        std::set<Enemy*> enemies;

};

Level::Level(std::string path) {

        std::ifstream file(path);
        assert(file);

        // Read level size
        int width, height;
        file >> width >> height;

        { // Read player start position
                int x, y;
                file >> x >> y;
                start = sf::Vector2i(x, y);
        }

        // Read amount of enemies
        int enemyCount;
        file >> enemyCount;

        // Read enemy position
        while (enemyCount --> 0) {
                Enemy* enemy = new Enemy;
                file >> enemy->position.x >> enemy->position.y;
                enemies.insert(enemy);
        }

        // Read cells
        for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {

                        // Position is automatic
                        Cell* cell = new Cell;
                        cell->position = sf::Vector2i(x, y);

                        // A character represents a type of cell
                        char type = 0;
                        file >> type;

                        switch (type) {

                                case 'X':
                                cell->asset = Asset::Wood;
                                break;

                                case ',':
                                cell->asset = Asset::Grass;
                                break;

                                // Any unknown character results in empty cell
                                // (except blanks which are skipped)
                                default:
                                delete cell;
                                continue;

                        }

                        map.insert(cell);

                }
        }

}

class Assets {

        public:

        static const char* getName(Asset a) {
                switch (a) {
                        case Asset::Player:
                        return "player";
                        case Asset::Grass:
                        return "grass";
                        case Asset::Wood:
                        return "wood";
                        case Asset::Enemy:
                        return "enemy";
                }
        }

        static std::string getPath(std::string file) {
                file.insert(0, "assets/");
                return file;
        }

        static std::string getPath(Asset a, std::string ext) {
                ext.insert(0, ".");
                ext.insert(0, getName(a));
                return getPath(ext);
        }

        static sf::Texture& getTexture(Asset a) {
                auto& t = textures[a];
                if (t)
                        return *t;
                t = new sf::Texture;
                const bool loaded = t->loadFromFile(getPath(a, "png"));
                assert(loaded);
                return *t;
        }

        private:

        static std::map<Asset, sf::Texture*> textures;

};

decltype(Assets::textures) Assets::textures;

int main() {

        sf::Vector2i viewSize(30, 20);
        const int cellSize = 32;
        const int maxRenderRadiusSq = squareLength(viewSize) / 4;

        Level level(Assets::getPath("level.map"));
        Player player;
        player.position = level.start;

        sf::RenderWindow win(sf::VideoMode(viewSize.x * cellSize, viewSize.y * cellSize),
                "Tiny World", sf::Style::Default & ~sf::Style::Resize);
        win.setFramerateLimit(60);

        while (win.isOpen()) {

                sf::Event event;
                while (win.pollEvent(event)) {

                        if (event.type == sf::Event::Closed
                                or (event.type == sf::Event::KeyPressed and event.key.code == sf::Keyboard::Escape))
                                win.close(); // TODO Menu ?

                        else if (event.type == sf::Event::KeyPressed) {

                                sf::Vector2i oldPos = player.position;

                                switch (event.key.code) {

                                        case sf::Keyboard::Up:
                                        --player.position.y;
                                        break;

                                        case sf::Keyboard::Down:
                                        ++player.position.y;
                                        break;

                                        case sf::Keyboard::Left:
                                        --player.position.x;
                                        break;

                                        case sf::Keyboard::Right:
                                        ++player.position.x;
                                        break;

                                        default:
                                        continue;

                                }

                                for (Enemy* enemy: level.enemies) {
                                        if (player.position == enemy->position) {
                                                player.position = oldPos;
                                                std::cout << "Touched enemy" << std::endl;
                                                // TODO HitPoints
                                        }
                                }

                                if (player.position != oldPos) {
                                        for (Enemy* enemy: level.enemies) {
                                                const sf::Vector2i relPos(enemy->position - player.position);
                                                if (std::abs(relPos.x) > std::abs(relPos.y))
                                                        enemy->position.x += relPos.x < 0 ? +1 : -1;
                                                else if (relPos.x and relPos.y)
                                                        enemy->position.y += relPos.y < 0 ? +1 : -1;
                                        }
                                }

                        }

                }

                win.clear(sf::Color(50, 50, 50));

                // Draw map
                for (const Cell* cell: level.map) {
                        const sf::Vector2i relPos(cell->position - player.position);
                        if (squareLength(relPos) < maxRenderRadiusSq) {
                                sf::Sprite sprite(Assets::getTexture(cell->asset));
                                sprite.move(viewSize.x / 2 * cellSize, viewSize.y / 2 * cellSize);
                                sprite.move(relPos.x * cellSize, relPos.y * cellSize);
                                win.draw(sprite);
                        }

                }

                // Draw enemies
                for (const Enemy* enemy: level.enemies) {
                        const sf::Vector2i relPos(enemy->position - player.position);
                        if (squareLength(relPos) < maxRenderRadiusSq) {
                                sf::Sprite sprite(Assets::getTexture(enemy->asset));
                                sprite.move(viewSize.x / 2 * cellSize, viewSize.y / 2 * cellSize);
                                sprite.move(relPos.x * cellSize, relPos.y * cellSize);
                                win.draw(sprite);
                        }

                }

                { // Draw Player
                        sf::Sprite sprite(Assets::getTexture(Asset::Player));
                        sprite.move((viewSize.x/2) * cellSize, (viewSize.y/2) * cellSize);
                        win.draw(sprite);
                }

                win.display();

        }

        return 0;

}
