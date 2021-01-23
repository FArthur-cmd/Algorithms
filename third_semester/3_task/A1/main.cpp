/**
 * Арсений уснул. И приснилась ему планета Ка-Пэкс, на которой протекают две реки.
 * Эти реки достаточно необычны для землян:
 * они текут строго от одной точки до другой точки напрямую в пространстве
 * (так как в сне Арсения планета вовсе не круглая).
 * Арсений решил, что он хочет прорыть тоннель между реками.
 * Так как он ленивый, то и копать много он не хочет.
 * Помогите Арсению найти, сколько-таки ему придется прорыть.
 *
 * Формат ввода
 *
 * В четырех строках задаются целые координаты начала и конца рек.
 * Координаты по модулю не превосходят 1000.
 *
 * Формат вывода
 *
 * Выведите искомую величину c точностью не менее 6 знаков после десятичной точки.
 */

#include <iostream>
#include <cmath>
#include <vector>
#include <tuple>
#include <algorithm>

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

/*
 * Класс решение со всеми вычислениями
 */
class solution {
public:
    solution() = default;

    solution(const Vector<int64_t> &first_beg, const Vector<int64_t> &first_end,
             const Vector<int64_t> &second_beg, const Vector<int64_t> &second_end);

    double_t calculate_distance();// находит расстояние между отрезками

private:
    void make_normal(); // задать нормаль к плоскости, образованной направляющими векторами

    void make_surfaces();//сделать параллельные плоскости через эти отрезки

    int64_t distance_between_surface() const;// расстояние между параллельными плоскостями

    std::pair<Vector<double_t>, Vector<double_t>> make_projection_of_second();// сделаем проекцию второго вектора на
    //первую плоскость

    double_t calculate_distance_for_projections();//расстояние между проекциями

    double_t calculate_for_parallel();// рассчет для параллельных векторов

    double_t calculate_for_same_surface(const std::pair<Vector<double_t>, Vector<double_t>> &first,
                                        const std::pair<Vector<double_t>, Vector<double_t>> &second);// расстояние на
    //одной плоскости

    // проверка на поворот в одну сторону
    template<typename U>
    bool rotations_in_the_same_direction(const Vector<U> &main_vec, const Vector<U> &first, const Vector<U> &second);

    Vector<int64_t> first_begin; // координаты начала первого вектора
    Vector<int64_t> second_begin;// координаты начала второго вектора
    Vector<int64_t> first_end;   // координаты конца первого вектора
    Vector<int64_t> second_end;  // координаты конца второго вектора
    Vector<int64_t> normal;      // координаты нормального вектора
    bool normal_is_made = false;   // уже считали нормаль?
    int64_t surf_coef_first{};     // Свободный коэффициент первой плоскости
    int64_t surf_coef_second{};    // Свободный коэффициент второй плоскости
};

/*
 * Создаем плоскости
 * Если нормально нулевая, то это параллельные линии, посчитаем это
 * Посчитаем расстояние между проекциями на плоскости и по формуле Пифагора
 */
double_t solution::calculate_distance() {
    make_surfaces();
    if (normal == Vector<int64_t>(0, 0, 0)) {
        return calculate_for_parallel();
    }
    double_t distance_on_surface = calculate_distance_for_projections();
    if (normal.find_sqr_length() != 0) {
        return sqrt(distance_on_surface * distance_on_surface +
                    static_cast<double_t>(distance_between_surface() * distance_between_surface()) /
                    normal.find_sqr_length());
    } else {
        return distance_on_surface;
    }
}


solution::solution(const Vector<int64_t> &first_beg, const Vector<int64_t> &first_end,
                   const Vector<int64_t> &second_beg, const Vector<int64_t> &second_end) : first_begin(first_beg),
                                                                                           first_end(first_end),
                                                                                           second_begin(second_beg),
                                                                                           second_end(second_end) {}

int64_t solution::distance_between_surface() const {
    int64_t difference = surf_coef_second - surf_coef_first;
    return std::abs(difference);
}

void solution::make_surfaces() {
    if (!normal_is_made) {
        make_normal();
    }
    surf_coef_first = -first_begin.dot_product(normal);
    surf_coef_second = -second_begin.dot_product(normal);
}

void solution::make_normal() {
    normal = (first_end - first_begin).vector_product(second_end - second_begin);
}

std::pair<Vector<double_t>, Vector<double_t>> solution::make_projection_of_second() {
    Vector<double_t> first(second_begin);
    // сдвиг
    Vector<double_t> double_normal(normal);
    Vector<double_t> move_to =
            double_normal * static_cast<double_t>(surf_coef_second - surf_coef_first) /
            static_cast<double_t>(normal.find_sqr_length());
    first += move_to;
    Vector<double_t> second(second_end);
    second += move_to;
    return {first, second};
}

double_t solution::calculate_distance_for_projections() {
    std::pair<Vector<double_t>, Vector<double_t>> second_vector = make_projection_of_second();
    Vector<double_t> first_new_vector_beg(first_begin);
    Vector<double_t> first_new_vector_end(first_end);
    /*
     * Проверка на пересечение
     */
    if ((second_vector.second - second_vector.first).vector_product(
            first_new_vector_end - second_vector.first).dot_product(
            (second_vector.second - second_vector.first).vector_product(
                    first_new_vector_beg - second_vector.first)) <= 0 &&
        (first_new_vector_end - first_new_vector_beg).vector_product(
                second_vector.second - first_new_vector_beg).dot_product(
                (first_new_vector_end - first_new_vector_beg).vector_product(
                        second_vector.first - first_new_vector_beg)) <= 0) {
        return 0.0;
    }
    double_t dist = calculate_for_same_surface({first_new_vector_beg, first_new_vector_end}, second_vector);
    double_t min_between_endings = sqrt(static_cast<double_t>(std::min({
                                                                               (second_vector.first - first_new_vector_beg).find_sqr_length(),
                                                                               (second_vector.first - first_new_vector_end).find_sqr_length(),
                                                                               (second_vector.second - first_new_vector_beg).find_sqr_length(),
                                                                               (second_vector.second - first_new_vector_end).find_sqr_length()
                                                                       }
    )));
    if (dist != 0 && dist < min_between_endings) { // минимальное из расстояний на плоскости или между концами отрезков
        return dist;
    }
    return min_between_endings;
}


double_t solution::calculate_for_parallel() {
    Vector<int64_t> first_direction(first_end - first_begin);
    Vector<int64_t> second_direction(second_end - second_begin);

    Vector<int64_t> first_end_second_begin(second_begin - first_end);
    Vector<int64_t> first_begin_second_begin(second_begin - first_begin);
    Vector<int64_t> first_end_second_end(second_end - first_end);
    Vector<int64_t> first_begin_second_end(second_end - first_begin);

    // определяем попадает ли проекция на отрезок  и считаем соответствующую высоту
    if (!rotations_in_the_same_direction(second_direction,
                                         -first_end_second_begin,
                                         -first_end_second_end)) {
        return sqrt(second_direction.vector_product(first_end_second_begin).find_sqr_length() /
                    static_cast<double_t>(second_direction.find_sqr_length()));
    }
    if (!rotations_in_the_same_direction(-second_direction,
                                         -first_begin_second_end,
                                         -first_begin_second_begin)) {
        return sqrt(second_direction.vector_product(first_begin_second_end).find_sqr_length() /
                    static_cast<double_t>(second_direction.find_sqr_length()));
    }
    if (!rotations_in_the_same_direction(first_direction,
                                         first_begin_second_begin,
                                         first_end_second_begin)) {
        return sqrt(first_direction.vector_product(first_begin_second_begin).find_sqr_length() /
                    static_cast<double_t>(first_direction.find_sqr_length()));
    }
    if (!rotations_in_the_same_direction(-first_direction,
                                         first_end_second_end,
                                         first_begin_second_end)) {
        return sqrt(first_direction.vector_product(-first_end_second_end).find_sqr_length() /
                    static_cast<double_t>(first_direction.find_sqr_length()));
    }
    // иначе минимальное расстояние между концами
    return sqrt(std::min({
                                 first_end_second_end.find_sqr_length(),
                                 first_begin_second_end.find_sqr_length(),
                                 first_begin_second_begin.find_sqr_length(),
                                 first_end_second_begin.find_sqr_length()
                         }
    ));
}

template<typename U>
bool solution::rotations_in_the_same_direction(const Vector<U> &main_vec, const Vector<U> &first,
                                               const Vector<U> &second) {
    return (main_vec.dot_product(first) * (main_vec.dot_product(second)) >= 0);
}

/*
 * Возвращает минимальное расстояние между отрезками на плоскости
 * или 0, если нельзя построить высоту
 */
double_t solution::calculate_for_same_surface(const std::pair<Vector<double_t>, Vector<double_t>> &first,
                                              const std::pair<Vector<double_t>, Vector<double_t>> &second) {
    std::vector<double_t> answers;

    Vector<double_t> first_direction(first.second - first.first);
    Vector<double_t> second_direction(second.second - second.first);

    Vector<double_t> first_end_second_begin(second.first - first.second);
    Vector<double_t> first_begin_second_begin(second.first - first.first);
    Vector<double_t> first_end_second_end(second.second - first.second);
    Vector<double_t> first_begin_second_end(second.second - first.first);

    if (!rotations_in_the_same_direction(second_direction,
                                         -first_end_second_begin,
                                         -first_end_second_end)) {
        answers.push_back(sqrt(second_direction.vector_product(first_end_second_begin).find_sqr_length() /
                               static_cast<double_t>(second_direction.find_sqr_length())));
    }
    if (!rotations_in_the_same_direction(-second_direction,
                                         -first_begin_second_end,
                                         -first_begin_second_begin)) {
        answers.push_back(sqrt(second_direction.vector_product(first_begin_second_end).find_sqr_length() /
                               static_cast<double_t>(second_direction.find_sqr_length())));
    }
    if (!rotations_in_the_same_direction(first_direction,
                                         first_begin_second_begin,
                                         first_end_second_begin)) {
        answers.push_back(sqrt(first_direction.vector_product(first_begin_second_begin).find_sqr_length() /
                               static_cast<double_t>(first_direction.find_sqr_length())));
    }
    if (!rotations_in_the_same_direction(-first_direction,
                                         first_end_second_end,
                                         first_begin_second_end)) {
        answers.push_back(sqrt(first_direction.vector_product(-first_end_second_end).find_sqr_length() /
                               static_cast<double_t>(first_direction.find_sqr_length())));
    }

    if (answers.empty()) {
        return 0.0;
    }
    return *std::min_element(answers.begin(), answers.end());
}

int main() {
    Vector<int64_t> first_beg, first_end, second_beg, second_end;
    std::cin >> first_beg >> first_end >> second_beg >> second_end;
    solution answer(first_beg,
                    first_end,
                    second_beg,
                    second_end);
    std::cout.precision(10);
    std::cout << std::fixed << answer.calculate_distance();
}