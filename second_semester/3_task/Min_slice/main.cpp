/*
 *

Шрек и Фиона пригласили всех своих друзей на свою свадьбу. На церемонии они хотят рассадить их всех на две непустые
 части так, чтобы количество знакомств между двумя частями было минимальным. Всего приглашенных на свадьбу n, а каждое
 знакомство обоюдно.

Вам дан граф, в котором ребро означает знакомство между людьми. Помогите Шреку и Фионе поделить гостей на две непустые
 части.

 Формат ввода

В первой строке входного файла записано целое число n () — число гостей. Каждая из следующих n строк содержит по n
 символов. i-й символ j-й из этих строк равен «1», если между вершинами i и j есть ребро, и «0» в противном случае.
 Заданная таким образом матрица смежности является антирефлексивной (на главной диагонали стоят нули) и симметричной
 (относительно главной диагонали).

 Формат вывода

Выведите в выходной файл две строки. На первой выведите номера вершин, попавших в левую часть, через пробел,
 а на второй — номера вершин, попавших в правую часть, также через пробел. Номера вершин можно выводить в любом порядке.
 */
#include <iostream>
#include <vector>
#include <algorithm>
/*
 * Воспользуемся алгоримом Штор-Вагнера
 * асимптотика O(n^3)
 *
 * Для каждой вершины в массиве exist хранится, существует ли она, или она была объединена с какой-то другой вершиной.
 * В списке vertex[i] для каждой сжатой вершины i хранятся номера исходных вершин, которые были сжаты в эту вершину i.
 *
 * Алгоритм состоит из n-1 итерации. На каждой итерации сначала все вершины находятся вне множества разреза,
 * для чего массив in_cut  заполняется нулями, и связности connection всех вершин нулевые.
 * На каждой итерации находится вершина point_with_max_connection_with_cut. Если это итерация последняя, то ответ,
 * если надо, обновляется, а предпоследняя previous и последняя point_with_max_connection_with_cut
 * выбранные вершины объединяются в одну.
 * Если итерация не последняя, то point_with_max_connection_with_cut добавляется в множество разреза,
 * после чего пересчитываются веса всех остальных вершин.
 */
void Find_min_cut(std::vector<std::vector<int>>& ways, const int& size, std::vector<int>& min_cut) {
    int best_cost = 1000;
    int point_with_max_connection_with_cut;
    std::vector<int> connection(size);
    std::vector<bool> exist(size), in_cut(size);
    std::vector<std::vector<int>> vertex(size);
    for (int i = 0; i < size; ++i)
        vertex[i].resize(1, i);
    for(auto element: exist)
        element = true;
    for (int phase = 0; phase < size - 1; ++phase) {
        for(int& element: connection)
            element = 0;
        for(auto element: in_cut)
            element = false;
        for (int j = 0, previous = 0; j < size - phase; ++j) {
            point_with_max_connection_with_cut = -1;
            for (int i = 0; i < size; ++i)
                if (exist[i] && !in_cut[i] &&
                (point_with_max_connection_with_cut == -1 ||
                connection[i] > connection[point_with_max_connection_with_cut]))
                    point_with_max_connection_with_cut = i;
            if (j == size - phase - 1) {
                if (connection[point_with_max_connection_with_cut] < best_cost) {
                    best_cost = connection[point_with_max_connection_with_cut];
                    min_cut = vertex[point_with_max_connection_with_cut];
                }
                vertex[previous].insert (
                        vertex[previous].end(),
                        vertex[point_with_max_connection_with_cut].begin(),
                        vertex[point_with_max_connection_with_cut].end()
                        );
                for (int i = 0; i < size; ++i) {
                    ways[i][previous] += ways[point_with_max_connection_with_cut][i];
                    ways[previous][i] = ways[i][previous];
                }
                exist[point_with_max_connection_with_cut] = false;
            } else {
                in_cut[point_with_max_connection_with_cut] = true;
                for (int i = 0; i < size; ++i)
                    connection[i] += ways[point_with_max_connection_with_cut][i];
                previous = point_with_max_connection_with_cut;
            }
        }
    }
}

int main() {
    int n;
    std::vector<std::vector<int>> ways;
    std::vector<int> min_cut;
    std::cin >> n;
    ways.resize(n);
    for (auto &i: ways) {
        i.resize(n);
    }
    std::string symbol;
    for (int i = 0; i < n; ++i) {
        std::cin >> symbol;
        for (int j = 0; j < n; ++j) {
            ways[i][j] = symbol[j] - '0';
        }
    }
    Find_min_cut(ways, n, min_cut);
    for (auto i: min_cut){
        std::cout << i + 1 << " ";
    }
    std::cout << std::endl;
    std::vector<int> other;
    for(int i = 0; i < n; ++i) {
        if (std::find(min_cut.begin(), min_cut.end(), i) == min_cut.end()) {
            other.push_back(i + 1);
        }
    }
    for (auto i: other){
        std::cout << i << " ";
    }
    return 0;
}
