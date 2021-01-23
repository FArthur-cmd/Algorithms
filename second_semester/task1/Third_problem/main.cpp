/*
 * Фрэнку опять прилетел новый заказ.
 * Однако в этот раз город играет по очень
 * странным правилам: все дороги в городе
 * односторонние и связывают только офисы
 * нанимателей перевозчика. Множество офисов,
 * между любыми двумя из которых существует
 * путь, образуют квартал, если нельзя добавить
 * никакие другие, чтобы условие выполнялось.
 * Фрэнку интересно, каково минимальное количество
 * односторонних дорог нужно ещё построить,
 * чтобы весь город стал квраталом.
 *
 * Формат ввода
 * Первая строка содержит пару целых чисел
 * n и m (1 ≤ n ≤ 105, 0 ≤ m ≤ 105) — количество вершин
 * и рёбер заданного ориентированного графа.
 *
 * Следующие m строк содержат описания рёбер,
 * по одному в строке. Каждая из строк содержит
 * по два целых числа a и b (1 ≤ a, b ≤ n) — начало и конец ребра.
 *
 * Формат вывода
 * Выведите одно число — количество дорог,
 * которые нужно добавить, чтобы весь город стал одним кварталом.
 */
#include <iostream>
#include <vector>
#include <set>
#include <algorithm>

template <typename T>
class Stack{
private:
    std::vector<T> buffer;
public:
    void push_back(T element) {
        buffer.push_back(element);
    }
    void pop() {
        buffer.pop_back();
    }
    T top() {
        return buffer.back();
    }
    unsigned int Size() {
        return buffer.size();
    }
    bool empty() {
        return (buffer.size() == 0);
    }
};

/*
 * Класс Граф
 * содержит пути и обратные пути
 *
 * функция подсчета количества вершин,
 * в которые не входит ни один путь
 *
 * функция подсчета количества вершин,
 * из которых не выходит ни один путь
 *
 * функция добавления вершин
 *
 * дфс по графу
 *
 * дфс по транспонированному графу
 *
 * решение. Делаем топ_сорт для транспонированного
 * графа. В порядке убывания времени выхода(обратным обзодом массива)
 * запускам дфс, получаем компоненты связности
 * строим в временном графе tmp
 * (состоит из компонент связности(все точки объединены))
 * считаем количество вершин с нулевым выходом
 * считаем точки с нулевым входом
 * ответ - максимум из них (следует из математического вывода)
 */
class Graph {
private:
    std::vector<std::set<int>> ways;
    std::vector<std::set<int>> reversed_ways;
public:
    int points_no_inputs() {
        int counter = 0;
        for (int i = 1; i < reversed_ways.size(); ++i) {
            if (reversed_ways[i].size() == 0)
                ++counter;
        }
        return counter;
    }
    int points_no_outputs() {
        int counter = 0;
        for (int i = 1; i < ways.size(); ++i) {
            if (ways[i].size() == 0)
                ++counter;
        }
        return counter;
    }
    Graph(const int &n = 0) {
        ways.resize(n + 1);
        reversed_ways.resize(n + 1);
    }
    void add(int from, int to) {
        ways[from].insert(to);
        reversed_ways[to].insert(from);
    }
    void dfs(const int &start_point, std::vector<int> &colours,
             std::vector<int> &point_status, const int &status) {
        Stack<int> work_with;
        work_with.push_back(start_point);
        while (!work_with.empty()) {
            int tmp = work_with.top();
            if (colours[tmp] == 0) {
                colours[tmp] = 1;
                for (int i : ways[tmp]) {
                    if (colours[i] == 0)
                        work_with.push_back(i);
                }
            } else if (colours[tmp] == 1) {
                point_status[tmp] = status;
                colours[tmp] = 2;
                work_with.pop();
            } else {
                work_with.pop();
            }
        }
    }
    void reversed_dfs(const int &start_point, std::vector<int> &colours,
                      std::vector<int> &top_sorted){
        Stack<int> work_with;
        work_with.push_back(start_point);
        while (!work_with.empty()) {
            int tmp = work_with.top();
            if (colours[tmp] == 0) {
                colours[tmp] = 1;
                for (int i : reversed_ways[tmp]) {
                    if (colours[i] == 0)
                        work_with.push_back(i);
                }
            } else if (colours[tmp] == 1) {
                top_sorted.push_back(tmp);
                colours[tmp] = 2;
                work_with.pop();
            } else {
                work_with.pop();
            }
        }
    }
    int solution() {
        std::vector<int> top_sorted, colours, point_status;
        int status = 1;
        colours.resize(ways.size());
        point_status.resize(ways.size());
        for (int i = 1; i < ways.size(); i++) {
            if (colours[i] == 0)
                reversed_dfs(i, colours, top_sorted);
        }
        for (int i = 1; i < ways.size(); i++) {
            colours[i] = 0;
        }
        for (int i = top_sorted.size() - 1; i >= 0; i--) {
            if (colours[top_sorted[i]] == 0) {
                dfs(top_sorted[i], colours, point_status, status);
                ++status;
            }
        }
        if (status == 2)
            return 0;
        Graph tmp(status - 1);
        for (int i = 1; i < ways.size(); i++) {
            for (int j : ways[i]) {
                if (point_status[i] != point_status[j])
                    tmp.add(point_status[i], point_status[j]);
            }
        }
        return std::max(tmp.points_no_inputs(), tmp.points_no_outputs());
    }
};

int main() {
    int verticies, edges, from, to;
    std::cin >> verticies >> edges;
    Graph graph(verticies);
    for (int i = 0; i < edges; ++i) {
        std::cin >> from >> to;
        graph.add(from, to);
    }
    std::cout << graph.solution();
    return 0;
}