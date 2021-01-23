/*
Задано дерево с корнем, содержащее (1 ≤ n ≤ 100 000) вершин, пронумерованных от 0 до n-1.
Требуется ответить на m (1 ≤ m ≤ 10 000 000) запросов о наименьшем общем предке для пары вершин.
Запросы генерируются следующим образом. Заданы числа a1, a2 и числа x, y и z.
Числа a3, ..., a2m генерируются следующим образом: ai = (x ⋅ ai-2 + y ⋅ ai-1 + z) mod n.
Первый запрос имеет вид (a1, a2).
 Если ответ на i-1-й запрос равен v, то i-й запрос имеет вид ((a2i-1 + v) mod n, a2i).

Для решения задачи можно использовать метод двоичного подъёма.
Формат ввода

Первая строка содержит два числа: n и m.
Корень дерева имеет номер 0.
Вторая строка содержит n-1 целых чисел, i-е из этих чисел равно номеру родителя вершины i.
Третья строка содержит два целых числа в диапазоне от 0 до n-1: a1 и a2.
Четвертая строка содержит три целых числа: x, y и z, эти числа неотрицательны и не превосходят 109.
Формат вывода

Выведите в выходной файл сумму номеров вершин — ответов на все запросы.
 */

#include <iostream>
#include <vector>

#define ll long long
/**
 * Суть алгоритма:
 *  Предпосчитаем для каждой вершины 1, 2 ... 2^i предка(для двоичного подъема)
 *  Это выполняется в ДФС, кладется в массив родителей. Асимптотика О(N log N) в силу реализации двс
 *  Так же в ДФС запоминаем время входа и выхода, чтобы определять ответ на запрос за О(1)
 *  Находить ответы на запросы будем за log N по массиву parent
 *  class Finding_LCA_Class Выполняет все требуемы в задании задачи, обраюатывая и создавая следующие задачи
 *  в соответствии с требованиями
 */
class Finding_LCA_Class {
private:
    std::vector<std::vector<ll>> Graph, Parent;
    std::vector<ll> IN, OUT;
    ll TimePassed = 0, Log2FromSize, a1, a2, x, y, z, size;

    // DFS с запоминание времени входа и выхода из вершины
    void DFS(const ll& current, const ll& parent = 0) {
        IN[current] = ++TimePassed;
        Parent[current][0] = parent;

        // Присваиваем соответствующих предшественников (2^i)
        for (ll i = 0; i < Log2FromSize; ++i) {
            Parent[current][i + 1] = Parent[Parent[current][i]][i];
        }

        //Продолжаем дфс
        for (ll& child : Graph[current]) {
            if (child != parent) {
                DFS(child, current);
            }
        }

        OUT[current] = ++TimePassed;
    }

    // Создаем следующую задачу, используя формулу, упомянутую в условии, обновляем переменные
    std::pair<ll, ll> MakeNewTask(const ll& previous_result) {
        std::pair<ll, ll> NewTask = std::make_pair((a1 + previous_result) % size, a2);
        a1 = (x * a1 + y * a2 + z) % size;
        a2 = (x * a2 + y * a1 + z) % size;
        return NewTask;
    }

public:
    Finding_LCA_Class(std::vector<std::vector<ll>>& graph_, const ll& size_, const ll& Log,
                      const ll& a1_, const ll& a2_, const ll& x_,
                      const ll& y_, const ll& z_) {
        a1 = a1_;
        a2 = a2_;
        x = x_;
        y = y_;
        z = z_;
        size = size_;
        Graph = graph_;
        Log2FromSize = Log;
        IN.resize(size, 0);
        OUT.resize(size, 0);
        Parent.assign(size, std::vector<ll>(Log2FromSize + 1));
        DFS(0);
    }

    ll FindNextAnswer(const ll& previous_res) {
        ll First, Second;
        // Создаем следующий запрос
        std::pair<ll,ll> Task = MakeNewTask(previous_res);
        First = Task.first;
        Second = Task.second;
        // Проверяем является ли одна из вершин родителем другой, сравнивая время входа и выхода
        if ((IN[First] <= IN[Second]) && (OUT[First] >= OUT[Second])) return First;
        if ((IN[Second] <= IN[First]) && (OUT[Second] >= OUT[First])) return Second;

        //Находим ответ
        for (ll i = Log2FromSize; i >= 0; --i) {
            if (!((IN[Parent[First][i]] <= IN[Second]) && (OUT[Parent[First][i]] >= OUT[Second]))) {
                First = Parent[First][i];
            }
        }
        return Parent[First][0];
    }
};

// Функция для нахождения логарифма с округлением вверх
ll FindGreaterThenLog(const ll& number) {
    return number == 1 ? 1: FindGreaterThenLog(number / 2) + 1;
}

int main() {
    // Объявление
    ll n, m, Log, a1, a2, x, y, z, child, PrevRes = 0, result = 0;
    std::vector<std::vector<ll>> graph;

    // Счтывание
    std::cin >> n >> m;
    Log = FindGreaterThenLog(n);
    graph.resize(n);
    for (ll j = 1; j < n; ++j) {
        std::cin >> child;
        graph[child].push_back(j);
    }
    std::cin >> a1 >> a2 >> x >> y >> z;
    Finding_LCA_Class LCA(graph, n, Log, a1, a2, x, y, z);

    // Обработка
    for (ll i = 0; i < m; ++i) {
        PrevRes = LCA.FindNextAnswer(PrevRes);
        result += PrevRes;
    }

    // Вывод
    std::cout << result << std::endl;
    return 0;
}