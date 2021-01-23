/**
 * Даны точки, никакие 3 из которых не лежат на одной прямой.
 * Никакие 4 точки не лежат на одной окружности. Кроме того,
 * все точки имеют различные x-координаты.
 * Определите среднее число сторон в многоугольниках диаграммы Вороного этого множества точек.
 * Считаются только конечные многоугольники. Если все многоугольники неограниченны,
 * ответ полагается равным 0. Число точек n ≤ 100000.
 * Алгоритм должен иметь асимптотику O(n log n).
 *
 *
 * Формат ввода
 *
 * В каждой строке через пробел записаны действительные координаты точек xi yi.
 *
 *
 * Формат вывода
 *
 * Число - среднее число сторон в ограниченных многоугольниках диаграммы
 * Вороного с точностью 10-6. Если таких многоугольников нет - ответ 0.
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <set>
#include <tuple>

const uint32_t MAX_POINT_COUNT = 100001;
const long double EPSILON = 1e-9;
const long double INF = std::numeric_limits<long double>::infinity();


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

    long double find_length(); // длина

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
long double Vector<T>::find_length() {
    return sqrt(static_cast<long double>(find_sqr_length()));
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
std::istream &operator>>(std::istream &in, Vector<T>& vec) {
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
    return std::tie(first.x, first.y) < std::tie(second.x, second.y);
}

struct modified_vector {
    Vector<long double> point;
    modified_vector *next = nullptr;
    modified_vector *previous = nullptr;
    uint64_t number_of_point = MAX_POINT_COUNT;

    modified_vector() = default;

    modified_vector(Vector<long double> point);

    modified_vector(Vector<long double> point, uint64_t number);

    modified_vector(Vector<long double> point, modified_vector *previous, modified_vector *next);

    modified_vector(Vector<long double> point, uint64_t number, modified_vector *previous, modified_vector *next);

    bool operation(); //insert and delete operation

    void rotate_on_angle(long double angle);
};

void modified_vector::rotate_on_angle(long double angle) {
    long double new_coord = point.x * cos(angle) + point.z * sin(angle);
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

modified_vector::modified_vector(Vector<long double> point) : point(point), previous(nullptr), next(nullptr) {}

modified_vector::modified_vector(Vector<long double> point, uint64_t number) : point(point), number_of_point(number),
                                                                               previous(nullptr), next(nullptr) {}

modified_vector::modified_vector(Vector<long double> point, modified_vector *previous, modified_vector *next) :
        point(point), previous(previous), next(next) {}

modified_vector::modified_vector(Vector<long double> point, uint64_t number, modified_vector *previous,
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
    void print_answer(); // Вывод ответа согласно условиям

    void build_hull(); // Построение оболочки

    const std::vector <modified_vector> &get_points() const; // доступ к вершинам (для чтения)

    void set_points(std::vector <modified_vector> &points); //доступ к вершинам (для записи)

    const std::vector <Face> &get_hull();

private:

    //Нахождение времени по значениям точек (подробное объяснение в алгоритме чана)
    long double time(const modified_vector *first, const modified_vector *second, const modified_vector *third);

    long double turn(const modified_vector *first, const modified_vector *second, const modified_vector *third);

    std::vector<modified_vector *> build_recursively(const uint64_t &begin, const uint64_t &end);

    void build_lower_hull(bool reversed = false); // построение одной половины оболочки (в зависимости от флага
    // верхней или нижней)

    void sort_hull();  // Сортировка граней для вывода (лексикографический порядок)

    void inverse_z();  // Инвертируем ось Оz, чтобы найти другую половину решения

    void calculate_times(const std::vector<modified_vector *> &left,  //Заполним массив времен. Он показывает
                         const std::vector<modified_vector *> &right); // когда могут произойти события
    // Это позволяет идти по времени не от -бесконечности до бесконечности
    // А только по 6 потенциальным моментам, когда могут произойти изменения

    void get_closest_time();  //Получаем наиближайшее событие в будущем

    void work_with_cases(std::vector<modified_vector *> &probable_hull);  // Обрабатываем случай в зависимости
    // От времени, в котором он произошел

    void work_with_points(std::vector<modified_vector *> &probable_hull,  // Добавление вершин
                          uint64_t middle);

    bool can_find_new_time(std::vector<modified_vector *> &left_part,
                           std::vector<modified_vector *> &right_part);

    void find_initial_bridge();

    modified_vector null_point{Vector<long double>(INF, INF, INF)};
    std::vector <modified_vector> points_; //все точки
    std::vector <Face> Hull;   // оболочка - набор плоскостей

    long double new_time, current_time;

    size_t first_index, second_index, time_index; // Для итерации по массивам

    long double times[6]{};

    modified_vector *middle_point_from_first;  // Потенциальный мост
    modified_vector *middle_point_from_second;

    modified_vector *left_point; // Для итерации по масивам
    modified_vector *right_point;
};

const std::vector <Face> &Chan_algorithm::get_hull() {
    return Hull;
}

const std::vector <modified_vector> &Chan_algorithm::get_points() const {
    return points_;
}

void Chan_algorithm::set_points(std::vector <modified_vector> &points) {
    points_.resize(points.size());
    std::move(points.begin(), points.end(), points_.begin());
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
    std::sort(points_.begin(), points_.end());
    build_lower_hull(); //Построим нижнюю оболочку
    //inverse_z();
    //build_lower_hull(true); //построим верхнюю оболочку
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
    std::vector < modified_vector * > result;
    std::vector < modified_vector * > left_part = build_recursively(begin, middle);
    std::vector < modified_vector * > right_part = build_recursively(middle, end);
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
        if (turn(middle_point_from_first->previous, middle_point_from_first, middle_point_from_second) < 0) {
            middle_point_from_first = middle_point_from_first->previous;
        } else if (turn(middle_point_from_first, middle_point_from_second, middle_point_from_second->next) < 0) {
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

long double Chan_algorithm::turn(const modified_vector *first, const modified_vector *second,
                                 const modified_vector *third) {
    if (first == nullptr || second == nullptr || third == nullptr ||
        *first == null_point || *second == null_point || *third == null_point) {
        return 1.0;
    }
    return (second->point.x - first->point.x) * (third->point.y - first->point.y) -
           (third->point.x - first->point.x) * (second->point.y - first->point.y);
}

long double Chan_algorithm::time(const modified_vector *first, const modified_vector *second,
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

void Chan_algorithm::work_with_points(std::vector<modified_vector *> &probable_hull,
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

struct Point {
    Vector<long double> point;
    uint64_t id;

    Point(const Vector<long double> &point, uint64_t id);

    Point() = default;
};

Point::Point(const Vector<long double> &point, uint64_t id) : point(point), id(id) {}

bool compare_points(const Point &first, const Point &second) {
    return first.point < second.point;
}

/*
 * Алгоритм Эндрю. Разделяем множество по прямой, соединяющей левую нижнюю и правую верхнюю вершины
 *
 * Тогда для построения верхней оболочки поворот всегда по часовой.
 * Для построения нижней по часовой
 *
 * Строим по алгоритму Гретхема, проверяя новую потенциальную вершину.
 * При этом рассматриваем угол с предыдущими двумя вершинами, чтобы не нарушить условие выпуклости
 *
 * Первая и последняя точки входят в оба множества.
 * Добавление последней точки аргументируется необходимостью корректности
 * выпуклости на последнем шаге, то есть пройдет проверка в сравнении с последней точкой
 * является ли предпоследняя точка необходимой или она приведет к невыпуклой оболочке
 */
class Andrew_convex_hull {
public:

    const std::vector<Point> &get_points() const;

    std::vector<Point> &get_points_to_modify();

    std::vector<Point> create_convex_hull();

private:
    void add_in_part_of_hull(const Point &point, bool up);

    std::vector<Point> points_;
    std::vector<Point> up_hull;
    std::vector<Point> down_hull;
};

const std::vector<Point> &Andrew_convex_hull::get_points() const {
    return points_;
}

std::vector<Point> &Andrew_convex_hull::get_points_to_modify() {
    return points_;
}

std::vector<Point> Andrew_convex_hull::create_convex_hull() {
    std::sort(points_.begin(), points_.end(), compare_points);
    up_hull.push_back(points_[0]);
    down_hull.push_back(points_[0]);
    Vector<long double>  direction_vector = points_.back().point - points_[0].point;
    for (size_t i = 1; i < points_.size(); ++i) {
        if (i == points_.size() - 1 || direction_vector.vector_product(points_[i].point - points_[0].point).z > EPSILON) {
            add_in_part_of_hull(points_[i], true);
        }
        if (i == points_.size() - 1 || direction_vector.vector_product(points_[i].point - points_[0].point).z < -EPSILON){
            add_in_part_of_hull(points_[i], false);
        }
    }
    size_t copy_move = up_hull.size();
    up_hull.resize(up_hull.size() + down_hull.size() - 2);
    for (size_t i = down_hull.size() - 2; i > 0; --i, ++copy_move) {
        up_hull[copy_move] = down_hull[i];
    }
    return up_hull;
}

void Andrew_convex_hull::add_in_part_of_hull(const Point &point,
                                             bool up) {
    if (up) {
        while (up_hull.size() >= 2 && ((up_hull[up_hull.size() - 1].point - up_hull[up_hull.size() - 2].point).vector_product(
                point.point - up_hull[up_hull.size() - 2].point).z >= 0)){
            up_hull.pop_back();
        }
        up_hull.push_back(point);
    } else {
        while (down_hull.size() >= 2 && ((down_hull[down_hull.size() - 1].point - down_hull[down_hull.size() - 2].point).vector_product(
                point.point - down_hull[down_hull.size() - 2].point).z <= 0)) {
            down_hull.pop_back();
        }
        down_hull.push_back(point);
    }
}

class solution {
public:
    solution(std::istream &in);

    void print_answer(std::ostream &out);

private:
    long double calculate_answer();

    std::vector <Point> points;
    std::vector <modified_vector> parabaloid_points;
    Chan_algorithm Hull_3d;
    Andrew_convex_hull Hull_2d;

};

solution::solution(std::istream &in) {
    long double x, y;
    size_t index = 0;
    while (in >> x >> y) {
        points.emplace_back(Vector<long double>(x, y, 0), index);
        parabaloid_points.emplace_back(Vector<long double>(x, y, x * x + y * y), index);
        ++index;
    }
    Hull_3d.set_points(parabaloid_points);
    Hull_2d.get_points_to_modify() = points;
}

long double solution::calculate_answer() {
    /**
     * Создадим выпуклую оболочку на парабалоиде.
     * Её проекция - диаграмма Вороного
     * Асимптотика O(n log n) Так как алгоритм Чана
     * Но отличие от задачи на алгоритм Чана из этого же контеста в том,
     * что мы строим только нижнюю оболочку, так как ее проекция - триангуляция
     * А диаграмма вороного состоит из серединных перпендикуляров к триангуляции
     */
    Hull_3d.build_hull();
    std::vector <Face> hull3d = Hull_3d.get_hull();
    /**
     * Создадим оболочку алгоритмом Эндрю
     * Так мы найдем точки, которые не входят в ограниченные многоугольники
     * Асимптотика O(n log n)
     */
    std::vector <Point> hull2d = Hull_2d.create_convex_hull();

    std::vector<bool> should_be_counted(points.size(), true);
    std::vector <uint64_t> count_of_edges(points.size(), 0);

    for (const auto &point: hull2d) {
        should_be_counted[point.id] = false;
    }

    /*
     * Воспользуемся сетом, чтобы хранить все ребра в единственном экземпляре
     * При этом асимптотика действий с сетом будет O(logn), тогда для всех вершин
     * получаем асимптотику O(n log n), что и требовалось получить
     */
    std::set <std::pair<uint64_t, uint64_t>> edges;
    for (const auto &face: hull3d) {
        if (face.points[0] < face.points[1]) {
            edges.insert(std::make_pair(face.points[0], face.points[1]));
        } else {
            edges.insert(std::make_pair(face.points[1], face.points[0]));
        }
        if (face.points[2] < face.points[1]) {
            edges.insert(std::make_pair(face.points[2], face.points[1]));
        } else {
            edges.insert(std::make_pair(face.points[1], face.points[2]));
        }
        if (face.points[0] < face.points[2]) {
            edges.insert(std::make_pair(face.points[0], face.points[2]));
        } else {
            edges.insert(std::make_pair(face.points[2], face.points[0]));
        }
    }

    for (const auto &edge: edges) {
        ++count_of_edges[edge.first];
        ++count_of_edges[edge.second];
    }

    uint64_t answer = 0;
    size_t count_of_inside_points = points.size() - hull2d.size();
    if (count_of_inside_points == 0) {
        return 0;
    }
    for (size_t i = 0; i < should_be_counted.size(); ++i) {
        if (should_be_counted[i]) {
            answer += count_of_edges[i];
        }
    }

    return static_cast<long double>(answer) / static_cast<long double>(count_of_inside_points);
}

void solution::print_answer(std::ostream &out) {
    out << calculate_answer();
}

int main() {
    solution answer(std::cin);
    std::cout.precision(7);
    answer.print_answer(std::cout);
    return 0;
}
