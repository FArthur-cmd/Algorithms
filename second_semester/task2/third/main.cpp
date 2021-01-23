/*
 * Сейчас Рику надо попасть из вселенной с номером S во вселенную с номером F.
 * Он знает все существующие телепорты, и казалось бы нет никакой проблемы.
 * Но, далеко не секрет, что за свою долгую жизнь Рик поссорился много с кем.
 * Из своего личного опыта он знает, что при телепортациях есть вероятность,
 * что его заставят ответить за свои слова.
 *
 * Если Рик знает вероятности быть прижатым к стенке на всех существующих телепортациях,
 * помогите ему посчитать минимальную вероятность, что он всё-таки столкнется с неприятностями.
 *
 * Формат ввода
 *
 * В первой строке даны количество вселенных N (2 ≤ N ≤ 100),
 * количество существующих телепортаций M (1 ≤ M ≤ N ⋅ (N - 1) / 2),
 * стартовая вселенная S (1 ≤ S ≤ N) и конечная вселенная F (1 ≤ F ≤ N).
 *
 * Далее в M строках дается описание телепортаций в формате Si, Fi, Pi,
 * где Si и Fi — номера вселенных,
 * которые связывает i-я телепортация (телепорты двунаправленные, 1 ≤ Si, Fi ≤ N),
 * Pi — вероятность быть избитым на i-й телепортации в процентах (0 ≤ Pi ≤ 100).
 *
 * Формат вывода
 *
 * Вероятность быть избитым с точностью до 10^6.
 *
 * Примечания
 *
 * В этом случае опять же считается, что не бывает телепортов из вселенной в неё же саму,
 * как и двух одинаковых телепортов (то есть с одинаковыми начальной и конечной вселенной).
 * Гаранитруется, что путь между S и F существует всегда.
 */
#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>

/*
 * Граф содержит
 *     Инициализацию
 *     Добавление пути (в мэйне вызывается в обе стороны)
 *     Нахождение той самой вероятности
 * Добавляем не вероятность быть побитым, а вероятность быть не битым
 * тогда находим наибольшую веротяность ни разу не быть побитым и ответом
 * будет 1 - найденная вероятность
 */
class Graph {
private:
    std::vector<std::vector<std::pair<long long, long long>>> ways;
public:
    explicit Graph(long long counter = 0) {
        ways.resize(counter + 1);
    }
    void add_path(long long from, long long to, long long verity) {
        ways[from].push_back(std::pair<long long, long long>(to, 100 - verity));
    }
    double find_verity(const long long &Start, const long long &Finish) {
        std::queue<long long> points;
        std::vector<double> verities(ways.size());
        points.push(Start);
        verities[Start] = 1.0;
        long long CurrPoint;
        while (!points.empty()) {
            CurrPoint = points.front();
            points.pop();
            for (auto way: ways[CurrPoint]) {
                double tmp = static_cast<double>(way.second) / 100 * verities[CurrPoint];
                if (verities[way.first] < tmp) {
                    verities[way.first] = tmp;
                    if (way.first != Finish)
                        points.push(way.first);
                }
            }
        }
        return verities[Finish];
    }
};

int main() {
    long long N, M, S, F, s, f, p;
    std::cin >> N >> M >> S >> F;
    Graph graph(N);
    for (int i = 0; i < M; ++i) {
        std::cin >> s >> f >> p;
        graph.add_path(s, f, p);
        graph.add_path(f, s, p);
    }
    std::cout << std::setprecision(6) << 1 - graph.find_verity(S, F);
    return 0;
}