/**
 * Арсений открыл эзотерический салон в свои снах на планете Ка-Пэкс.
 * У Арсения всё будет хорошо. А вот у его клиентов — не факт.
 * Если хотя бы какие-нибудь их палочки в гадании "Мокусо Дзэй" пересеклись,
 * то день точно сложится удачно. А если нет — то стоит ждать беды.
 * Чтобы точнее сказать, что конкретно пойдет хорошо в этот день, нужно знать,
 * какие именно палочки пересекаются. Помогите Арсению узнать, как пройдет день у его клиентов.
 *
 *
 * Формат ввода
 *
 * Палочка представляется как отрезок прямой.
 * В первой строке входного файла записано число N
 * (1 ≤ N ≤ 125 000) — количество палочек в гадании.
 * Следующие N строк содержат описания палочек:
 * (i + 1)-я строка содержит координаты концов i-й палочки — целые числа x1, y1, x2, y2
 * (-10  000 ≤ x1, y1, x2, y2 ≤ 10  000).
 *
 *
 * Формат вывода
 *
 * В первой строке выходного файла выведите слово "YES",
 * если день сложится удачно. Вторая строка должна содержать числа i и j — номера палочек,
 * которые пересекаются. Если день пройдет плохо, выведите в единственной строке выходного файла "NO".
 */
#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <set>
#include <tuple>
// режимы для алгоритма
const uint8_t INSERT  = 1;
const uint8_t  REMOVE = 2;
const uint32_t  NOT_FOUND  = 10001;
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

// отрезок, хранит начало, конец и свой номер
struct Segment {
    Vector<int64_t> begin_;
    Vector<int64_t> end_;
    uint64_t id;

    Segment(const Vector<int64_t> &begin, const Vector<int64_t> &end, uint64_t number);

    double find_y_in(int64_t x) const;

    bool cross(const Segment &other) const;
};

Segment::Segment(const Vector<int64_t> &begin, const Vector<int64_t> &end, uint64_t number) :
        begin_(begin), end_(end), id(number) {
    if (end_ < begin_) {
        std::swap(begin_, end_);
    }
}
// найти значение y в точки x на прямой, которой принадлежит этот отрезок
double Segment::find_y_in(int64_t x) const {
    if (begin_.x == end_.x) {
        return begin_.y;
    } else {
        return static_cast<double>(begin_.y) + static_cast<double>((end_.y - begin_.y) * (x - begin_.x)) /
                                               static_cast<double>(end_.x - begin_.x);
    }
}
// Проверка на пересечение
// начинается с проверки, что они могут пересечься
// (если один из векторов начинается в координате, которая больше конца другого,
// то они не пересекаются)
// Затем проверяем на пересечение ориентированной площадью.
// Она должна быть разного знака, чтобы концы одного отрезка находились
// по разные стороны от рассматриваемого. Это должно выполняться для обоих векторов,
// иначе может быть случай (--- |) когда они не пересекаются,
// но второй относительно первого удовлетворяет условиям.
// Неравенство нестрогое, чтобы отлавливать случаи, когда конец одного отрезка лежит на другом
bool Segment::cross(const Segment &other) const {
    return std::max(begin_.x, other.begin_.x) <= std::min(end_.x, other.end_.x) + EPSILON&&
           std::max(std::min(begin_.y, end_.y), std::min(other.begin_.y, other.end_.y)) <=
           std::min(std::max(begin_.y, end_.y), std::max(other.begin_.y, other.end_.y)) + EPSILON &&
           (end_ - begin_).vector_product(other.end_ - begin_).z *
           (end_ - begin_).vector_product(other.begin_ - begin_).z <= 0 &&
           (other.end_ - other.begin_).vector_product(end_ - other.begin_).z *
           (other.end_ - other.begin_).vector_product(begin_ - other.begin_).z <= 0;
}
//Бдуем сортировать отрезки по взаимному рассположению (у кого начало ниже, тот меньше)
bool operator<(const Segment &first, const Segment &second) {
    int64_t max_x = std::max(first.begin_.x, second.begin_.x);
    return first.find_y_in(max_x) < second.find_y_in(max_x);
}
// Событие, чтобы выполнить алгоритм не за квадрат.
// Запоминаем координату, тип (вставка или удаление) и номер сегмента в массиве
struct Event {
    int64_t x_coord;
    uint16_t type;
    uint64_t id_of_segment;

    Event() = default;

    Event(int64_t x, int32_t type, uint64_t id);
};

Event::Event(int64_t x, int32_t type, uint64_t id) : x_coord(x), type(type), id_of_segment(id) {}

// Раньше происходят события, у которых x меньше,
// или при равных х вставки обрабатываются раньше, чем удаления, чтобы отыскать случай ---.---
// (конец одного == начало другого)
bool comp(const Event &first, const Event &second) {
    return std::tie(first.x_coord, first.type) < std::tie(second.x_coord, second.type);
}

/*
 * Решение с помощью sweep line (вдохновлен емаксом)
 * Добавляем все события удаления и вставок в список событий.
 * Идем по событиям и добавляем в set рассматриваемые в текущий момент сегменты.
 * В силу реализации set поиск в нем элементов происходит за log(n), что нам и надо
 * Находим соседние отрезки относительно текущего и проверяем на пересечение.
 * Всегда проверяем на пересечение, если можем(может, соседа не нашлось).
 * В добавлении мы сохраняем сегмент и запоминаем, куда его положили в массив where
 * В удалении мы соответственно удаляем рассмотренный (после всех проверок)
 */
struct solution {
    std::vector<Segment> segments;

    std::pair<uint64_t, uint64_t> find_pair() {
        std::vector<Event> events;
        events.reserve(segments.size() * 2);
        for (size_t i = 0; i < segments.size(); ++i) {
            events.emplace_back(segments[i].begin_.x, INSERT, i);
            events.emplace_back(segments[i].end_.x, REMOVE, i);
        }
        std::sort(events.begin(), events.end(), comp);

        std::set<Segment> current_watch;
        std::vector<std::set<Segment>::iterator> where(segments.size());

        for (auto &event : events) {
            uint64_t id = event.id_of_segment;
            if (event.type == INSERT) {
                auto next_segment = current_watch.lower_bound(segments[id]);
                auto previous = next_segment == current_watch.begin() ? current_watch.end() : next_segment;
                // Если в самом начале находится следующий, то в предыдущий положим конец, чтобы было легче отследить
                if (next_segment != current_watch.begin()) {
                    --previous;
                }
                if (next_segment != current_watch.end() && next_segment->cross(segments[id])) {
                    return {next_segment->id, segments[id].id};
                }
                if (previous != current_watch.end() && previous->cross(segments[id])) {
                    return {previous->id, segments[id].id};
                }
                where[id] = current_watch.insert(next_segment, segments[id]);
            } else {
                auto probable_seg = where[id];
                auto previous = where[id] == current_watch.begin() ? current_watch.end() : where[id];
                if (probable_seg != current_watch.begin()) {
                    --previous;
                }
                if ((++probable_seg) != current_watch.end() && previous != current_watch.end() &&
                    probable_seg->cross(*previous)) {
                    return {previous->id, probable_seg->id};
                }
                current_watch.erase(where[id]);
            }
        }
        return {NOT_FOUND, NOT_FOUND};
    }
};

int main() {
    std::ios::sync_with_stdio(false);
    solution result;
    int64_t x1, y1, x2, y2;
    uint64_t N;
    std::cin >> N;
    result.segments.reserve(N);
    for (uint64_t i = 0; i < N; ++i) {
        std::cin >> x1 >> y1 >> x2 >> y2;
        result.segments.emplace_back(Vector<int64_t>(x1, y1), Vector<int64_t>(x2, y2), i + 1);
    }
    std::pair<uint64_t, uint64_t> answer = result.find_pair();
    if (answer.first == NOT_FOUND || answer.second == NOT_FOUND) {
        std::cout << "NO";
    } else {
        std::cout << "YES\n" << answer.first << " " << answer.second;
    }
    return 0;
}
