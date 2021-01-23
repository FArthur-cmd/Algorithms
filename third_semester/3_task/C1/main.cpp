/**
 *  Арсений решил поиграть в настольную игру со своим другом Ильей.
 *  Так как играть обычными кубиками им уже стало неинтересно,
 *  Илья попросил по его чертежам сконструировать новую игральную кость.
 *  Так как он ленивый, то просто накидал точек в пространстве и сказал,
 *  что все они должны лежать в кубике его мечты. У Арсения есть 3D-принтер,
 *  вот только материалы для печати достаточно дорогие,
 *  поэтому он хочет выполнить требования своего друга, потратив как можно меньше пластика.
 *
 *
 *  Формат ввода
 *
 *  Первая строка содержит число M — количество тестов.
 *  В последующих строках описаны сами тесты. Каждый тест начинается со строки,
 *  содержащей N (N ≤ 1000) — число точек. Далее, в N строках даны по три числа — координаты точек.
 *  Все координаты целые, не превосходят по модулю 500.
 *
 *
 *  Формат вывода
 *
 *  Для каждого теста выведите следующее. В первую строку выведите количество граней m.
 *  Далее в последующие m строк выведите описание граней: количество точек грани (=3)
 *  и номера точек в исходном множестве. Точки нумеруются в том же порядке,
 *  в котором они даны во входном файле. Точки в пределах грани должны быть отсортированы
 *  в порядке против часовой стрелки относительно внешней нормали к грани.
 *  Первая точка – точка с минимальным номером. Порядок граней лексикографический.
 */
#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <tuple>

const uint32_t MAX_POINT_COUNT = 1001;
const double rotation_angle = 0.01;
const double EPSILON = 1e-12;
const double_t INF = std::numeric_limits<double_t>::infinity();


/*
 * Класс вектор - вектор из точки (0,0,0) в
 * точку (x,y,z) в пространстве
 */
template<typename T>
struct Vector {
public:
    Vector(const T &x = 0, const T &y = 0, const T &z = 0);

    template<typename U>
    Vector(const Vector<U> &other) :x(other.x), y(other.y), z(other.z) {};

    Vector &operator=(const Vector &other);

    T find_sqr_length();  // длину в квадрате, чтобы не терять точность в интах

    double find_length(); // длина

    T dot_product(const Vector &other) const; // скалярное произведение

    Vector vector_product(const Vector &other) const; // векторное произведение

    Vector &operator+=(const Vector &other);

    Vector &operator-=(const Vector &other);

    Vector operator-() const;

    Vector &operator*=(const T &other);

    Vector &operator/=(const T &other);

    T x;
    T y;
    T z;
};

template<typename T>
Vector<T> &Vector<T>::operator=(const Vector<T> &other) {
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}


template<typename T>
Vector<T> &Vector<T>::operator*=(const T &other) {
    x *= other;
    y *= other;
    z *= other;
    return *this;
}

template<typename T>
Vector<T>::Vector(const T &x, const T &y, const T &z) : x(x), y(y), z(z) {}

template<typename T>
Vector<T> &Vector<T>::operator/=(const T &other) {
    x /= other;
    y /= other;
    z /= other;
    return *this;
}

template<typename T>
T Vector<T>::dot_product(const Vector<T> &other) const {
    return x * other.x + y * other.y + z * other.z;
}

template<typename T>
Vector<T> Vector<T>::vector_product(const Vector<T> &other) const {
    return Vector<T>(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
    );
}

template<typename T>
Vector<T> &Vector<T>::operator+=(const Vector<T> &other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

template<typename T>
Vector<T> &Vector<T>::operator-=(const Vector<T> &other) {
    return *this += (-other);
}

template<typename T>
double Vector<T>::find_length() {
    return sqrt(static_cast<double>(find_sqr_length()));
}

template<typename T>
T Vector<T>::find_sqr_length() {
    return dot_product(*this);
}

template<typename T>
Vector<T> operator+(const Vector<T> &first, const Vector<T> &second) {
    return Vector<T>(first) += second;
}

template<typename T>
Vector<T> operator-(const Vector<T> &first, const Vector<T> &second) {
    return Vector<T>(first) -= second;
}

template<typename T>
Vector<T> operator*(const Vector<T> &first, const T &second) {
    return Vector<T>(first) *= second;
}

template<typename T>
Vector<T> operator/(const Vector<T> &first, const T &second) {
    return Vector<T>(first) /= second;
}

template<typename T>
bool operator==(const Vector<T> &first, const Vector<T> &second) {
    return std::tie(first.x, first.y, first.z) == std::tie(second.x, second.y, second.z);
}

template<typename T>
Vector<T> Vector<T>::operator-() const {
    return Vector<T>(-x, -y, -z);
}

template<typename T>
std::istream &operator>>(std::istream &in, Vector<T> &vec) {
    in >> vec.x >> vec.y >> vec.z;
    return in;
}

template<typename T>
std::ostream &operator<<(std::ostream &out, const Vector<T> &point) {
    out << point.x << " " << point.y;
    return out;
}

template<typename T>
bool operator<(const Vector<T> &first, const Vector<T> &second) {
    return std::tie(first.x, first.y, first.z) < std::tie(second.x, second.y, second.z);
}

struct modified_vector {
    Vector<double_t> point;
    modified_vector *next = nullptr;
    modified_vector *previous = nullptr;
    uint64_t number_of_point = MAX_POINT_COUNT;

    modified_vector() = default;

    modified_vector(Vector<double_t> point);

    modified_vector(Vector<double_t> point, uint64_t number);

    modified_vector(Vector<double_t> point, modified_vector *previous, modified_vector *next);

    modified_vector(Vector<double_t> point, uint64_t number, modified_vector *previous, modified_vector *next);

    bool operation(); //insert and delete operation

    void rotate_on_angle(double_t angle);
};

void modified_vector::rotate_on_angle(double_t angle) {
    double new_coord = point.x * cos(angle) + point.z * sin(angle);
    point.z = -point.x * sin(angle) + point.z * cos(angle);
    point.x = new_coord;

    new_coord = point.z * cos(angle) + point.y * sin(angle);
    point.y = -point.z * sin(angle) + point.y * cos(angle);
    point.z = new_coord;

    new_coord = point.x * cos(angle) + point.y * sin(angle);
    point.y = -point.x * sin(angle) + point.y * cos(angle);
    point.x = new_coord;
}

bool modified_vector::operation() {
    if (previous->next != this) {
        previous->next = next->previous = this;
        return true;
    } else {
        previous->next = next;
        next->previous = previous;
        return false;
    }
}

modified_vector::modified_vector(Vector<double_t> point) : point(point), previous(nullptr), next(nullptr) {}

modified_vector::modified_vector(Vector<double_t> point, uint64_t number) : point(point), number_of_point(number),
                                                                            previous(nullptr), next(nullptr) {}

modified_vector::modified_vector(Vector<double_t> point, modified_vector *previous, modified_vector *next) :
        point(point), previous(previous), next(next) {}

modified_vector::modified_vector(Vector<double_t> point, uint64_t number, modified_vector *previous,
                                 modified_vector *next) : point(point), number_of_point(number),
                                                          previous(previous), next(next) {}

bool operator<(const modified_vector &first, const modified_vector &second) {
    return first.point < second.point;
}

bool operator==(const modified_vector &first, const modified_vector &second) {
    return first.point == second.point;
}

struct Face {
    Face();

    std::vector<int32_t> points;

    void build_from_points(const modified_vector *point);
};

Face::Face() {
    points.resize(3);
}

void Face::build_from_points(const modified_vector *point) {
    points[0] = point->previous->number_of_point;
    points[1] = point->number_of_point;
    points[2] = point->next->number_of_point;
}

bool operator<(const Face &first, const Face &second) {
    return std::tie(first.points[0], first.points[1], first.points[2]) <
           std::tie(second.points[0], second.points[1], second.points[2]);
}

/*
 * Решим алгоритмом Чана (вдохновлен статьей с диска с лекциями)
 */
class Chan_algorithm {
public:
    Chan_algorithm(std::istream &in);

    void print_answer(); // Вывод ответа согласно условиям

    void build_hull(); // Построение оболочки

    const std::vector<modified_vector> &get_points() const; // доступ к вершинам (для чтения)

private:

    //Нахождение времени по значениям точек (подробное объяснение в алгоритме чана)
    double time(const modified_vector *first, const modified_vector *second, const modified_vector *third);

    double turn(const modified_vector *first, const modified_vector *second, const modified_vector *third);

    std::vector<modified_vector *> build_recursively(const uint64_t &begin, const uint64_t &end);

    void build_lower_hull(bool reversed = false); // построение одной половины оболочки (в зависимости от флага
    // верхней или нижней)

    void sort_hull();  // Сортировка граней для вывода (лексикографический порядок)

    void inverse_z();  // Инвертируем ось Оz, чтобы найти другую половину решения

    void rotate_all_points(); // для избежания проблем с коллизиями, повернем все точки на заранее выбранный угол

    void calculate_times(const std::vector<modified_vector *> &left,  //Заполним массив времен. Он показывает
                         const std::vector<modified_vector *> &right); // когда могут произойти события
    // Это позволяет идти по времени не от -бесконечности до бесконечности
    // А только по 6 потенциальным моментам, когда могут произойти изменения

    void get_closest_time();  //Получаем наиближайшее событие в будущем

    void work_with_cases(std::vector<modified_vector *> &probable_hull);  // Обрабатываем случай в зависимости
    // От времени, в котором он произошел

    void work_with_points(const std::vector<modified_vector *> &probable_hull,  // Добавление вершин
                          uint64_t middle);

    bool can_find_new_time(std::vector<modified_vector *> &left_part,
                           std::vector<modified_vector *> &right_part);

    void find_initial_bridge();

    modified_vector null_point{Vector<double_t>(INF, INF, INF)};
    std::vector<modified_vector> points_; //все точки
    std::vector<Face> Hull;   // оболочка - набор плоскостей

    double new_time, current_time;

    size_t first_index, second_index, time_index; // Для итерации по массивам

    double times[6]{};

    modified_vector *middle_point_from_first;  // Потенциальный мост
    modified_vector *middle_point_from_second;

    modified_vector *left_point; // Для итерации по масивам
    modified_vector *right_point;


};

Chan_algorithm::Chan_algorithm(std::istream &in) {
    uint64_t count_of_points;
    in >> count_of_points;
    points_.resize(count_of_points);
    for (uint64_t j = 0; j < count_of_points; ++j) {
        in >> points_[j].point;
        points_[j].number_of_point = j;
    }
}

const std::vector<modified_vector> &Chan_algorithm::get_points() const {
    return points_;
}

void Chan_algorithm::rotate_all_points() {
    for (auto &mod_point: points_) {
        mod_point.rotate_on_angle(rotation_angle);
    }
}

void Chan_algorithm::print_answer() {
    sort_hull();
    std::cout << Hull.size() << std::endl;
    for (Face &face : Hull) {
        std::cout << "3 " << face.points[0] << " " << face.points[1] << " "
                  << face.points[2] << "\n";
    }
}

// Сортировка в лексикографическом порядке
// Замена точек местами не эквивалента std::sort,
// так как необходимо сохранить порядок обхода, а не просто отсортировать точки.
void Chan_algorithm::sort_hull() {
    for (Face &face: Hull) {
        if (face.points[1] < face.points[0] && face.points[1] < face.points[2]) {
            std::swap(face.points[0], face.points[1]);
            std::swap(face.points[1], face.points[2]);
        } else if (face.points[2] < face.points[0] && face.points[2] < face.points[1]) {
            std::swap(face.points[1], face.points[2]);
            std::swap(face.points[0], face.points[1]);
        }
    }
    std::sort(Hull.begin(), Hull.end());
}

void Chan_algorithm::build_hull() {
    rotate_all_points();
    std::sort(points_.begin(), points_.end());
    build_lower_hull(); //Построим нижнюю оболочку
    inverse_z();
    build_lower_hull(true); //построим верхнюю оболочку
}

void Chan_algorithm::inverse_z() {
    for (modified_vector &element:points_) {
        element.point.z = -element.point.z;
        element.next = nullptr;
        element.previous = nullptr;
    }
}

/*
 * Строим нижнюю оболочку рекурсивно разбивая множество точек на части
 * Для каждой полученной тройки строим грань и определяем направление
 * Так как во второй раз полученные результаты будут для перевернутой оболочки то изменится
 * условие того, что поворот верный (если поворот верный, то предыдущий указывает на текущего и
 * вернется ложь. Когда у нас оболочка перевернутая, то мы по факту будет получать обратные направления
 * нормали)
 */
void Chan_algorithm::build_lower_hull(bool reversed) {
    auto lower_hull_points = build_recursively(0, points_.size());
    Face current_face{};
    for (modified_vector *point: lower_hull_points) {
        current_face.build_from_points(point);
        if (reversed == point->operation()) {
            std::swap(current_face.points[0], current_face.points[1]);
        }
        Hull.push_back(current_face);
    }
}

std::vector<modified_vector *> Chan_algorithm::build_recursively(const uint64_t &begin, const uint64_t &end) {
    if (end - begin <= 1) {
        return {};
    }
    uint64_t middle = (begin + end) / 2;
    std::vector<modified_vector *> result;
    auto left_part = build_recursively(begin, middle);
    auto right_part = build_recursively(middle, end);
    middle_point_from_first = &points_[middle - 1];  // Потенциальный мост
    middle_point_from_second = &points_[middle];

    //Рассмотрим правда ли мы нашли потенциальный мост
    find_initial_bridge();

    current_time = -INF;
    first_index = second_index = 0;
    while (can_find_new_time(left_part, right_part)) {
        work_with_cases(result);
        current_time = new_time;
    }
    // объединяем множетсва
    middle_point_from_first->next = middle_point_from_second;
    middle_point_from_second->previous = middle_point_from_first;

    // Добавить точки в множества
    work_with_points(result, middle);

    return result;
}

void Chan_algorithm::find_initial_bridge() {
    while (true) {
        if (turn(middle_point_from_first->previous, middle_point_from_first, middle_point_from_second) < -EPSILON) {
            middle_point_from_first = middle_point_from_first->previous;
        } else if (turn(middle_point_from_first, middle_point_from_second, middle_point_from_second->next) < -EPSILON) {
            middle_point_from_second = middle_point_from_second->next;
        } else {
            break;
        }
    }
}

bool Chan_algorithm::can_find_new_time(std::vector<modified_vector *> &left_part,
                                       std::vector<modified_vector *> &right_part) {
    left_point = &null_point;
    right_point = &null_point;
    calculate_times(left_part, right_part);
    new_time = INF;
    time_index = 6;
    get_closest_time();
    if (time_index == 6) {
        return false;
    }
    return true;
}

double Chan_algorithm::turn(const modified_vector *first, const modified_vector *second,
                            const modified_vector *third) {
    if (first == nullptr || second == nullptr || third == nullptr ||
        *first == null_point || *second == null_point || *third == null_point) {
        return 1.0;
    }
    return (second->point.x - first->point.x) * (third->point.y - first->point.y) -
           (third->point.x - first->point.x) * (second->point.y - first->point.y);
}

double Chan_algorithm::time(const modified_vector *first, const modified_vector *second,
                            const modified_vector *third) {
    if (first == nullptr || second == nullptr || third == nullptr ||
        *first == null_point || *second == null_point || *third == null_point) {
        return INF;
    }
    return ((second->point.x - first->point.x) * (third->point.z - first->point.z) -
            (third->point.x - first->point.x) * (second->point.z - first->point.z)) / turn(first, second, third);
}

void Chan_algorithm::calculate_times(const std::vector<modified_vector *> &left,
                                     const std::vector<modified_vector *> &right) {
    // Если происходит вставка элемента из первой части
    if (first_index < left.size()) {
        left_point = left[first_index];
        times[0] = time(left_point->previous, left_point, left_point->next);
    } else {
        times[0] = INF;
    }

    // Если происходит вставка элемента из первой части
    if (second_index < right.size()) {
        right_point = right[second_index];
        times[1] = time(right_point->previous, right_point, right_point->next);
    } else {
        times[1] = INF;
    }

    // Когда новым мостом становится предыдущая вершина из левой половины, и та, которая была из правой
    times[2] = time(middle_point_from_first->previous, middle_point_from_first, middle_point_from_second);
    // Когда новым мостом становится следующая вершина из левой половины, и та, которая была из правой
    times[3] = time(middle_point_from_first, middle_point_from_first->next, middle_point_from_second);
    // Когда новым мостом становится следующая вершина из правой половины, и та, которая была из левой
    times[4] = time(middle_point_from_first, middle_point_from_second, middle_point_from_second->next);
    // Когда новым мостом становится предыдущая вершина из правой половины, и та, которая была из левой
    times[5] = time(middle_point_from_first, middle_point_from_second->previous, middle_point_from_second);

}

void Chan_algorithm::get_closest_time() {
    for (int i = 0; i < 6; i++) {
        if (times[i] - current_time > 0 && new_time - times[i] > 0) {
            new_time = times[i];
            time_index = i;
        }
    }
}

void Chan_algorithm::work_with_cases(std::vector<modified_vector *> &probable_hull) {
    switch (time_index) {
        case 0: {
            if (middle_point_from_first->point.x > left_point->point.x) { // Добавление элемента из первой части
                probable_hull.push_back(left_point);
            }
            left_point->operation();
            ++first_index;
            break;
        }
        case 1: {
            if (right_point->point.x > middle_point_from_second->point.x) {// Добавление элемента из второй части
                probable_hull.push_back(right_point);
            }
            right_point->operation();
            ++second_index;
            break;
        }
        case 2: {// Добавление элемента из моста на этом времени
            probable_hull.push_back(middle_point_from_first);
            middle_point_from_first = middle_point_from_first->previous;
            break;
        }
        case 3: {// Добавление элемента из моста на этом времени
            middle_point_from_first = middle_point_from_first->next;
            probable_hull.push_back(middle_point_from_first);
            break;
        }
        case 4: { // Добавление элемента из моста на этом времени
            probable_hull.push_back(middle_point_from_second);
            middle_point_from_second = middle_point_from_second->next;
            break;
        }
        case 5: {// Добавление элемента из моста на этом времени
            middle_point_from_second = middle_point_from_second->previous;
            probable_hull.push_back(middle_point_from_second);
            break;
        }
    }
}

void Chan_algorithm::work_with_points(const std::vector<modified_vector *> &probable_hull,
                                      uint64_t middle) {
    for (size_t i = probable_hull.size(); i > 0; --i) {
        modified_vector *point = probable_hull[i - 1];
        if (middle_point_from_first->point.x < point->point.x &&
            point->point.x < middle_point_from_second->point.x) {
            middle_point_from_first->next = middle_point_from_second->previous = point;
            point->previous = middle_point_from_first;
            point->next = middle_point_from_second;
            if (point->point.x <= points_[middle - 1].point.x) {
                middle_point_from_first = point;
            } else {
                middle_point_from_second = point;
            }
        } else {
            point->operation();
            if (point == middle_point_from_first) {
                middle_point_from_first = middle_point_from_first->previous;
            }
            if (point == middle_point_from_second) {
                middle_point_from_second = middle_point_from_second->next;
            }
        }
    }
}

int main() {
    uint64_t number_of_tests;
    std::cin >> number_of_tests;
    for (uint64_t i = 0; i < number_of_tests; ++i) {
        Chan_algorithm solution(std::cin);
        solution.build_hull();
        solution.print_answer();
    }
    return 0;
}
