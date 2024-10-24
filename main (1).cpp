#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <ctime>

class InputManager {
public:
    std::string question(const std::string& questionText) {
        std::string answer;
        std::cout << questionText;
        std::getline(std::cin, answer);
        return answer;
    }
};

class Dice {
public:
    static int roll(int min, int max) {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }
};

class Player {
public:
    std::string color;
    int columnsWinned;

    Player(const std::string& _color) : color(_color), columnsWinned(0) {}
};

class Board {
public:
    std::vector<std::vector<std::vector<std::string>>> table;

    Board() {
        table = {
            std::vector<std::vector<std::string>>(3),
            std::vector<std::vector<std::string>>(5),
            std::vector<std::vector<std::string>>(7),
            std::vector<std::vector<std::string>>(9),
            std::vector<std::vector<std::string>>(11),
            std::vector<std::vector<std::string>>(13),
            std::vector<std::vector<std::string>>(11),
            std::vector<std::vector<std::string>>(9),
            std::vector<std::vector<std::string>>(7),
            std::vector<std::vector<std::string>>(5),
            std::vector<std::vector<std::string>>(3)
        };
    }

    void cleanBoard(const std::string& color) {
        for (auto& row : table) {
            for (auto& column : row) {
                auto it = std::find_if(column.begin(), column.end(),
                    [&color](const std::string& token) { return token == color; });
                if (it != column.end()) {
                    column.erase(it);
                }
            }
        }
    }

    void saveBoard(const std::string& color) {
        for (auto& row : table) {
            for (auto& column : row) {
                auto it = std::find_if(column.begin(), column.end(),
                    [&color](const std::string& token) { return token == color; });
                if (it != column.end()) {
                    column.erase(it);
                    column.push_back(color + "_save");
                }
            }
        }
    }

    void updateColumn(std::vector<std::vector<std::string>>& column, const std::string& color) {
        bool alreadySavePosition = false;
        bool alreadyHaveToken = false;
        size_t saveIndex = 0;
        size_t tokenIndex = 0;

        for (size_t i = 0; i < column.size(); ++i) {
            for (const auto& token : column[i]) {
                if (token == color + "_save") {
                    alreadySavePosition = true;
                    saveIndex = i;
                }
                if (token == color) {
                    alreadyHaveToken = true;
                    tokenIndex = i;
                }
            }
        }

        if (alreadySavePosition && !alreadyHaveToken) {
            column[saveIndex + 1].push_back(color);
        }
        else if (alreadyHaveToken) {
            column[tokenIndex + 1].push_back(color);
            auto& tokens = column[tokenIndex];
            tokens.erase(std::remove(tokens.begin(), tokens.end(), color), tokens.end());
        }
        else {
            column[0].push_back(color);
        }
    }

    bool hasColorInColumn(const std::vector<std::vector<std::string>>& column, const std::string& color) {
        for (const auto& cell : column) {
            for (const auto& token : cell) {
                if (token.find(color) != std::string::npos) {
                    return true;
                }
            }
        }
        return false;
    }

    bool isColumnWon(const std::vector<std::vector<std::string>>& column) {
        return !column[column.size() - 1].empty();
    }
};

class Game {
private:
    Board board;
    InputManager inputManager;
    std::vector<Player> players;
    size_t turnCounter;

public:
    Game() : turnCounter(0) {}

    void initialize() {
        std::vector<std::string> colors = {"red", "blue", "green", "yellow", "black", "white", 
            "purple", "orange", "pink", "brown", "grey", "cyan", "magenta"};
        
        std::string input = inputManager.question("¿Cuántos jugadores son? (only numbers) ");
        int playersNumber = std::stoi(input);

        for (int i = 0; i < playersNumber; i++) {
            players.emplace_back(colors[i]);
        }
    }

    void playTurn() {
        bool playing = true;
        int availableTokens = 3;
        Player& currentPlayer = players[turnCounter];

        std::cout << "Turno de " << currentPlayer.color << std::endl;
        std::cout << "________________________________________________" << std::endl;

        while (playing) {
            std::cout << "Tokens Restantes: " << availableTokens << std::endl;

            int dice1 = Dice::roll(1, 6);
            int dice2 = Dice::roll(1, 6);
            int dice3 = Dice::roll(1, 6);
            int dice4 = Dice::roll(1, 6);

            int sum1 = dice1 + dice2;
            std::cout << "Dado 1: " << dice1 << ", Dado 2: " << dice2 << ", Suma: " << sum1 << std::endl;
            int sum2 = dice3 + dice4;
            std::cout << "Dado 3: " << dice3 << ", Dado 4: " << dice4 << ", Suma: " << sum2 << std::endl;

            auto& column1 = board.table[sum1 - 2];
            auto& column2 = board.table[sum2 - 2];

            if (availableTokens == 0) {
                if (!board.hasColorInColumn(column1, currentPlayer.color) && 
                    !board.hasColorInColumn(column2, currentPlayer.color)) {
                    board.cleanBoard(currentPlayer.color);
                    playing = false;
                    turnCounter = (turnCounter + 1) % players.size();
                    break;
                }
            }

            if (column1[column1.size() - 1].empty()) {
                if (availableTokens > 0 || board.hasColorInColumn(column1, currentPlayer.color)) {
                    if (availableTokens > 0 && !board.hasColorInColumn(column1, currentPlayer.color)) {
                        availableTokens--;
                    }

                    board.updateColumn(column1, currentPlayer.color);

                    if (board.isColumnWon(column1)) {
                        currentPlayer.columnsWinned++;
                    }
                }
            }
            else {
                std::cout << "Esta columna ya fue ganada..." << std::endl;
            }

            if (column2[column2.size() - 1].empty()) {
                if (availableTokens > 0 || board.hasColorInColumn(column2, currentPlayer.color)) {
                    if (availableTokens > 0 && !board.hasColorInColumn(column2, currentPlayer.color)) {
                        availableTokens--;
                    }

                    board.updateColumn(column2, currentPlayer.color);

                    if (board.isColumnWon(column2)) {
                        currentPlayer.columnsWinned++;
                    }
                }
            }
            else {
                std::cout << "Esta columna ya fue ganada..." << std::endl;
            }

            std::string continuePlaying = inputManager.question("¿Quieres seguir jugando? (S/N) ");
            
            std::cout << "_______________Table___________________________" << std::endl;
            // Print table (simplified for readability)
            for (const auto& row : board.table) {
                for (const auto& column : row) {
                    std::cout << "[";
                    for (const auto& token : column) {
                        std::cout << token << " ";
                    }
                    std::cout << "] ";
                }
                std::cout << std::endl;
            }
            std::cout << "_______________________________________________" << std::endl;

            if (continuePlaying == "N" || continuePlaying == "n") {
                board.saveBoard(currentPlayer.color);
                playing = false;
                turnCounter = (turnCounter + 1) % players.size();
            }
        }
    }

    bool isGameOver() {
        return std::any_of(players.begin(), players.end(), 
            [](const Player& player) { return player.columnsWinned >= 3; });
    }

    void play() {
        initialize();

        while (!isGameOver()) {
            playTurn();
        }

        std::cout << "Game Over" << std::endl;
    }
};

int main() {
    Game game;
    game.play();
    return 0;
}