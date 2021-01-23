/*
Требуется найти в связном графе остовное дерево минимального веса.

Воспользуйтесь алгоритмом Прима.
Формат ввода

Первая строка входного файла содержит два натуральных числа n и m — количество вершин и ребер графа соответственно.
 Следующие m строк содержат описание ребер по одному на строке. Ребро номер i описывается тремя натуральными числами
 bi, ei и wi — номера концов ребра и его вес соответственно (1 ≤ bi, ei ≤ n, 0 ≤ wi ≤ 100 000). n ≤ 5 000, m ≤ 100 000.

Граф является связным.
Формат вывода

Первая строка выходного файла должна содержать одно натуральное число — вес минимального остовного дерева.
 */
#include <iostream>
#include <vector>
//INF Больше чем любое допустимое значение пути по условию. Означает отсутствие пути
#define INF 1000000
/*
 * Класс Граф
 * Содержит стоимости путей в виде матрицы смежностей
 * ИНФ - отсутствие пути
 * В самом алгоритме Прима:
 *  Смотрим использована вершина или нет(лежит ли оно в подмоножестве уже построенной части остовного дерева или нет)
 *  min_edge Обозначает минимальное значение ребра из вершины до не использованной вершины
 *  В самом алгоритме. Находим точку с минимальным путем в неиспользованную часть. Добавляем это ребро в наше остовное
 *  древо (согласно алгоритму Прима, мы будем получать подмножество остовного дерева после каждого шага)
 *  После добавления вершины, считаем расстояния из нее до других, то есть заполняем min_edge для добавленной вершины
 *  Делаем n шагов, потому что на первом шаге мы добавим стартовую вершину(стартуем из 1, стартовая верщина не имеет
 *  значения) и затем добавляем n-1 ребро из остовного дерева
 */
class Graph {
private:
    std::vector <std::vector<int>> ways;
public:
    Graph(size_t size = 0) {
        ways.resize(size + 1);
        for (size_t i = 0; i < size + 1; ++i) {
            ways[i].resize(size + 1, INF);
        }
    }
    void add_connection(const int& from, const int& to, const int& cost) {
        ways[from][to] = cost;
        ways[to][from] = cost;
    }
    long long Prim_find_MST_sum() {
        std::vector<bool> used(ways.size());
        std::vector<int> min_edge(ways.size(), INF);
        long long int sum = 0;
        min_edge[1] = 0;
        for (size_t i = 1; i < ways.size(); ++i) {
            int from = 1;
            for (size_t j = 1; j < ways.size(); ++j)
                if (!used[j] && min_edge[j] < min_edge[from])
                    from = j;
            used[from] = true;
            sum += min_edge[from];
            min_edge[from] = INF;
            for (int to = 1; to < ways.size(); ++to)
                if (!used[to] && ways[from][to] < min_edge[to])
                    min_edge[to] = ways[from][to];
        }
        return sum;
    }
};

int main() {
    size_t n,m;
    std::cin >> n >> m;
    Graph graph(n);
    int from, to, cost;
    for (size_t i = 0; i < m; ++i) {
        std::cin >> from >> to >> cost;
        graph.add_connection(from, to, cost);
    }
    std::cout << graph.Prim_find_MST_sum();
    return 0;
}
