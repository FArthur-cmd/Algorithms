/**
 * Дана строка длины n. Найти количество ее различных подстрок.
 * Используйте суффиксный массив.
 *
 * Построение суффиксного массива выполняйте за O(n log n).
 * Вычисление количества различных подстрок выполняйте за O(n).
 *
 */
#include <iostream>
#include <cstdint>
#include <string>
#include <vector>

const uint8_t ALPHABET_SIZE 255

/**
 *  Суффиксный массив.
 *  Может построить (как это не странно) суффиксный массив и lcp суффиксов
 *  Так же может посчитать количество разных подстрок в слове.
 */
class suffix_array {
private:
    std::string word_; // слово, для которого будет все выполняться
    std::vector <uint32_t> places_; //тот самый суффиксный массив
    std::vector <uint32_t> classes_;//классы эквивалентности
    std::vector <uint32_t> counter_; // счетчик для сортировки
    std::vector <uint32_t> lcp_array_; // lcp суффиксов
    std::vector <uint32_t> reversed_places_; // массив, обратный суффиксному
    int32_t classes_index_ = 1;
    bool lcp_was_built = false;//проверяем построен ли lcp массив
    // (для вычисления разного количества слов)

    void preparing_for_suffix(); //подготовка к построению суф-массива

    void make_classes(); //создание классов эквивалентности

    void creating_suffix_array();// Построение суф-массива

    void prepare_for_lcp();//подготовка к построению lcp

    void count_lcp();//построение lcp

public:
    explicit suffix_array(std::string word);

    void make_lcp_array(); //создать lcp

    uint64_t calculate_different_patterns_in_word(); // посчитать количество подслов

    const std::vector <uint32_t> &get_array() const; //получить суфф массив

    const std::vector <uint32_t> &get_lcp(); // получить массив lcp

};

suffix_array::suffix_array(std::string word) :
        places_(word.length() + 1, 0),
        counter_(ALPHABET_SIZE, 0),
        classes_(word.length() + 1, 0) {
    word_ = std::move(word);
    lcp_was_built = false;
    word_.push_back(0);
    preparing_for_suffix();
    make_classes();
    creating_suffix_array();
}

// Нулевая фаза. Создаем массивы и отсортируем циклические подстроки
// длинной 1
void suffix_array::preparing_for_suffix() {
    for (char i : word_) {
        ++counter_[i];
    }
    for (int32_t i = 1; i < ALPHABET_SIZE; ++i) {
        counter_[i] += counter_[i - 1];
    }
    for (int32_t i = 0; i < word_.length(); ++i, --counter_[word_[i]]) {
        places_[counter_[word_[i]]] = i;
    }
}

// создадим классы эквивалентности
void suffix_array::make_classes() {
    classes_[places_[0]] = 0;
    for (uint32_t i = 1; i < word_.length(); ++i) {
        if (word_[places_[i]] != word_[places_[i - 1]]) {
            ++classes_index_;
        }
        classes_[places_[i]] = classes_index_ - 1;
    }
}

//Строим за O(n log n)так как рассматриваем части длинной 2^k, используя посчитанные ранее результаты.
// то есть log(n) итераций по n сравнений. Всего O(n log n)
void suffix_array::creating_suffix_array() {
    std::vector <uint32_t> places_by_next_part(word_.length(), 0);
    std::vector <uint32_t> new_classes(word_.length(), 0);
    for (uint32_t j = 0; (1u << j) < word_.length(); ++j) {
        new_classes.resize(word_.length()); //без этой строки будет 0 размер после move
        for (uint32_t i = 0; i < word_.length(); ++i) {
            places_by_next_part[i] = (places_[i] + word_.length() - (1u << j)) % word_.length();
        }
        counter_.assign(word_.length(), 0); //обновляем счетчик
        for (uint32_t i = 0; i < word_.length(); ++i) {
            ++counter_[classes_[places_by_next_part[i]]];
        }
        for (uint32_t i = 1; i < classes_index_; ++i) {
            counter_[i] += counter_[i - 1];
        }
        for (uint32_t i = word_.length(); i > 0; --i) {
            places_[--counter_[classes_[places_by_next_part[i - 1]]]] = places_by_next_part[i - 1];
        }
        new_classes[places_[0]] = 0;
        classes_index_ = 1;
        for (uint32_t i = 1; i < word_.length(); ++i) {
            uint32_t middle_first = (places_[i] + (1u << j)) % word_.length();
            uint32_t middle_second = (places_[i - 1] + (1u << j)) % word_.length();
            if (classes_[places_[i]] != classes_[places_[i - 1]] || classes_[middle_first] != classes_[middle_second]) {
                ++classes_index_;
            }
            new_classes[places_[i]] = classes_index_ - 1;
        }
        classes_ = std::move(new_classes);
    }
    for (uint64_t i = 1; i < places_.size(); ++i) {
        places_[i - 1] = places_[i];
    }
    places_.resize(places_.size() - 1);
    word_.pop_back();
}

void suffix_array::make_lcp_array() {
    prepare_for_lcp();
    count_lcp();
}

void suffix_array::prepare_for_lcp() {
    lcp_array_.resize(word_.length(), 0);
    reversed_places_.resize(word_.length(), 0);
    for (uint32_t i = 0; i < word_.length(); ++i) {
        reversed_places_[places_[i]] = i;
    }
}

// По алгоритму Касаи. Асимптотика О(n)
void suffix_array::count_lcp() {
    uint32_t counter_of_lcp = 0;
    uint32_t current = 0;
    for (uint32_t i = 0; i < word_.length(); ++i) {
        if (counter_of_lcp > 0) {
            --counter_of_lcp;
        }
        if (reversed_places_[i] == word_.length() - 1) {
            lcp_array_[word_.length() - 1] = 0;
            counter_of_lcp = 0;
        } else {
            current = places_[reversed_places_[i] + 1];
            while (std::max(i + counter_of_lcp, current + counter_of_lcp) < word_.length()
                   && word_[i + counter_of_lcp] == word_[current + counter_of_lcp]) {
                ++counter_of_lcp;
            }
            lcp_array_[reversed_places_[i]] = counter_of_lcp;
        }
    }
}

uint64_t suffix_array::calculate_different_patterns_in_word() {
    if (!lcp_was_built) {
        make_lcp_array();
    }
    uint64_t sum = 0;
    for (uint32_t i = 0; i < lcp_array_.size(); ++i) {
        sum += word_.length() - places_[i] - lcp_array_[i];
    }
    return sum;
}

// получим доступ к суффиксному массиву
const std::vector <uint32_t> &suffix_array::get_array() const {
    return places_;
}

// получим доступ к lcp массиву
const std::vector <uint32_t> &suffix_array::get_lcp() {
    if (!lcp_was_built) {
        make_lcp_array();
    }
    return lcp_array_;
}

int main() {
    std::string word;
    std::cin >> word;
    suffix_array array(word);
    std::cout << array.calculate_different_patterns_in_word() << std::endl;
    return 0;
}

