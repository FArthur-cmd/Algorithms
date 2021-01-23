/*
 Джон Макклейн сообщает по рации новую информацию
 о террористах в отдел с n полицейскими. Он звонит
 нескольким сотрудникам и просит распространить
 информацию по отделу, зная, что у каждого полицейского
 есть связь с определёнными коллегами. Джон Макклейн хочет,
 чтобы операция прошла успешно. Но если полицейский позвонит
 коллеге, от которого(возможно, не напрямую) сам получил
 информацию, террористы смогут отследить этот звонок и
 помешать операции. Если же двое сотрудников оповестят
 одного, ничего плохого не произойдёт. Помогите Джону Макклейну.
 Выведите NO, если кто-то из полицейских ошибётся, делая звонок.
 Если всё пройдёт хорошо, выведите YES и порядок,
 в котором полицейские получат информацию, считая,
 что полицейские оповещают коллег по возрастанию их номеров,
 а в начале Джон даёт информацию тем,
 кому не может позвонить никто из коллег.

 Формат ввода
 Первая строка содержит пару целых чисел
 n и m (1 ≤ n, m ≤ 105) — количество вершин
 и рёбер заданного ориентированного графа.

 Следующие m строк содержат описания рёбер,
 по одному в строке. Каждая из строк содержит
 по два целых числа a и b
 (0 ≤ a, b < n) — начало и конец ребра.

 Формат вывода
 Если такого порядка не существует,
 то выведите «NO» (без кавычек).

 В противном случае, выведите «YES» в первую из строк.
 Во вторую строку выведите перестановку индексов вершин
 (целые числа от 0 до n - 1), такую, что для каждого
 ребра его стартовая вершина находится строго левее его
 конечной вершины.
*/

#include <iostream>
#include <vector>
#include <algorithm>

template <typename T>
class Stack{
private:
    std::vector<T> buffer;
public:
    void push_back(T element) {
        buffer.push_back(element);
    }
    void pop() {
        buffer.pop_back();
    }
    T top() {
        return buffer.back();
    }
    unsigned int Size() {
       return buffer.size();
    }
    bool empty() {
        return (buffer.size() == 0);
    }
};
/*
 *Класс Граф
 * содержит пути
 * есть функции добавления пути,
 * поиска решения задачи, ДФС для
 * топологической сортировки
 * В обычном дфс, когда вершина
 * становится черной, добавляем ее в вектор
 * если наткнулись на серую - нашли цикл
 * решения, так как у нее наименьшее время выхода
 * Выводить будем вектор с конца
 * Так как требуется получить обратную топ.сорту связь
 * (Мы должны вывести вершины обратно пропорционально
 * времени выхода, чтобы начало ребра было раньше конца)
 */
class Graph {
private:
    std::vector<std::vector<int>> ways;

public:
    Graph(int size = 0) {
        ways.resize(size);
    }
    void AddConnection(int from, int to) {
        ways[from].push_back(to);
    }
    void Solution(bool &correct, Stack<int> &answer) {
        for (int i = 0; i < ways.size(); ++i) {
            std::sort(ways[i].begin(), ways[i].end());
        }
        std::vector<int> colours;
        colours.resize(ways.size());
        for (int i = 0; i < ways.size(); ++i) {
            DFS(i, correct, colours, answer);
        }
    }
    void DFS(const int &CurPoint, bool &Correct,
             std::vector<int> &colours,
             Stack<int> &top_sorted) {
        if ((colours[CurPoint] == 2) || (colours[CurPoint] == 1)) {
            return;
        }
        Stack<int> work_with;
        work_with.push_back(CurPoint);
        while (!work_with.empty()) {
            int tmp = work_with.top();
            if (colours[tmp] == 1) {
                top_sorted.push_back(tmp);
                work_with.pop();
                colours[tmp] = 2;
            } else if (colours[tmp] == 0){
                colours[tmp] = 1;
                for (int i = ways[tmp].size() - 1; i >= 0; --i) {
                    if (colours[ways[tmp][i]] == 1) {
                        Correct = false;
                        return;
                    }
                    if (colours[ways[tmp][i]] == 0)
                        work_with.push_back(ways[tmp][i]);
                }
            } else {
                work_with.pop();
            }
        }
    }
};

int main() {
    int verticies, edges, from, to;
    std::cin >> verticies >> edges;
    Graph graph(verticies);
    for (int i = 0; i < edges; ++i) {
        std::cin >> from >> to;
        graph.AddConnection(from, to);
    }
    bool Correct = true;
    Stack<int> answer;
    graph.Solution(Correct, answer);
    if (Correct) {
        std::cout << "YES\n";
        for (int i = 0; i < verticies; ++i) {
            std::cout << answer.top();
            answer.pop();
        }
    } else {
        std::cout << "NO";
    }
}