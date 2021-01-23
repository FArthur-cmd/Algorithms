/**
 * Два игрока играют в настольную игру. Игровое поле представляет собой квадратный лабиринт,
 * 8× 8 клеток. В некоторых клетках располагаются стенки. Один игрок управляет фишкой-терминатором,
 * а второй — фишкой-беглецом. Игроки ходят по очереди, ходы пропускать нельзя
 * (гарантируется, что ход всегда возможен). За один ход игрок может переместить свою фишку
 * в любую из свободных клеток, расположенных рядом с исходной по горизонтали,
 * вертикали или по диагонали (то есть ходом короля). Терминатор, кроме того,
 * может стрелять в беглеца ракетами. Выстрел идет по прямой в любом направлении по горизонтали,
 * вертикали или диагонали. Если беглец оказывается на линии выстрела терминатора и
 * не прикрыт стенками, то терминатор незамедлительно делает выстрел (вне зависимости от того, чей ход),
 * и беглец проигрывает. Начальное положение фишек задано. Первый ход делает беглец.
 * Он выигрывает, если сделает ход с восьмой строки за пределы игрового поля,
 * так как остальные границы поля окружены стенками.
 * Вопрос задачи: может ли беглец выиграть при оптимальной игре обеих сторон?
 *
 *
 * Формат ввода
 * Во входном файле задано игровое поле. Свободная клетка обозначена цифрой 0,
 * а клетка со стенкой — цифрой 1. Клетка, в которой находится беглец, обозначена цифрой 2,
 * а клетка с терминатором — цифрой 3.
 *
 * Формат вывода
 * В выходной файл выведите число 1, если беглец выигрывает, и -1 — в противном случае.
 */
#include <iostream>
#include <vector>
#include <cstring>

// Константы для проверок
const char WALL = '1';
const char RUNNER = '2';
const char TERMINATOR = '3';
const int16_t ONE_SIZE_LENGTH = 8;
const uint16_t MAX_SIZE = ONE_SIZE_LENGTH * ONE_SIZE_LENGTH;
const uint8_t PLAYERS_COUNT = 2;

// Информация о состоянии игры (чей ход и на каких позициях игроки)
struct players_info {
    int32_t x_coord;
    int32_t y_coord;

    bool first_turn;

    players_info(int32_t x = 0, int32_t y = 0, bool turn = false);
};

players_info::players_info(int32_t x, int32_t y, bool turn) : x_coord(x), y_coord(y), first_turn(turn) {}

// Информация о текущем состоянии, выигрыш или проигрыш и использовалось ли в дфс
struct node_info {
    bool win = false;
    bool lose = false;
    bool used = false;
};

/*
 * Класс граф.
 *     Выполняются добавления вершин в граф (с увеличением соответствующий степени вершины графа)
 *     Дфс имеет как бы обратный вид. В алгоритме мы будем идти от клеток, для которых известно
 *     их выигрышность или проигрышность. Статус обновляется если:
 *     - мы смотрим из проигрыша, то другая выигрыш
 *     - мы смотрим из выигрыша, то пока не кончились связи
 *     (пока все вершины, из которых можно попасть в рассматриваюмаю, - выигрышные, то она проигрышная)
 *     Иначе запускаем дфс дальше
 */
struct Graph {

    Graph();

    void add_connection(const players_info &first, const players_info &second);

    void dfs(uint32_t x, uint32_t y, bool player_id);

    node_info status[MAX_SIZE][MAX_SIZE][PLAYERS_COUNT];
    uint8_t degree[MAX_SIZE][MAX_SIZE][PLAYERS_COUNT];

    std::vector<players_info> connections[MAX_SIZE][MAX_SIZE][PLAYERS_COUNT];

};

void Graph::dfs(uint32_t x, uint32_t y, bool player_id) {
    if (!(status[x][y][player_id].used)) {
        status[x][y][player_id].used = true;
        for (auto &[x_coord, y_coord, first_turn] : connections[x][y][player_id]) {  // (u, v) \in Edges
            if (!status[x_coord][y_coord][first_turn].used) {
                if (status[x][y][player_id].lose) {
                    status[x_coord][y_coord][first_turn].win = true;
                } else if (--degree[x_coord][y_coord][first_turn] == 0) {
                    status[x_coord][y_coord][first_turn].lose = true;
                } else {
                    continue;
                }
                dfs(x_coord, y_coord, first_turn);
            }
        }
    }
}

Graph::Graph() {
    std::memset(status, 0, sizeof(status));
    std::memset(degree, 0, sizeof(degree));
    for (size_t i = ONE_SIZE_LENGTH * (ONE_SIZE_LENGTH - 1); i < MAX_SIZE; ++i) {
        for (size_t j = 0; j < MAX_SIZE; ++j) {
            degree[i][j][0] = 1;
        }
    }
}


void Graph::add_connection(const players_info &first, const players_info &second) {
    connections[first.x_coord][first.y_coord][first.first_turn].push_back(second);
    ++degree[second.x_coord][second.y_coord][second.first_turn];
}

/*
 * Класс игра.
 * Считываем поле и по нему строим граф игры.
 * Находим позиции беглеца и терминатора
 * Выполняем дфс по позициям и выводим ответ, который получается в
 * клетке, соответствующей позициям персонажей и началу хода беглеца
 */
class Game {
public:
    Game(std::istream &in);

    bool answer();

private:
    void build_graph();

    void make_win_lose();

    bool check_line() const;

    void create_connections(uint8_t first, uint8_t second, bool term_turn);

    static inline bool on_board(int32_t coordinate);

    Graph game_graph_;
    std::vector<std::string> field_;
    int16_t run_x_, run_y_, term_y_, term_x_; // знаковый для корректного прибавления -1
    uint8_t first_{}, second_{};
    const std::vector<std::pair<int8_t, int8_t>> directions_ = {
            {1,  0},
            {1,  1},
            {1,  -1},
            {0,  -1},
            {-1, -1},
            {-1, 0},
            {-1, 1},
            {0,  1}
    };
};

bool Game::on_board(int32_t coordinate) {
    return 0 <= coordinate && coordinate < ONE_SIZE_LENGTH;
}

Game::Game(std::istream &in) : field_(ONE_SIZE_LENGTH), run_x_(0), run_y_(0),
                               term_x_(0), term_y_(0) {
    for (size_t i = 0; i < ONE_SIZE_LENGTH; ++i) {
        in >> field_[i];
    }
    build_graph();
    make_win_lose();
}

bool Game::check_line() const {
    // Если не одна диагональ и не на одной вертикали\горизонатли (тангенс равен +- 1,
    // то есть разность координат равна или имеет противоположный знак или одна из координат совпадает)
    if ((run_x_ - term_x_ != term_y_ - run_y_) && (run_x_ - term_x_ != run_y_ - term_y_)
        && !(run_x_ == term_x_ || run_y_ == term_y_)) {
        return false;
    }

    // Направление изменения координаты
    int8_t x_difference = (term_x_ - run_x_ == 0) ? 0 : (term_x_ - run_x_ < 0) ? -1 : 1;
    int8_t y_difference = (term_y_ - run_y_ == 0) ? 0 : (term_y_ - run_y_ < 0) ? -1 : 1;
    int16_t new_first_x = run_x_;
    int16_t new_first_y = run_y_;

    while (new_first_x != term_x_ || new_first_y != term_y_) {
        if (field_[new_first_x + x_difference][new_first_y + y_difference] != WALL) {
            new_first_x += x_difference, new_first_y += y_difference;
        } else {
            return false;
        }
    }
    return true;
}

void Game::create_connections(uint8_t first_player_pos, uint8_t second_player_pos, bool term_turn) {
    int16_t next_first_x, next_first_y, next_sec_x, next_sec_y;

    for (auto &dir : directions_) {
        next_first_x = run_x_, next_first_y = run_y_;
        next_sec_x = term_x_, next_sec_y = term_y_;

        if (term_turn) {
            next_first_x += dir.first;
            next_first_y += dir.second;
        } else {
            next_sec_x += dir.first;
            next_sec_y += dir.second;
        }

        if (on_board(next_first_x) && on_board(next_first_y) &&
            on_board(next_sec_x) && on_board(next_sec_y) &&
            field_[next_first_x][next_first_y] != WALL &&
            field_[next_sec_x][next_sec_y] != WALL) {
            players_info from(first_player_pos, second_player_pos, static_cast<bool>(term_turn));
            players_info to(next_first_x * ONE_SIZE_LENGTH + next_first_y,
                            next_sec_x * ONE_SIZE_LENGTH + next_sec_y,
                            static_cast<bool>(!term_turn));
            game_graph_.add_connection(from, to);
        }
    }
}

void Game::build_graph() {
    for (run_x_ = 0; run_x_ < ONE_SIZE_LENGTH; ++run_x_) {
        for (run_y_ = 0; run_y_ < ONE_SIZE_LENGTH; ++run_y_) {
            for (term_x_ = 0; term_x_ < ONE_SIZE_LENGTH; ++term_x_) {
                for (term_y_ = 0; term_y_ < ONE_SIZE_LENGTH; ++term_y_) {
                    for (int8_t second_turn = 0; second_turn < PLAYERS_COUNT; ++second_turn) {
                        if (field_[run_x_][run_y_] == WALL || field_[term_x_][term_y_] == WALL) {
                            continue;
                        }

                        if (field_[run_x_][run_y_] == RUNNER) {
                            first_ = run_x_ * ONE_SIZE_LENGTH + run_y_;
                        }

                        if (field_[term_x_][term_y_] == TERMINATOR) {
                            second_ = term_x_ * ONE_SIZE_LENGTH + term_y_;
                        }

                        auto &cur_node = game_graph_.status[run_x_ * ONE_SIZE_LENGTH +
                                                            run_y_][term_x_ * ONE_SIZE_LENGTH +
                                                                    term_y_][second_turn];

                        if (!second_turn) {
                            cur_node.win = (run_x_ == ONE_SIZE_LENGTH - 1) && !check_line();
                            cur_node.lose = check_line();
                        } else {
                            cur_node.win = check_line();
                        }
                        create_connections(run_x_ * ONE_SIZE_LENGTH + run_y_,
                                           term_x_ * ONE_SIZE_LENGTH + term_y_,
                                           second_turn);
                    }
                }
            }
        }
    }
}

void Game::make_win_lose() {
    for (uint8_t x = 0; x < MAX_SIZE; ++x) {
        for (uint8_t y = 0; y < MAX_SIZE; ++y) {
            for (uint8_t turn = 0; turn < PLAYERS_COUNT; ++turn) {
                if ((game_graph_.status[x][y][turn].win || game_graph_.status[x][y][turn].lose) &&
                    !game_graph_.status[x][y][turn].used) {
                    game_graph_.dfs(x, y, turn);
                }
            }
        }
    }
}

bool Game::answer() {
    return game_graph_.status[first_][second_][0].win;
}

int main() {
    Game game(std::cin);

    std::cout << (game.answer() ? 1 : -1);
    return 0;
}
