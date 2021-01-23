/**
 * Арсений опять уснул. И снова на планете Ка-Пэкс без него никак —
 * два фермера поссорились из-за того, что их территории могут перекрываться.
 * Естественно, как самого рассудительного, Арсения позвали урегулировать конфликт.
 * Для начала он решил узнать, насколько серьезен конфликт. Для этого он решил узнать,
 * пересекаются ли фермы вообще. Помогите Арсению это понять, если известно,
 * что каждая ферма представляет собой выпуклый многоугольник.
 *
 *
 * Формат ввода
 *
 * Первая строка содержит число N точек первого многоугольника.
 * Затем идут N строк с координатами точек первого многоугольника
 * по часовой стрелке (координаты – действительные числа).
 * Второй прямоугольник задается аналогично. N, M ≤ 80000.
 *
 *
 * Формат вывода
 *
 * Выведите “YES”, если фермы пересекаются, и “NO”, если нет (без кавычек).
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
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


/*
 * Решение требуется реализовать решение за  O(N+M).
 * Очевидно, что решение будет с помощью суммы Минковского.
 * Для этого надо взять первое множетсво и отрицание ко второму.
 * Если (0,0) принадлежит сумме, то есть пересечение.
 */
class solution {
public:
    solution(std::vector<Vector<double>> first, std::vector<Vector<double>> second);

    bool are_cross();

private:
    void build_minkovsk_sum();

    static void make_right(std::vector<Vector<double>> &points, bool opposite_sign = false);

    std::vector<Vector<double>> first_points;
    std::vector<Vector<double>> second_points;
    std::vector<Vector<double>> minkovsk_sum;
};

solution::solution(std::vector<Vector<double>> first, std::vector<Vector<double>> second) :
        first_points(std::move(first)),
        second_points(std::move(second)) {
    make_right(first_points);
    make_right(second_points, true);
    build_minkovsk_sum();
}

/*
 * У нас оборот по часовой, а угол удобнее отслеживать по часовой, поэтому развернем
 * массив точек. Так же найдем крайнюю точку в каждом массиве(чтобы начинать с одного и
 * того же угла. В нашем случае самый левый, нижний)
 * И нужно ли поменять знак, говорит нам opposite_sign
 */
void solution::make_right(std::vector<Vector<double>> &points, bool opposite_sign) {
    if (opposite_sign) {
        for (auto &point : points) {
            point = -point;
        }
    }
    Vector<double> for_reversing{};
    for (size_t i = 0; i < points.size() / 2; ++i) {
        for_reversing = points[i];
        points[i] = points[points.size() - 1 - i];
        points[points.size() - 1 - i] = for_reversing;
    }
    std::vector<Vector<double>> buffer(points);
    Vector<double> min_value(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());
    size_t index_of_min = 0;
    for (size_t i = 0; i < points.size(); ++i) {
        if (points[i] < min_value) {
            min_value = points[i];
            index_of_min = i;
        }
    }
    if (index_of_min != 0) {
        for (size_t i = 0; i < points.size(); ++i) {
            points[i] = buffer[(index_of_min + i) % points.size()];
        }
    }
}

// Вдохновлен neerc'ом (добавляем точки, которые на меньший угол отклоняются)
void solution::build_minkovsk_sum() {
    size_t first_iter = 0;
    size_t second_iter = 0;
    size_t first_size = first_points.size();
    size_t second_size = second_points.size();
    first_points.push_back(first_points[0]);
    second_points.push_back(second_points[0]);
    while (first_iter < first_size && second_iter < second_size) {
        minkovsk_sum.push_back(first_points[first_iter] + second_points[second_iter]);
        if ((first_points[first_iter + 1] - first_points[first_iter]).vector_product(
                second_points[second_iter + 1] - second_points[second_iter]).z > EPSILON) {
            ++first_iter;
        } else if ((first_points[first_iter + 1] - first_points[first_iter]).vector_product(
                second_points[second_iter + 1] - second_points[second_iter]).z < -EPSILON) {
            ++second_iter;
        } else {
            ++first_iter;
            ++second_iter;
        }
    }
}

// Крутимся по всей фигуре. Так как получили выпуклую оболочку, то ноль будет внутри,
// только если совершим полный оборот по фигуре, то есть угол между соседними всегда в одну сторону
bool solution::are_cross() {
    for (size_t i = 0; i < minkovsk_sum.size(); ++i) {
        if (minkovsk_sum[i].vector_product(minkovsk_sum[(i + 1) % minkovsk_sum.size()]).z < -EPSILON) {
            return false;
        }
    }
    return true;
}


int main() {
    std::vector<Vector<double>> buffer_first, buffer_second;
    size_t N, M;
    std::cin >> N;
    buffer_first.resize(N);
    for (size_t i = 0; i < N; ++i) {
        std::cin >> buffer_first[i].x >> buffer_first[i].y;
    }
    std::cin >> M;
    buffer_second.resize(M);
    for (size_t i = 0; i < M; ++i) {
        std::cin >> buffer_second[i].x >> buffer_second[i].y;
    }
    solution result(buffer_first, buffer_second);
    if (result.are_cross()) {
        std::cout << "YES";
    } else {
        std::cout << "NO";
    }

    return 0;
}
