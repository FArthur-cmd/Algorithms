/*
 *
ООО "Новые Железные Дороги" поручило вам разработать систему бронирования билетов на новый маршрут поезда дальнего
 следования. Маршрут проходит через N станций, занумерованных от 0 до N-1. Вместимость поезда ограничена.
В систему бронирования последовательно приходят запросы от пассажиров с указанием номера начальной и конечной станции,
 а также количество билетов, которые пассажир хочет приобрести.
Требуемая скорость обработки каждого запроса - O(logN).
Формат ввода

Первая строка содержит число N –– количество станций (1 ≤ N ≤ 50 000).

Вторая строка содержит начальное количество проданных билетов.
В примере продано по одному билету на перегонах 0->1 и 4->5.

В третьей строке указана вместимость поезда. В четвёртой — количество запросов на бронирование.
Далее идут непосредственно запросы в виде: номер начальной станции, номер конечной станции, количество запрашиваемых
 билетов.
Формат вывода

Необходимо вывести номера запросов, которые не удалось выполнить из-за отсутствия свободных мест.
 */
#include <iostream>
#include <vector>

#define ull unsigned long long
/*
 * Суть алгоритма: используем дерево отрезков. Храним его в виде массива
 * строим из начального массива
 * для поиска максимума:
 *  Если у нас нарушились границы - выходим
 *  Если границы совпали - ответ получен. Иначе заходим в парвую и левую половины и ищем ответ там
 * Асимптотика log, так как глубина рекурсии log
 *
 * Аналогично для добавления
 * нарушены границы - выход
 * совпали - ответ
 * иначе заходим в половинки
 *
 * По поводу хранения данных:
 *     size - ближайшая степень двойки к N
 *     в дереве отрезков храним пары - текущее значение и то, на сколько надо увеличить
 */

class SegmentTree{
private:
    std::vector<std::pair<ull, ull>> segment_tree;
    ull size;
public:
    SegmentTree(const std::vector<ull>& start_array, ull size_) {
        size = size_;
        segment_tree.assign(2 * size - 1, std::make_pair(0, 0));
        for (ull i = 0; i < start_array.size(); ++i)
            segment_tree[segment_tree.size() / 2 + i].first = start_array[i];
        for (long long i = segment_tree.size() / 2 - 1; i >= 0; --i)
            segment_tree[i].first = std::max(segment_tree[2*i + 1].first, segment_tree[2*i + 2].first);
    }
    ull MaxPassegers(ull current_point, ull left, ull right, ull from, ull to) {
        if (from > to || left > right)
            return 0;
        if (from == left && to == right)
            return segment_tree[current_point].first + segment_tree[current_point].second;

        ull middle = (left + right) / 2;
        ull left_max = MaxPassegers(2 * current_point + 1, left,
                                                   middle, from, std::min(to, middle));
        ull right_max = MaxPassegers(2 * current_point + 2, middle + 1, right,
                                                    std::max(from, middle + 1), to);
        return std::max(right_max, left_max) + segment_tree[current_point].second;
    }
    void AddNewPassegers(ull current_point, ull left, ull right,
                         ull from, ull to, ull tickets)  {
        if (from > to || left > right)
            return;
        if (from == left && to == right) {
            segment_tree[current_point].second += tickets;
            return;
        }
        ull middle = (left + right) / 2;
        AddNewPassegers(2 * current_point + 1, left, middle, from, std::min(to, middle), tickets);
        AddNewPassegers(2 * current_point + 2, middle + 1, right, std::max(from, middle + 1), to, tickets);
        segment_tree[current_point].first = std::max(segment_tree[2 * current_point + 1].first +
                                                       segment_tree[2 * current_point + 1].second,
                                                       segment_tree[2 * current_point + 2].first +
                                                       segment_tree[2 * current_point + 2].second);
    }
};

ull FindLog(const ull& number) {
    return number == 1 ? 0: FindLog(number/2) + 1;
}

int main() {
    ull n, train_size, m, from, to, tickets, tree_size;
    std::cin >> n;
    std::vector<ull> start_array(n);
    for (ull i = 0; i < n - 1; ++i) {
        std::cin >> start_array[i];
    }
    tree_size = 1ull << (FindLog(start_array.size()) + 1); // Получение ближайшей степени двойки(беззнаково)
    SegmentTree tree(start_array, tree_size);
    std::cin >> train_size;
    std::cin >> m;
    for (ull i = 0; i < m; ++i) {
        std::cin >> from >> to >> tickets;
        // Если при добавлении пассажиров будет превышение вместимости, то это искомый запрос
        // Выполняем до to-1 так как на последней остановке он освобождет место
        if (tree.MaxPassegers(0, 0, tree_size - 1, from, to - 1) + tickets > train_size) {
            std::cout << i << ' ';
        } else
            tree.AddNewPassegers(0, 0, tree_size - 1, from, to - 1, tickets);
    }
    return 0;
}