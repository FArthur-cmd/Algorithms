/*
Реализуйте структуру данных “массив строк” на основе декартового дерева по неявному ключу со следующими методами:
// Добавление строки в позицию position.\\
// Все последующие строки сдвигаются на одну позицию вперед.\\
void InsertAt( int position, const std::string& value );\\
// Удаление строки из позиции position.\\
// Все последующие строки сдвигаются на одну позицию назад.\\
void DeleteAt( int position );\\
// Получение строки из позиции position.\\
std::string GetAt( int position );

Все методы должны работать за O(log n) в среднем, где n – текущее количество строк в массиве.
Формат ввода
Первая строка содержит количество команд k≤106.

Последующие k строк содержат описания команд:
Команда “+ 10 hello”
означает добавление строки hello в позицию 10.
Команда “- 14 16”
означает удаление строки от позиции 14 до позиции 16.
Команда “? 33”
означает запрос на вывод строки из массива в позиции 33.
Формат вывода
Выведите все строки, запрошенные командами “?”.
 */

#include <iostream>
#include <memory>
#include <utility>

class Node {
public:
    explicit Node(std::string setValue) :
            value(std::move(setValue)),
            priority(rand()) {}

    int childSize = 1;
    int priority = 0;

    std::string value;

    std::shared_ptr<Node> leftChild  = nullptr,
            rightChild = nullptr;
};

class ArrayOfStrings {
private:
    std::shared_ptr<Node> root = nullptr;

    std::pair<std::shared_ptr<Node>, std::shared_ptr<Node> > split
            (const std::shared_ptr<Node> &node, int position) {
        if (!node) {
            return std::make_pair(nullptr, nullptr);
        }

        int leftSize = getSize(node -> leftChild);
        if (leftSize >= position) {
            auto splitLeft = split(node -> leftChild, position);

            node -> leftChild = splitLeft.second;
            update(node);

            return std::make_pair(splitLeft.first, node);
        } else {
            auto splitRight = split(node -> rightChild, position - leftSize - 1);

            node -> rightChild = splitRight.first;
            update(node);

            return std::make_pair(node, splitRight.second);
        }
    }

    std::shared_ptr<Node> merge(const std::shared_ptr<Node> &left, const std::shared_ptr<Node> &right) {
        if (!left)  return right;
        if (!right) return left;

        if (left -> priority > right -> priority) {
            left -> rightChild = merge(left -> rightChild, right);
            update(left);

            return left;
        } else {
            right -> leftChild = merge(left, right -> leftChild);
            update(right);

            return right;
        }
    }

    int getSize(const std::shared_ptr<Node> &node)  {
        return node ? node -> childSize : 0;
    }

    void update(const std::shared_ptr<Node> &node) {
        if (node) {
            node -> childSize = 1 + getSize(node -> leftChild) + getSize(node -> rightChild);
        }
    }
public:
    void InsertAt(int position, const std::string& value) {
        auto newNode = std::make_shared<Node>(value);

        auto splitNode = split(root, position);

        auto rightChild = merge(newNode, splitNode.second);
        root = merge(splitNode.first, rightChild);
    }

    void DeleteAt(int position)  {
        auto splitNode = split(root, position);
        auto newNode = split(splitNode.second, 1);

        root = merge(splitNode.first, newNode.second);
    }

    std::string GetAt(int position) {
        auto currentNode = root;
        int leftIndex = getSize(root -> leftChild);

        while (leftIndex != position) {
            if (position < leftIndex) {
                currentNode = currentNode -> leftChild;
            } else {
                position -= leftIndex + 1;
                currentNode = currentNode -> rightChild;
            }
            leftIndex = getSize(currentNode -> leftChild);
        }

        return currentNode -> value;
    }
};

int main() {
    srand(time(nullptr));
    ArrayOfStrings array;

    int k = 0;
    std::cin >> k;

    for (int i = 0; i < k; ++i) {
        char buffer;
        std::cin >> buffer;

        int position = 0;
        std::cin >> position;

        if (buffer == '+') {
            std::string value;
            std::cin >> value;

            array.InsertAt(position, value);
        } else if (buffer == '-') {
            int endPosition = 0;
            std::cin >> endPosition;

            for (int deletePosition = position; deletePosition <= endPosition; ++deletePosition) {
                array.DeleteAt(deletePosition);
            }
        } else if (buffer == '?'){
            std::cout << array.GetAt(position) << std::endl;
        }
    }

    return 0;
}