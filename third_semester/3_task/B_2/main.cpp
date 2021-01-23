/**
 * Арсений продолжает спать. Теперь ему снится кроличья ферма на планете Ка-Пэкс.
 * Правда, ферма у него так себе — одни кроличьи норы в пустом поле.
 * Чтобы её хоть как-то облагородить, Арсений решил поставить забор так,
 * чтобы все норки оказались внутри огражденной территории, а досок он потратил как можно меньше.
 * Помогите Арсению найти длину забора, чтобы он мог уже заказывать стройматериалы!
 *
 * Формат ввода
 *
 * В первой строке вводится число N (3 ≤ N ≤ 105).
 * Следующие N строк содержат пары целых чисел x и y (-109 ≤ x, y ≤ 109)
 * — координаты кроличьих нор.
 *
 * Формат вывода
 *
 * Одно вещественное число — длину забора — с максимально возможной точностью.

 */

#include <iostream>
#include <cmath>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <tuple>

const double EPSILON = 1e-9;

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
bool compare_points(const Vector<T> &first, const Vector<T> &second) {
    return std::tie(first.x, first.y) < std::tie(second.x, second.y);
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
    Andrew_convex_hull(std::istream& in);

    const std::vector<Vector<int64_t>> &get_points() const;

    std::vector<Vector<int64_t>> &get_points_to_modify();

    std::vector<Vector<int64_t>> create_convex_hull();

private:
    void add_in_part_of_hull(const Vector<int64_t> &point, bool up);

    std::vector<Vector<int64_t>> points_;
    std::vector<Vector<int64_t>> up_hull;
    std::vector<Vector<int64_t>> down_hull;
};

Andrew_convex_hull::Andrew_convex_hull(std::istream &in) {
    uint64_t N;
    int64_t x, y;
    in >> N;

    for (size_t i = 0; i < N; ++i) {
        in >> x >> y;
        points_.emplace_back(x, y);
    }
}

const std::vector<Vector<int64_t>> &Andrew_convex_hull::get_points() const {
    return points_;
}

std::vector<Vector<int64_t>> &Andrew_convex_hull::get_points_to_modify() {
    return points_;
}

std::vector<Vector<int64_t>> Andrew_convex_hull::create_convex_hull() {
    std::sort(points_.begin(), points_.end(), compare_points<int64_t>);
    up_hull.push_back(points_[0]);
    down_hull.push_back(points_[0]);
    Vector<int64_t> direction_vector = points_.back() - points_[0];
    for (size_t i = 1; i < points_.size(); ++i) {
        if (i == points_.size() - 1 || direction_vector.vector_product(points_[i] - points_[0]).z > EPSILON) {
            add_in_part_of_hull(points_[i], true);
        }
        if (i == points_.size() - 1 || direction_vector.vector_product(points_[i] - points_[0]).z < -EPSILON){
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

void Andrew_convex_hull::add_in_part_of_hull(const Vector<int64_t> &point,
                                             bool up) {
    if (up) {
        while (up_hull.size() >= 2 && ((up_hull[up_hull.size() - 1] - up_hull[up_hull.size() - 2]).vector_product(
                point - up_hull[up_hull.size() - 2]).z >= 0)){
            up_hull.pop_back();
        }
        up_hull.push_back(point);
    } else {
        while (down_hull.size() >= 2 && ((down_hull[down_hull.size() - 1] - down_hull[down_hull.size() - 2]).vector_product(
                point - down_hull[down_hull.size() - 2]).z <= 0)) {
            down_hull.pop_back();
        }
        down_hull.push_back(point);
    }
}

double_t calculate_perimeter(const std::vector<Vector<int64_t>> &figure) {
    double_t summ = 0;
    for (size_t i = 1; i < figure.size(); ++i) {
        summ += (figure[i] - figure[i - 1]).find_length();
    }
    summ += (figure[0] - figure.back()).find_length();
    return summ;
}


int main() {
    Andrew_convex_hull solution(std::cin);
    std::cout.precision(10);
    std::cout << std::fixed << calculate_perimeter(solution.create_convex_hull());
    return 0;
}
