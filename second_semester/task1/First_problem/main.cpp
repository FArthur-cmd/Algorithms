/*
 Леон и Матильда собрались пойти в магазин за молоком.
 Их хочет поймать Стэнсфилд, поэтому нашим товарищам нужно
 cделать это как можно быстрее. Каково минимальное количество улиц,
 по которым пройдёт хотя бы один из ребят
 (либо Матильда, либо Леон, либо оба вместе)?

 Формат ввода
 Первая строка содержит количество вершин n (1 ≤ n ≤ 105),
 количество ребер m (0 ≤ m ≤ 105)
 и номера вершин графа leon, matilda, milk,
 в которых находятся соответственно Леон, Матильда и магазин с молоком.

 Следующие m строк содержат ребра графа. В каждой строке два числа,
 разделенные пробелом, если между i и j существует ребро.
 Гарантируется, что в графе нет петель и мультиребер.

 Формат вывода
 Одно число – количество ребер.
*/
#include <iostream>
#include <vector>
#include <queue>

/*
 * class Graph
 * содержит все пути в графе
 * add_connection добавляет путь между
 * двумя вершинами
 * BFS выполняет поиск в ширину
 * solution ищет решение задачи
 * Для этого она выполняет BFS для
 * 3 точек, затем ищет точку,
 * суммарное расстояние от которой до
 * всех точек минимально, оно и будет
 * искомым решением
 * Асимптотика O(V+E)
 * (BFS) O(V+E)
 * Проход по всем вершинам O(V) < O(V+E)
 * 3 * O(V+E) + O(V) = O(V+E)
 * */
class Graph {
private:
    std::vector<std::vector<int>> ways;

public:
    Graph(int size = 0) {
        ways.resize(size + 1);
    }
    void add_conection(const int &from, const int &to) {
        ways[from].push_back(to);
        ways[to].push_back(from);
    }
    int solution(const int&leon, const int&matilda, const int&milk) {
        std::vector<std::vector<int> > numbers;
        numbers.resize(3);
        for (int i = 0; i < 3; i++) {
            numbers[i].resize(ways.size());
        }
        BFS(leon, numbers[0]);
        BFS(matilda, numbers[1]);
        BFS(milk, numbers[2]);
        int min = 99999999, tmp = 0;
        for (int i = 1; i < ways.size(); i++) {
            tmp = numbers[0][i] + numbers[1][i] + numbers[2][i];
            if (tmp < min) {
                min = tmp;
            }
        }
        return min;
    }
    void BFS(const int &first_point, std::vector<int> &numbers) {
        std::vector<int> colour;
        colour.resize(ways.size());
        std::queue<int> work_with;
        work_with.push(first_point);
        colour[first_point] = 1;
        int that_wave = 1, next_wave = 0,
                curr_wave = 1;
        while (!work_with.empty()) {
            int tmp = work_with.front();
            work_with.pop();
            --that_wave;
            for (int i : ways[tmp]) {
                if (colour[i] == 0) {
                    numbers[i] += curr_wave;
                    colour[i] = 1;
                    work_with.push(i);
                    ++next_wave;
                }
            }
            if (that_wave == 0) {
                that_wave = next_wave;
                next_wave = 0;
                ++curr_wave;
            }
        }
    }
};
/* Считываем входные данные
 * Задаем граф и его ребра
 * Запускаем поиск ответа
 */
int main() {
    int number_of_verticies, number_of_edges, matilda,
            leon, milk, edge_from, edge_to;
    std::cin >> number_of_verticies >> number_of_edges
             >> leon >> matilda >> milk;
    Graph graph(number_of_verticies);
    for (int i = 0; i < number_of_edges; i++) {
        std::cin>> edge_from >> edge_to;
        graph.add_conection(edge_from, edge_to);
    }
    std::cout << graph.solution(leon, matilda, milk);
    return 0;
}
