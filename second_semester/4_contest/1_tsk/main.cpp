/*
 *

Дано число N и последовательность из N целых чисел. Найти вторую порядковую статистику на заданных диапазонах.

Для решения задачи используйте структуру данных Sparse Table. Требуемое время обработки каждого диапазона O(1).
 Время подготовки структуры данных .
Формат ввода

    В первой строке заданы 2 числа: размер последовательности N и количество диапазонов M.

    Следующие N целых чисел задают последовательность. Далее вводятся M пар чисел - границ диапазонов.

    Гарантируется, что каждый диапазон содержит как минимум 2 элемента.

Формат вывода

Для каждого из M диапазонов напечатать элемент последовательности - 2ю порядковую статистику. По одному числу в строке.
 */

#include <iostream>
#include <vector>

/*
 * Немного слов об Алгоритме:
 *  Усовершенствуем немного Spare Table, будем хранить два минимума
 *  Зачем? Потому что на отрезке надо найти второй минимум, а он может быть одним из 4 чисел:2 минимумов 1 подотрезка
 *  или 2 минимумов 2 подотрезка
 *  Асимптотика построения та же, отличие происходит лишь в том, как заполняем(на каждом шаге у нас есть не более
 *  8 сравнений(если учитывать сравнения индексов), поэтому О(8 n log(n)) = O(n log(n)) - требуемая
 *  В ответе у нас происходят лишь обращения к памяти и максимум 8 сравнений, то есть О(8)=О(1) - требуемая
 */

class SpareTableSecondMin {
private:
    std::vector<std::vector<std::pair< long long,  long long>>> st_of_two_mins; //та самая таблица
    std::vector< long long> log, degree, points; //вспомогательные массивы логорифмов и степеней 2, чтобы не считать, и числа
public:
    SpareTableSecondMin(const  long long &size) {
        points.resize(size + 1);
        FillAdditionalArrays(size); //на этом шаге уже можно заполнить вспомогательные массивы
    }
    void AddPoint(const  long long& index, const  long long &value) {
        points[index] = value;
    }
    void FillAdditionalArrays(const  long long &size) {
        log.resize(size + 1, -1);
        for( long long i = 1; i <= size; ++i) {
            log[i] = FindLog(i);
        }
        degree.resize(log[size] + 1);
        degree[0] = 1;
        for( long long i = 1; i <= log[size]; ++i) { //не более Log(size) степеней двойки нужны
            degree[i] = degree[i-1] * 2;
        }
    }
    static  long long FindLog(const  long long& number) {
        return number == 1 ? 0: FindLog(number/2) + 1;
    }
    //Сравниваем 4 значения и находим среди них 2 минимума
    std::pair<long long, long long> FindTwoMins(const std::pair< long long,  long long>& first_indexes,
                                    const std::pair< long long,  long long>& second_indexes) {
         long long first_point = points[first_indexes.first], second_point = points[first_indexes.second],
        third_point = points[second_indexes.first], fourth_point = points[second_indexes.second];
        //Если какие-то индексы совпадают, их можно повторно не смотреть
        bool first_different = (second_indexes.first != first_indexes.first &&
                second_indexes.first != first_indexes.second);
        bool second_different = (second_indexes.second != first_indexes.first &&
                               second_indexes.second != first_indexes.second);
        long long min1 = first_point <= second_point ? first_indexes.first: first_indexes.second,
        min2 = (min1 == first_indexes.first)? first_indexes.second: first_indexes.first;
        if (first_different) {
            if (third_point < points[min1]) {
                min2 = min1;
                min1 = second_indexes.first;
            } else if (third_point < points[min2]) {
                min2 = second_indexes.first;
            }
        }
        if (second_different) {
            if (fourth_point < points[min1]) {
                min2 = min1;
                min1 = second_indexes.second;
            } else if (fourth_point < points[min2]) {
                min2 = second_indexes.second;
            }
        }
        return std::make_pair(min1, min2);
    }
    //Построение таблицы
    void MakeSpareTable(const long long &size) {
        st_of_two_mins.assign(log[size] + 1, std::vector<std::pair<long long, long long>>(size, {-1, -1}));
        for (long long i = 0; i < size - 1; ++i) {
            st_of_two_mins[1][i] = points[i] < points[i+1] ? std::make_pair(i, i + 1): std::make_pair(i + 1, i);
        }
        for (long long i = 2; i < log[size] + 1; ++i) {
            for (long long j = 0; j < size; ++j) {
                if (j + degree[i]/2 < size) {
                    st_of_two_mins[i][j] = FindTwoMins(st_of_two_mins[i-1][j],
                            st_of_two_mins[i-1][j + degree[i]/2]);
                }
            }
        }
    }
    //ответ. Если в диапазоне 1 число - его и вернем, иначе найдем 2 статистику
    long long Answer(const long long& first_index, const long long& second_index) {
        if (second_index - first_index == 1) {
            return points[st_of_two_mins[1][first_index].second];
        }
        long long degree_koefficient = log[second_index - first_index +1];
        return points[FindTwoMins(st_of_two_mins[degree_koefficient][first_index],
                st_of_two_mins[degree_koefficient][second_index - degree[degree_koefficient] + 1]).second];
    }
};

int main() {
    long long N, M, value, left, right;
    std::cin >> N >> M;
    SpareTableSecondMin Solution(N);
    for (long long i = 0; i < N; ++i) {
        std::cin >> value;
        Solution.AddPoint(i, value);
    }
    Solution.MakeSpareTable(N);
    for (long long i = 0; i < M; ++i) {
        std::cin >> left >> right;
        std::cout << Solution.Answer(left-1, right-1) << std::endl;
    }
}