/*
 *
У художника-авангардиста есть полоска разноцветного холста. За один раз он перекрашивает некоторый отрезок полоски в 
 некоторый цвет. После каждого перекрашивания специально обученный фотограф делает снимок части получившегося творения 
 для музея современного искусства. Для правильного экспонирования требуется знать яркость самого темного цвета на 
 выбранном фотографом отрезке. Напишите программу для определения яркости самого темного цвета на отрезке.

Требуемая скорость определения яркости — O(log N).
Цвет задается тремя числами R, G и B (0 ≤ R, G, B ≤ 255), его яркость = R + G + B.
Цвет (R1, G1, B1) темнее цвета (R2, G2, B2), если R1 + G1 + B1 < R2 + G2 + B2.
Формат ввода

Первая строка содержит число N –– длина отрезка (координаты [0...N-1]).
Затем следует N строк по 3 числа –– цвета отрезка.
Следующая строка содержит число K –– количество перекрашиваний и фотографирований.
Затем задано K строк, описывающих перекрашивание и фотографирование.
В каждой такой строке 7 натуральных чисел: C, D, R, G, B, E, F, где [C, D] –– координаты перекрашиваемого отрезка, 
 (R, G, B) –– цвет покраски, [E, F] –– фотографируемый отрезок.
Формат вывода

Требуется вывести K целых чисел. Каждое число –– яркость самого темного цвета на отрезке [E, F] после 
 соответствующего окрашивания. 
 */
#include <iostream>
#include <vector>

#define ull unsigned long long
#define INF 100000000
/*
 * Суть алгоритма: используем дерево отрезков. Храним его в виде массива
 * строим из начального массива
 * для поиска минимума:
 *  Если у нас нарушились границы - выходим
 *  Если границы совпали - ответ получен. Иначе заходим в парвую и левую половины и ищем ответ там
 * Асимптотика log, так как глубина рекурсии log
 *
 * Аналогично для добавления
 * нарушены границы - выход
 * совпали - ответ
 * иначе заходим в половинки
 *
 * Update Изменяет цвет согласно условию задачи(также работае за логарифм)
 *
 * По поводу хранения данных:
 *     size - ближайшая степень двойки к N
 *     в дереве отрезков храним текущие значение и то, какой цвет надо установить
 */

class Tree{
private:
    std::vector<ull> tree, SettingColour;
    ull size;
public:
    Tree(const std::vector<ull>& start_array, ull size_) {
        size = size_;
        tree.assign(2 * size - 1, INF);
        SettingColour.assign(2 * size - 1, INF);
        for (ull i = 0; i < start_array.size(); ++i)
            tree[tree.size() / 2 + i] = start_array[i];
        for (long long i = tree.size() / 2 - 1; i >= 0; --i)
            tree[i] = std::min(tree[2*i + 1], tree[2*i + 2]);
    }
    ull MinBrightness(ull current_point, ull left, ull right, ull from, ull to) {
        UpdateData(current_point, left, right);
        if (from > to || left > right)
            return INF;
        if (from == left && to == right)
            return tree[current_point];

        ull middle = (left + right) / 2;
        ull left_min = MinBrightness(2 * current_point + 1, left,
                                     middle, from, std::min(to, middle));
        ull right_min = MinBrightness(2 * current_point + 2, middle + 1, right,
                                      std::max(from, middle + 1), to);
        return std::min(right_min, left_min);
    }
    void SetColour(ull current_point, ull left, ull right,
                   ull from, ull to, ull colour)  {
        UpdateData(current_point, left, right);
        if (from > to || left > right)
            return;
        if (from == left && to == right) {
            SettingColour[current_point] = colour;
            UpdateData(current_point, left, right);
            return;
        }
        ull middle = (left + right) / 2;
        SetColour(2 * current_point + 1, left, middle, from, std::min(to, middle), colour);
        SetColour(2 * current_point + 2, middle + 1, right, std::max(from, middle + 1), to, colour);
        tree[current_point] = std::min(tree[2 * current_point + 1], tree[2 * current_point + 2]);
    }
    void UpdateData(ull current, ull left, ull right) {
        if (SettingColour[current] != INF) {
            tree[current] = SettingColour[current];
            if (left != right) {
                SettingColour[current * 2 + 1] = SettingColour[current];
                SettingColour[current * 2 + 2] = SettingColour[current];
            }
            SettingColour[current] = INF;
        }
    }
};

ull FindLog(const ull& number) {
    return number == 1 ? 0: FindLog(number/2) + 1;
}

int main() {
    ull n, m, from, to, tree_size, r, g, b, picture_from, picture_to;
    std::cin >> n;
    std::vector<ull> start_array(n);
    for (ull i = 0; i < n; ++i) {
        std::cin >> r >> g >> b;
        start_array[i] = r + b + g;
    }
    tree_size = 1ull << (FindLog(start_array.size()) + 1); // Получение ближайшей степени двойки(беззнаково)
    Tree tree(start_array, tree_size);
    std::cin >> m;
    for (ull i = 0; i < m; ++i) {
        std::cin >> from >> to >> r >> g >> b >> picture_from >> picture_to;
        tree.SetColour(0, 0, tree_size - 1, from, to, r + b + g);
        std::cout << tree.MinBrightness(0, 0, tree_size-1, picture_from, picture_to) << " ";
    }
    return 0;
}