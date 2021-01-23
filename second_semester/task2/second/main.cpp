/*
 * Рику необходимо попасть на межвселенную конференцию.
 * За каждую телепортацию он платит бутылками лимонада,
 * поэтому хочет потратить их на дорогу как можно меньше
 * (он же всё-таки на конференцию едет!).
 * Однако после K перелетов подряд Рика начинает сильно тошнить,
 * и он ложится спать на день. Ему известны все существующие телепортации.
 * Теперь Рик хочет найти путь (наименьший по стоимости в бутылках лимонада),
 * учитывая, что телепортация не занимает времени,
 * а до конференции осталось 10 минут (то есть он может совершить не более K перелетов)!
 *
 * Формат ввода
 *
 * В первой строке находятся количество вселенных N,
 * количество существующих телепортаций M, количество перелетов подряд K,
 * номер вселенной Рика S, номер вселенной конференции F
 * (2 ≤ N ≤ 300, 1 ≤ M ≤ 105, 1 ≤ K ≤ 300, 1 ≤ S ≤ N, 1 ≤ F ≤ N).
 *
 * Далее идет M строк, задающих телепортации. i-я строка содержит три натуральных числа:
 * Si, Fi и Pi, где Si - номер вселенной, из которой ведет i-я телепортация,
 * Fi - номер вселенной, в которую ведет i-я телепортация, Pi - стоимость i-й телепортации.
 * 1 ≤ Si ≤ N, 1 ≤ Fi ≤ N, 1 ≤ Pi ≤ 106.
 *
 * Формат вывода
 *
 * Выведите одно число - минимальную стоимость пути, подходящего для профессора.
 * Если профессор не сможет за K ночей добраться до конференции, выведите число -1.
 *
 * Примечания
 *
 * Не существует телепортов, ведущих из одной вселенной в неё же саму,
 * в рамках этого путешествия ;)
 */
#include <iostream>
#include <vector>
#include <queue>

#define INF 100000000000

/*
 *  В класс граф описана
 *      Инициализация
 *      Добавление пути и его стоимости
 *      Совершение К шагов
 *  Совершаем К шагов обходом в ширину и находим наименьший возможный ответ
 *  Если результат не изменился или очередь опустела раньше времени, то пути
 *  за к шагов до места назначения нет
 */

class Graph {
private:
    std::vector<std::vector<std::pair<long long, long long>>> ways;
public:
    explicit Graph(const long long counter = 0) {
        ways.resize(counter + 1);
    }
    void add_path(const long long &from, const long long &to, const long long &cost) {
        ways[from].push_back(std::pair<long long, long long>(to, cost));
    }
    long long int make_k_moves(const long long &K, const long long &F, const long long &S) {
        long long counter = 0, current_wave = 1, tmp;
        std::vector<long long int> results(ways.size(), INF);
        std::queue<long long> points;
        points.push(S);
        results[S] = 0;
        while (counter < K && !points.empty()) {
            --current_wave;
            tmp = points.front();
            points.pop();

            for (std::pair<long long, long long> way: ways[tmp]) {
                if (results[tmp] + way.second < results[way.first]) {
                    results[way.first] = results[tmp] + way.second;
                    points.push(way.first);
                }
            }
            if (current_wave == 0) {
                current_wave = points.size();
                ++counter;
            }
        }
        return results[F] == INF ? -1 : results[F];
    }
};

int main() {
    long long N, M, K, S, F, p, s, f;
    std::cin >> N >> M >> K >> S >> F;
    Graph graph(N);
    for (size_t i = 0; i < M; ++i) {
        std::cin >> s >> f >> p;
        graph.add_path(s, f, p);
    }
    std::cout << graph.make_k_moves(K, F, S);
    return 0;
}
