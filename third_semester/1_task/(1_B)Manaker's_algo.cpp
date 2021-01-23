/**
 * Строка называется палиндромом, если она одинаково читается как слева направо,
 * так и справа налево. Например, «abba» – палиндром, а «roman» – нет.
 *
 * Для заданной строки s длины n (1 ≤ n ≤ 105) требуется подсчитать число пар (i, j),
 * 1 ≤ i < j ≤ n, таких что подстрока s[i..j] является палиндромом.
 *
 * Формат ввода
 * Входной файл содержит одну строку s длины n, состоящюю из маленьких латинских букв.
 *
 * Формат вывода
 * В выходной файл выведите искомое число подстрок-палиндромов.
 */
#include <iostream>
#include <string>
#include <cinttypes>
#include <vector>

/*
 * Унифицированный алгоритм Манакера.
 * Так как отличий между четными и нечетными полиндромами мало, чтобы
 * избежать дублирования кода введем флаг, с помощью которого
 * будем отличать режим, в котором надо работать и вносить соответствующие
 * изменения в выичсления
 */
template <typename T>
std::vector<T> Manakers_algo(std::string& word, bool even=true) {
    std::vector<T> answer(word.size(), 0);
    int64_t left = 0, right = -1;
    int64_t radius;
    for (int64_t i = 0; i < word.size(); ++i) {
        if (i > right) {
            radius = 1 - even;
        } else if (answer[left + right - i + even] < right - i + 1) {
            radius = answer[left + right - i + even];
        } else {
            radius = right - i + 1;
        }
        while (i + radius < word.size() &&
               i - radius - even >= 0 &&
               word[i + radius] == word[i - radius - even])
            ++radius;
        answer[i] = radius;
        if (i + radius - 1 > right) {
            left = i - radius + 1 - even;
            right = i + radius - 1;
        }
    }
    return answer;
}

/*
 * Просто посчитаем алгоритмом Манакера количество полиндромов
 * четной и нечетной длины. Каждое значение радиуса - количество пар индексов
 * Это легко заметить 123321 - радиус 3 и соответствующие 3 пары индексов
 * В случае нечетных в силу алгоритма просто символ будет иметь радиус 1
 * Чтобы его не учитывать вычитаем единицу
 */
uint64_t solution(std::string& word) {
    std::vector<uint64_t> odd_length_palyndroms = Manakers_algo<uint64_t>(word, false);
    std::vector<uint64_t> even_length_palyndroms = Manakers_algo<uint64_t>(word);
    uint64_t sum = 0;
    for (int64_t i = 0; i < word.size(); ++i) {
        sum += even_length_palyndroms[i];
        sum += odd_length_palyndroms[i] - 1;
    }
    return sum;
}

int main() {
    std::string word;
    std::cin >> word;
    std::cout << solution(word);
    return 0;
}
