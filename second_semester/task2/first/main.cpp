/*
 * Условие задачи:
 *
 * Рик и Морти снова бороздят просторы вселенных, но решили ограничиться только теми,
 * номера которых меньше M. Они могут телепортироваться из вселенной с номером z во
 * вселенную (z+1) mod M за a бутылок лимонада или во вселенную (z2+1) mod M за b бутылок лимонада.
 * Рик и Морти хотят добраться из вселенной с номером x во вселенную с номером y.
 * Сколько бутылок лимонада отдаст Рик за такое путешествие, если он хочет потратить их как можно меньше?
 *
 * Формат ввода
 *
 * В строке подряд даны количество бутылок a за первый тип телепортации,
 * количество бутылок b за второй тип телепортации, количество вселенных M,
 * номер стартовой вселенной x, номер конечной вселенной y (0 ≤ a, b ≤ 100, 1 ≤ M ≤ 10^6, 0 ≤ x, y < M).
 *
 * Формат вывода
 *
 * Выведите одно число — количество бутылок лимонада, которые отдаст Рик за такое путешествие.
 */
#include <iostream>
#include <vector>
#include <deque>

#define INF 10000000000 // Значение не превышает макс значения М умноженного на максимальную стоимость пути
// равенство только в случае, когда прошли по всем планетам по наибольшей стоимости

/*
 * Реализация нахождения пути в 0-1 дереве
 * (так как дано 2 стоимости, то задачу можно свести к 0-1 путям,
 * вычитанием из обоих наименьшей и делением на оставшуюся положительную стоимость)
 *
 * Сам алгоритм заключается в добавлении пути с наименьшей стоисомтью в начало очереди, а с наибольшей стоимостью
 * в конец (доказательство корректности работы было у нас на семинаре)
 */
void find_path (const long long &x, const long long &a, const long long &b,
                const long long &y, const long long &M, std::vector<long long> &results) {
    std::deque<long long> watch_point;
    watch_point.push_back(x);
    results[x] = 0;
    long long current_point, next_point, distance;
    bool first_bigger_than_second = a > b;
    while (!watch_point.empty()) {
        current_point = watch_point.front();
        watch_point.pop_front();

        if (current_point == y)
            continue;

        next_point = (current_point + 1) % M;
        distance = results[current_point] + a;
        if (distance < results[next_point]) {
            results[next_point] = distance;
            if (first_bigger_than_second) {
                watch_point.push_back(next_point);
            } else {
                watch_point.push_front(next_point);
            }
        }
        next_point = (current_point * current_point + 1) % M;
        distance = results[current_point] + b;
        if (distance < results[next_point]) {
            results[next_point] = distance;
            if (!first_bigger_than_second) {
                watch_point.push_back(next_point);
            } else {
                watch_point.push_front(next_point);
            }
        }
    }

}

int main() {
    long long a, b, M, x, y;
    std::cin >> a >> b >> M >> x >> y;
    std::vector<long long> results(M, INF);
    find_path(x, a, b, y, M, results);
    std::cout << results[y];
    return 0;
}
