/**
 * Даны строки s и t. Постройте сжатое суффиксное дерево,
 * которое содержит все суффиксы строки s и строки t.
 * Найдите такое дерево, которое содержит минимальное
 * количество вершин.
 *
 *
 * Формат ввода
 *
 * В первой строке записана строка s (1 ≤ |s| ≤ 105),
 * последний символ строки равен `$',
 * остальные символы строки — маленькие латинские буквы.
 *
 * Во второй строке записана строка t (1 ≤ |t| ≤ 105),
 * последний символ строки равен `#',
 * остальные символы строки — маленькие латинские буквы.
 *
 *
 * Формат вывода
 *
 * Пронумеруйте вершины дерева от 0 до n-1 в порядке обхода в глубину,
 * обходя поддеревья в порядке лексикографической сортировки исходящих
 * из вершины ребер.
 * Используйте ASCII-коды символов для определения их порядка.
 * В первой строке выведите целое число n — количество вершин дерева.
 * В следующих n-1 строках выведите описание вершин дерева, кроме корня,
 * в порядке увеличения их номеров.
 *
 * Описание вершины дерева v состоит из четырех целых чисел:
 * p, w, lf, rg, где p (0 ≤ p < n, p ≠ v) — номер родителя текущей вершины,
 * w (0 ≤ w ≤ 1) — номер строки для определения подстроки на ребре.
 * Если w = 0, то на ребре, ведущем из p в v,
 * написана подстрока s[lf … rg - 1] (0 ≤ lf < rg ≤ |s|).
 * Если w = 1, то на ребре, ведущем из p в v,
 * написана подстрока t[lf … rg -1] (0 ≤ lf < rg ≤ |t|).
 */

#include <iostream>
#include <vector>
#include <stack>

const uint64_t UNDEFINED = 1ull << 63ull;//неопределенности заведем код,
//который точно не встретится в нашем алгоритме при построении
const uint64_t FIRST_ENDING = '$'; // Далее объявлены константы,
// смысл которых указан в названиях
const uint64_t SECOND_ENDING = '#';
const uint64_t ALPHABET_SIZE = 28;
const uint64_t FIRST_INDEX = 1;
const uint64_t SECOND_INDEX = 0;
const uint64_t MOVED_FROM_BEGIN = 2;

/**
 * Реализуем алгоритм Укконена
 * (см комментарии к реализации)
 */
class suffix_tree {
private:
    struct Node {
        uint64_t left; //левая граница содержания ребра
        uint64_t right; // правая граница содержания ребра
        uint64_t parent; //родитель
        uint64_t link; // ссылка
        uint64_t position; //отслеживание позиции в дереве с точки зрения дфс
        std::vector<uint64_t> way_to; // все пути из текущей вершины

        uint64_t belongs_to; // каким словам принадлежит данный узел

        Node(uint64_t left = 0, uint64_t right = 0,
             uint64_t parent = UNDEFINED);

        uint64_t pattern_len() const; //получить длину слова в ребре

        uint64_t &get_next_node(char symbol); //переход по символу через имеющиеся пути
    };

    std::vector<Node> tree; // само суффиксное дерево

    struct moving_ptr { //вспомогательная структура для удобства перемещения
        uint64_t current_point; // в какой сейчас вершине
        uint64_t position_in_str; //на сколько продвинулись по слову

        moving_ptr(uint64_t node_index, uint64_t position);
    };

    moving_ptr current_ptr; // для отслеживания перемещения

    moving_ptr tmp_ptr; // чтобы избежать лишнего копирования, создаем временный
    //отслеживатель перемещения, который будет отвечать за перемещение внутри вспомогательных
    // функций при построении (split, go, ...)

    std::string current_word; // соберем сюда все слова

    uint64_t position; // отслеживание позиции в строке (потом позиция в дереве с точки зрения дфс)

    std::vector<uint64_t> words_sizes; // храним длины слов уже вошедших

    void add_symbol(); // добавление одного символа

    uint64_t get_suf_link(int32_t from); // суф ссылка

    uint64_t split_current_part(); // Если нам необходимо разорвать ребро, и создать ответвление от середины

    moving_ptr go_while_can_go(uint64_t left_bound, uint64_t right_bound); //проверка куда можем идти

    void dfs_print(); //требуемый вывод

    void print_node(const Node &current);//вывод вершины в виде родитель - слово - левая - граница - правая - граница

public:
    suffix_tree();

    void add_str(const std::string &word); //добавляем строку в наш список слов

    void build(); // построение по алгоритму Укконена

    void clear(); //Если мы захотим этот класс использовать несколько раз

    void print_tree(); // вывод
};

suffix_tree::Node::Node(uint64_t left, uint64_t right,
                        uint64_t parent) : left(left), right(right),
                                           parent(parent), link(UNDEFINED),
                                           way_to(ALPHABET_SIZE, UNDEFINED),
                                           belongs_to(0), position(0) {}

uint64_t suffix_tree::Node::pattern_len() const {
    return right - left;
}

// Особые символы у нас переводятся в начало алфавита, согласно ASCII
uint64_t &suffix_tree::Node::get_next_node(char symbol) {
    if (symbol == FIRST_ENDING) {
        return way_to[FIRST_INDEX];
    }
    if (symbol == SECOND_ENDING) {
        return way_to[SECOND_INDEX];
    }
    return way_to[symbol - 'a' + MOVED_FROM_BEGIN];
}

// создаем и указываем на корень
suffix_tree::suffix_tree() : current_ptr(0, 0),
                             tmp_ptr(0, 0),
                             position(0), current_word("") {
    tree.emplace_back();
}

void suffix_tree::build() {
    for (uint64_t i = 0; i < current_word.size(); ++i) {
        position = i;
        add_symbol();
    }
}

void suffix_tree::clear() {
    tree.resize(0);
    tree.emplace_back();
}

void suffix_tree::print_tree() {
    std::cout << tree.size() << "\n";
    dfs_print();
}

// Запускаем дфс, а потом выводим ответ
void suffix_tree::dfs_print() {
    std::vector<uint64_t> answers_indexes;
    std::stack<uint64_t> stack_of_nodes;
    position = 0;
    stack_of_nodes.push(0);
    uint64_t index_of_current_point;
    while (!stack_of_nodes.empty()) {
        index_of_current_point = stack_of_nodes.top();
        stack_of_nodes.pop();
        if (tree[index_of_current_point].left >= words_sizes[0]) { // Если индексы больше длины начальной строки
            // То эти индексы отвечают за последующие строки, значит, надо их уменьшить на длину строки
            tree[index_of_current_point].belongs_to = 1;
            tree[index_of_current_point].left -= words_sizes[0];
            tree[index_of_current_point].right -= words_sizes[0];
        } else if (tree[index_of_current_point].right >= words_sizes[0]) {
            //Если одна из границ превышает длину, то это длина слова (по построению)
            // Тогда, вычитая длину следующих слов, получим ответ для текущего слова
            // (так как у нас левая граница указывает на текущее слово)
            tree[index_of_current_point].right -= words_sizes[1];
        }


        // отслеживаем позицию для вывода и сохраняем вершину
        tree[index_of_current_point].position = position;
        ++position;
        answers_indexes.push_back(index_of_current_point);

        for (uint64_t i = ALPHABET_SIZE; i > 0; --i) {
            if (tree[index_of_current_point].way_to[i - 1] < UNDEFINED) {
                stack_of_nodes.push(tree[index_of_current_point].way_to[i - 1]);
            }
        }
    }
    for (uint64_t i = 1; i < tree.size(); ++i) {
        print_node(tree[answers_indexes[i]]);
    }
}

void suffix_tree::print_node(const Node &current) {
    std::cout << tree[current.parent].position << " " << current.belongs_to << " "
              << current.left << " " << current.right << "\n";
}

suffix_tree::moving_ptr::moving_ptr(uint64_t node_index, uint64_t position) :
        current_point(node_index), position_in_str(position) {}

void suffix_tree::add_str(const std::string &word) {
    this->current_word += word;

    words_sizes.push_back(word.size());
}

/*
 * Пока мы не получим, что разбить эту часть нельзя
 * (либо мы в корне, либо слово разбивать не надо и вернется корень)
 * Мы идем, пока можем на символ из строки. Если мы нашли точку, то просто перейдем в нее
 * Иначе вызываем разбиение относительно текущей вершины и смотрим, возвращаемый сплитом результат
 * Добавляем новый узел, заполняем в нем информацию о родителе и в родителе о нем.
 * Выполняем переход по символу из текущей позиции
 */
void suffix_tree::add_symbol() {
    uint64_t middle_index = 1;
    uint64_t leaf_index;
    for (; middle_index != 0;) {
        tmp_ptr = current_ptr;
        tmp_ptr = go_while_can_go(position, position + 1);
        if (tmp_ptr.current_point != UNDEFINED) {
            current_ptr = tmp_ptr;
            return;
        }
        tmp_ptr = current_ptr;
        middle_index = split_current_part();
        leaf_index = tree.size();
        tree.emplace_back(position, current_word.size(), middle_index);
        tree[middle_index].get_next_node(current_word[position]) = leaf_index;
        current_ptr.current_point = get_suf_link(middle_index);
        current_ptr.position_in_str = tree[current_ptr.current_point].pattern_len();
    }
}

/*
 * Получить суф. ссылку
 * Если уже посчитана - вернем её
 * Если родитель не задан, то вернем 0, то есть суф ссылка в корень
 * Иначе получим суф ссылку родителя и переместимся по искомым символам.
 * Воспользуемся сплит, чтобы получить координаты, куда ведет суф.ссылка
 */
uint64_t suffix_tree::get_suf_link(int32_t from) {
    if (tree[from].link != UNDEFINED) {
        return tree[from].link;
    }
    if (tree[from].parent == UNDEFINED) {
        return 0;
    }
    uint64_t to = get_suf_link(tree[from].parent);
    tmp_ptr = moving_ptr(to, tree[to].pattern_len());

    tmp_ptr = go_while_can_go(tree[from].left + (tree[from].parent == 0), tree[from].right);
    return tree[from].link = split_current_part();
}

/*
 * Пока с границами все в порядке, перемещаемся по дереву с помощью путей.
 * Проверяем закончилось ли ребро, по которому мы шли. Если да, то начнем переход из следующего ребра
 * В случае, если мы пришли в никуда, вернем это, потому что поймем, что надо создать вершину
 * Если не мы можем просто идти по ребру на разность границ, то перемещаемся
 * Иначе добавляем к границе пройденный путь и продолжаем цикл.
 */
suffix_tree::moving_ptr suffix_tree::go_while_can_go(uint64_t left_bound, uint64_t right_bound) {
    while (left_bound < right_bound) {
        if (tmp_ptr.position_in_str == tree[tmp_ptr.current_point].pattern_len()) {
            tmp_ptr = moving_ptr(tree[tmp_ptr.current_point].get_next_node(current_word[left_bound]), 0);
            if (tmp_ptr.current_point == UNDEFINED) {
                return tmp_ptr;
            }
        } else if (tmp_ptr.position_in_str != tree[tmp_ptr.current_point].pattern_len()) {
            if (current_word[tree[tmp_ptr.current_point].left + tmp_ptr.position_in_str] != current_word[left_bound]) {
                tmp_ptr = moving_ptr(UNDEFINED, UNDEFINED);
                return tmp_ptr;
            }
            if (right_bound - left_bound < tree[tmp_ptr.current_point].pattern_len() - tmp_ptr.position_in_str) {
                tmp_ptr = moving_ptr(tmp_ptr.current_point, tmp_ptr.position_in_str + right_bound - left_bound);
                return tmp_ptr;
            }
            left_bound += tree[tmp_ptr.current_point].pattern_len() - tmp_ptr.position_in_str;
            tmp_ptr.position_in_str = tree[tmp_ptr.current_point].pattern_len();
        }
    }
    return tmp_ptr;
}

/*
 * Этот сплит имеет немного другое представление о жизни, чем вы думаете
 * Он немного модифицированный, потому что сплитит он, когда надо, а когда можно не сплитить, он
 * вернет вершину, от которой вести отсчет
 *
 * Что касается самого процесса сплита:
 *     Мы создаем новую вершину в другой функции. Здесь создается новая вершина, которая
 *     поддерживает копирует данные о родителе и одновляет в старой вершине данные
 *
 *     то есть было (корень)0 - abc мы встретили ad
 *     тогда эта функция создаст вершину 0 - a -bc
 *     создав новую а и изменив границы в старой вершине на bc
 *     сделав новую вершину ее родителем. Затем от созданной вершины будет отделяться новая вершина
 */
uint64_t suffix_tree::split_current_part() {
    if (tmp_ptr.position_in_str == tree[tmp_ptr.current_point].pattern_len()) {
        return tmp_ptr.current_point;
    }
    if (tmp_ptr.current_point == 0) {
        return tree[tmp_ptr.current_point].parent;
    }
    Node old_node = tree[tmp_ptr.current_point];
    tree.emplace_back(old_node.left, old_node.left + tmp_ptr.position_in_str, old_node.parent);
    tree[old_node.parent].get_next_node(current_word[old_node.left]) = tree.size() - 1;
    tree[tree.size() - 1].get_next_node(current_word[old_node.left + tmp_ptr.position_in_str]) = tmp_ptr.current_point;
    tree[tmp_ptr.current_point].parent = tree.size() - 1;
    tree[tmp_ptr.current_point].left += tmp_ptr.position_in_str;
    return tree.size() - 1;
}


int main() {
    std::ios::sync_with_stdio(false);
    std::string first;
    std::string second;
    std::cin >> first >> second;
    suffix_tree answer;
    answer.add_str(first);
    answer.add_str(second);
    answer.build();
    answer.print_tree();
    return 0;
}
