/**
 * Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
 * Найти позиции всех вхождений шаблона в тексте длины n. Каждое вхождение шаблона предполагает,
 * что все обычные символы совпадают с соответствующими из текста, а вместо символа “?” в тексте
 * встречается произвольный символ. Гарантируется, что сам “?” в тексте не встречается.
 *
 * Время работы - O(n + m + Z), где Z - общее число вхождений подстрок шаблона “между вопросиками”
 * в исходном тексте. m ≤ 5000, n ≤ 2000000.
 */
#include <iostream>
#include <vector>
#include <string>
#include <cinttypes>

/*
 * Алгоритм построения бора и рассчета суффиксных ссылок (вдохновлен emaxx с дополнением функциональности)
 */
class Algorithm {
private:
    // Узел бора, который мы будем строить:

    struct Node {
        uint16_t ALPHABET_SIZE = 26;
        bool is_terminate; // Является ли данный узел концом какого-либо паттерна
        int32_t parent;    // Родительский узел
        char parent_symbol;// символ, благодаря которому мы из родительского узла пришли сюда
        int32_t reference; // суффиксная ссылка (простая)
        std::vector<int32_t> way_to; //Куда мы можем попасть в боре из этой вершины
        std::vector<int32_t> go; // куда мы можем попасть в улучшенном с помощью ссылок боре
        std::vector<int32_t> compressed_suffix_reference; // все узлы, к которым можно попасть по
        // ссылкам из данного узла, при этом сюда будут занесены узлы, являющиеся концами паттернов
        // (немного видоизмененная сжатая суффиксная ссылка)
        std::vector<int32_t> leaf_pattern_number; //концом какого паттерна является данная вершина
        // Может возникнуть случай ab?ab тогда в b будет лежать 2 индекса и каждый учтется в программе
        // Это позволяет не потерять случаи, похожии на указанный пример
        Node();
    };

    std::vector<Node> bor;
public:
    Algorithm();

    void add_pattern(const std::string &word, int32_t pattern_number);

    int32_t go(int32_t node_from, int32_t symbol);

    int32_t get_reference(int32_t node_from);

    std::vector<int32_t> calculate_compressed_suffix_references(int32_t start = 0);

    const std::vector<Node> &get_bor();

    void finish_building();
};

Algorithm::Node::Node() : parent(-1), reference(-1), parent_symbol(0), is_terminate(false),
                          way_to(ALPHABET_SIZE, -1), go(ALPHABET_SIZE, -1) {}

Algorithm::Algorithm() {
    bor.emplace_back(Node()); // <- это корень
}

/*
 * Добавления паттерна в бор. Если нет перехода, создаем узел и заносим данные о родителе
 */
void Algorithm::add_pattern(const std::string &word, int32_t pattern_number) {
    int32_t start = 0;
    int32_t tmp_symbol;
    for (char i : word) {
        tmp_symbol = i - 'a';
        if (bor[start].way_to[tmp_symbol] == -1) {
            bor.emplace_back(Node());
            bor[bor.size() - 1].parent = start;
            bor[bor.size() - 1].parent_symbol = tmp_symbol;
            bor[start].way_to[tmp_symbol] = bor.size() - 1;
        }
        start = bor[start].way_to[tmp_symbol];
    }
    bor[start].is_terminate = true;
    bor[start].leaf_pattern_number.push_back(pattern_number);
}


// Перемещаемся либо если есть путь, либо смотрим, существует ли путь через ссылку
// запоминаем данные в go, чтобы потом не считать повторно эту рекурсию
int32_t Algorithm::go(int32_t node_from, int32_t symbol) {
    if (bor[node_from].go[symbol] == -1) {
        if (bor[node_from].way_to[symbol] != -1) {
            bor[node_from].go[symbol] = bor[node_from].way_to[symbol];
        } else if (node_from == 0) {
            bor[node_from].go[symbol] = 0;
        } else {
            bor[node_from].go[symbol] = go(get_reference(node_from), symbol);
        }
    }
    return bor[node_from].go[symbol];
}

// Если это корень или его сын - то ссылка будет корнем.
// Иначе переходим от ссылки родителя по символу, по которому мы пришли в этот узел
int32_t Algorithm::get_reference(int32_t node_from) {
    if (bor[node_from].reference == -1) {
        if (node_from == 0 || bor[node_from].parent == 0) {
            bor[node_from].reference = 0;
        } else {
            bor[node_from].reference = go(get_reference(bor[node_from].parent), bor[node_from].parent_symbol);
        }
    }
    return bor[node_from].reference;
}

// Проходим по ссылкам, запоминаем все узлы, являющиеся концом паттерна. Делаем это только 1 раз, поэтому
// Асимптотика O(n). (из-за того, что мы запоминаем результаты, мы сохраняем, то мы будем совершать только 1 проход
// по ссылками для каждой вершины)
std::vector<int32_t> Algorithm::calculate_compressed_suffix_references(int32_t start) {
    if (bor[start].compressed_suffix_reference.empty()) {
        int32_t current_vertex = start;
        while (current_vertex != 0) {
            if (bor[current_vertex].is_terminate) {
                bor[start].compressed_suffix_reference.push_back(current_vertex);
            }
            current_vertex = get_reference(current_vertex);
        }
    }
    return bor[start].compressed_suffix_reference;
}

// получаем доступ к приватному полю
const std::vector<Algorithm::Node> &Algorithm::get_bor() {
    return bor;
}

void Algorithm::finish_building() {
    for (int32_t i = 1; i < bor.size(); ++i)
        calculate_compressed_suffix_references(i);
}

/*
 * Пройдемся по паттерну и запомним следующие данные:
 *
 * количество вопросов в начале паттерна
 * количество вопросов в конце паттерна
 * индекс относительно начала слова(где кончились вопросы) для каждой части, в которой нет вопросов
 *
 * Строим бор
 */
struct source_information {
    int32_t starting_questions = 0;
    int32_t ending_questions = 0;
    std::string pattern;
    std::string text;
    Algorithm algorithm;
    std::vector<int32_t> pattern_lengths;
    std::vector<int32_t> answer;

    std::vector<int32_t> calculate_lengths_and_add_patterns();

// идем по тексту и увеличиваем на 1 элемент в массиве, где начинался бы найденный паттерн
// Легко заметить, что начала всех паттернов пересекутся в одной точке, если это будет
// тот самый паттерн, который мы ищем. (По построению совпадение всех индексов не возможно)
// Остается только учесть начальные и конечные вопросы
    void calculate_positions();

// Решение представляет собой запуск построения бора, вычисление сжатых ссылок, и проход по массиву в поисках ответа за
// O(m). Так как до этого был реализован алгоритм Ахо-Корасика(просто по частям), то асимптотическая сложность:
// O(n+m+z) + O(m) = O(m+n+z). Что и требовалось реализовать
    std::vector<int32_t> solve();
};

std::vector<int32_t> source_information::calculate_lengths_and_add_patterns() {
    std::string part_of_pattern;
    std::vector<int32_t> lengths;
    bool count_questions = true, first_questions = true;
    int32_t i = 0, question_counter = 0;
    while (i < pattern.size()) {
        if (count_questions) {
            if (pattern[i] != '?') {
                if (first_questions) {
                    starting_questions = question_counter;
                }
                first_questions = false;
                count_questions = false;
                part_of_pattern.push_back(pattern[i]);
                question_counter = 0;
            } else {
                ++question_counter;
            }
        } else {
            if (pattern[i] != '?')
                part_of_pattern.push_back(pattern[i]);
            else {
                question_counter = 1;
                count_questions = true;
                lengths.push_back(i);
                algorithm.add_pattern(part_of_pattern, lengths.size() - 1);
                part_of_pattern = "";
            }
        }
        ++i;
    }
    ending_questions = question_counter;
    if (!part_of_pattern.empty()) {
        lengths.push_back(pattern.size());
        algorithm.add_pattern(part_of_pattern, lengths.size() - 1);
    }
    algorithm.finish_building();
    return lengths;
}

void source_information::calculate_positions() {
    int32_t start = 0;
    int32_t current_symbol;
    answer.resize(text.size() + 1, 0);
    for (int32_t i = starting_questions; i < text.size() - ending_questions; ++i) {
        current_symbol = text[i] - 'a';
        start = algorithm.go(start, current_symbol);
        for (auto l :algorithm.get_bor()[start].compressed_suffix_reference) {
            for (auto j: algorithm.get_bor()[l].leaf_pattern_number) {
                if (i - pattern_lengths[j] + 1 >= 0)
                    ++answer[i - pattern_lengths[j] + 1];
            }
        }
    }
}

std::vector<int32_t> source_information::solve() {
    pattern_lengths = calculate_lengths_and_add_patterns();
    calculate_positions();
    std::vector<int32_t> result;
    for (int32_t i = 0; i < answer.size() - ending_questions; ++i) {
        if (answer[i] == pattern_lengths.size()) {
            result.push_back(i);
        }
    }
    return result;
}

int main() {
    source_information solution;
    std::cin >> solution.pattern >> solution.text;
    std::vector<int32_t> answer = solution.solve();
    for (int32_t i : answer) {
        std::cout << i << " ";
    }
    return 0;
}
