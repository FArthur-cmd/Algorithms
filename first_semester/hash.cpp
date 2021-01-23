/*
Реализуйте структуру данных типа “множество строк”
на основе динамической хеш-таблицы с открытой адресацией.
Хранимые строки непустые и состоят из строчных латинских букв.
Хеш-функция строки должна быть реализована с помощью вычисления значения многочлена методом Горнера.
Начальный размер таблицы должен быть равным 8-ми.
Перехеширование выполняйте при добавлении элементов в случае,
когда коэффициент заполнения таблицы достигает 3/4.
Структура данных должна поддерживать операции добавления строки в множество,
удаления строки из множества и проверки принадлежности данной строки множеству.
1_2. Для разрешения коллизий используйте двойное хеширование.

Формат ввода
Каждая строка входных данных задает одну операцию над множеством.
Запись операции состоит из типа операции и следующей за ним через пробел строки,
над которой проводится операция. Тип операции – один из трех символов:
+ означает добавление данной строки в множество;
- означает удаление строки из множества;
? означает проверку принадлежности данной строки множеству.
При добавлении элемента в множество НЕ ГАРАНТИРУЕТСЯ, что он отсутствует в этом множестве.
При удалении элемента из множества НЕ ГАРАНТИРУЕТСЯ, что он присутствует в этом множестве.

Формат вывода
Программа должна вывести для каждой операции одну из двух строк OK или FAIL.

Для операции '?': OK, если элемент присутствует во множестве. FAIL иначе.

Для операции '+': FAIL, если добавляемый элемент уже присутствует во множестве и потому не может быть добавлен.
OK иначе.

Для операции '-': OK, если элемент присутствовал во множестве и успешно удален. FAIL иначе.
*/
#include <iostream>
#include<vector>
#include<string>
#include<cassert>
//первая хеш-функция
unsigned long long int hash_1(const std::string &word,const unsigned long long int &module) {
    unsigned long long int value = 0;
    for (unsigned long long int i = 0; i < word.size(); i++) {
        value = value*31 + word[i];
    }
    return value % module;
}
//вторая хеш-функция
unsigned long long int hash_2(const std::string &word,const unsigned long long int &module) {
    unsigned long long int value = 0;
    for (unsigned long long int i = 0; i < word.size(); i++) {
        value = value*37 + word[i];
    }
    return (2 * value + 1) % module;
}

/*
   rehesh увеличивает размер таблицы и переносит все неудаленные данные
   путем повторного хеширования
   add_element добавляет элемент. Если не найден такой элемент, то добавляем его на пустое место
   или на первое удаленное
   delete_element удаляет элемент. Если элемент не найден, то удалить нельзя
   иначе находим элемент и удаляем
   find_element ищет элемент
   print печатает таблицу(нужно для отладки)
   избагаются коллизии путем двойного хеширования:
   h = (hash_1 + i* hash_2)
   Сама таблица состоит из элементов типа item, в которых
   в ключе содержится значение строки, а в tag содержится флаг,
   поясняющий состояние ячейки (пуста, удалена или занята)
   (класс и структура состоят из прописанных в библиотеках
    типов, которые имеют деструкторы, то есть деструктор для
    всей таблицы выполняется автоматически)
*/
class HashTable {
public:
    HashTable(unsigned long long int init_size = 8) {
        table.resize(init_size);
    }
    void rehesh() {
        deleted_number = 0;
        std::vector<item> second_table;
        second_table.resize(table.size() * 2);
        used = 0;
        for (unsigned long long int i = 0; i < table.size(); i++) {
            if (table[i].tag == item::BUSY) {
                unsigned long long int h1 = hash_1(table[i].key, second_table.size());
                unsigned long long int h2 = hash_2(table[i].key, second_table.size());
                unsigned long long int h = h1;
                while (second_table[h].tag != item::EMPTY)
                    h=(h + h2)%second_table.size();
                second_table[h].key = table[i].key;
                second_table[h].tag = item::BUSY;
                used++;
            }
        }
        table = second_table;
    }
    bool add_element(const std::string &element) {
        if (3*table.size() <= 4*(used+deleted_number+1))
            rehesh();
        if (find_element(element))
            return false;
        bool found = false;
        unsigned long long int place = 0;
        unsigned long long int h1 = hash_1(element, table.size());
        unsigned long long int h2 = hash_2(element, table.size());
        unsigned h = h1;
        while (table[h].tag != item::EMPTY) {
            if (table[h].tag == item::DELETED && !found) {
                place = h;
                found = true;
            }
            h = (h + h2)% table.size();
        }
        if (!found) {
            place = h;
        } else {
            deleted_number--;
        }
        table[place].key = element;
        table[place].tag = item::BUSY;
        used++;
        return true;
    }
    bool delete_element(const std::string &element) {
        unsigned long long int h2 = hash_2(element, table.size());
        unsigned long long int h1 = hash_1(element, table.size());
        if (!find_element(element))
            return false;
        for (auto h = h1; ;h =(h+ h2)%table.size()) {
            if (table[h].tag == item::DELETED)
                continue;
            if (table[h].key == element) {
                table[h].tag = item::DELETED;
                used--;
                deleted_number++;
                return true;
            }
        }
        return false;
    }
    bool find_element(const std::string &element) {
        unsigned long long int h2 = hash_2(element, table.size());
        unsigned long long int h1 = hash_1(element, table.size());
        for (auto h = h1; ;h =(h+ h2)%table.size()) {
            if (table[h].tag == item::EMPTY)
                return false;
            if (table[h].tag == item::DELETED)
                continue;
            if (table[h].key == element)
                return true;
        }
        return false;
    }
    void print() {
        for (unsigned long long int i = 0; i < table.size(); i++) {
            if (table[i].tag == item::BUSY)
                std::cout << i << "-" << table[i].key << std::endl;
            if (table[i].tag == item::EMPTY)
                std::cout << i << "-*EMPTY*" << std::endl;
            if (table[i].tag == item::DELETED)
                std::cout << i << "-*DELETED*" << std::endl;
        }
        std::cout<<"del "<<deleted_number<<" used "<< used<<"\n";
    }
private:
    struct item {
        std::string key;
        item (std::string const &key = "", int tag = item ::EMPTY) :key(key), tag(tag){};
        enum tag_e{
            EMPTY,
            BUSY,
            DELETED
        };
        int tag = EMPTY;
    };
    std::vector<item> table;
    unsigned long long int used = 0, deleted_number = 0;
};

int main()
{
    HashTable table;
    std::string element;
    char operation;
    while (std::cin>> operation>>element) {
        switch (operation) {
        case '+': {
            if (table.add_element(element)) {
                std::cout <<"OK\n";
            } else {
                std::cout <<"FAIL\n";
            }
            break;
        }
        case '-': {
            if (table.delete_element(element)) {
                std::cout <<"OK\n";
            } else {
                std::cout <<"FAIL\n";
            }
            break;
        }
        case '?': {
            if (table.find_element(element)) {
                std::cout <<"OK\n";
            } else {
                std::cout <<"FAIL\n";
            }
            break;
        }
        case 'p': {
            table.print();
            std::cout << std::endl;
            break;
        }
    }
    }
    return 0;
}
