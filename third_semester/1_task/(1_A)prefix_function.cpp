/**
 * Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n.
 * Время O(n + p), доп. память – O(p).
 * p ≤ 30000, n ≤ 300000.
 * Использовать один из методов:
 * Вариант 1. С помощью префикс-функции;
 * Вариант 2. С помощью z-функции.
 * Формат ввода
 * Шаблон, символ перевода строки, строка.
 * Формат вывода
 * Позиции вхождения шаблона в строке.
 */
#include <iostream>
#include <string>
#include <vector>
/*
 *  Посчитаем префикс функцию для префикса.
 *  Странно? Тогда читаем объяснение дальше
 */
std::vector<size_t> calculate_prefix(const std::string& prefix) {
    size_t previous_position;
    std::vector<size_t> pi(prefix.size());
    pi[0] = 0;
    for (size_t i = 1; i < prefix.size(); ++i) {
        previous_position = pi[i - 1];
        while (previous_position > 0 && prefix[i] != prefix[previous_position]) {
            previous_position = pi[previous_position - 1];
        }
        if (prefix[i] == prefix[previous_position])
            ++previous_position;
        pi[i] = previous_position;
    }
    return pi;
}

/*
 * Алгоритм Морисса-Пратта, который работает за меньшее количество памяти.
 * Смысл в том, чтобы делать все, что в алгоритме, но считывать по символу за раз
 * Благодаря этому мы повторяем алгоритм, но не используем O(n+p) памяти,
 * а используем только O(p) памяти(что и требовалось в задаче)
 * Что касается эффективности, то мы выполняем в точности алгоритм
 * Морисса-Пратта, так что сложность O(n+p)
 */

void read_and_calculate_prefix(std::string& prefix) {
    char letter;
    size_t index, cur_pos = 0, last_match = 0;
    std::vector<size_t> pi = calculate_prefix(prefix);
    prefix += '\n';
    while (std::cin.get(letter)) {
        if (letter == '\n') continue;
        index = last_match;
        while (index > 0 && letter != prefix[index]) {
            index = pi[index - 1];
        }
        if (letter == prefix[index])
            ++index;
        last_match = index;
        if (index == prefix.size() - 1) { // нашли наш случай
            std::cout << cur_pos + 1 - index << " ";
        }
        ++cur_pos;
    }
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr);
    std::string pattern;
    std::cin >> pattern;
    read_and_calculate_prefix(pattern);
    return 0;
}
