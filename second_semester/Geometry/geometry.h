#include <vector>
#include <cmath>
/*
Напишите иерархию классов для работы с геометрическими фигурами на плоскости.

    Структура Point - точка на плоскости. Точку можно задать двумя числами типа double.
    Должны быть открыты поля x и y. Точки можно сравнивать операторами == и !=.

    Класс Line - прямая. Прямую можно задать двумя точками, можно двумя числами (угловой коэффициент и сдвиг),
    можно точкой и числом (угловой коэффициент). Линии можно сравнивать операторами == и !=.

    Абстрактный класс Shape - фигура.

    Класс Polygon - многоугольник. Многоугольник - частный случай фигуры.
    У многоугольника можно спросить verticesCount() - количество вершин - и std::vector<Point> getVertices
    - сами вершины без возможности изменения. Можно спросить isConvex() - выпуклый ли.
    Можно сконструировать многоугольник из вектора точек-вершин в порядке обхода.
    Можно сконструировать многоугольник из точек, передаваемых в качестве параметров через запятую
    (т.е. неуказанное число аргументов). Для простоты будем считать, что многоугольники с самопересечениями
    никогда не возникают (гарантируется, что в тестах таковые будут отсутствовать).

    Класс Ellipse - эллипс. Эллипс - частный случай фигуры.
    У эллипса можно спросить std::pair<Point,Point> focuses() - его фокусы;
    std::pair<Line, Line> directrices() - пару его директрис; double eccentricity() - его эксцентриситет,
    Point center() - его центр. Эллипс можно сконструировать из двух точек и double
    (два фокуса и сумма расстояний от эллипса до них);

    Класс Circle - круг. Круг - частный случай эллипса. У круга можно спросить double radius() - радиус.
    Круг можно задать точкой и числом (центр и радиус).

    Класс Rectangle - прямоугольник. Прямоугольник - частный случай многоугольника.
    У прямоугольника можно спросить Point center() - его центр; std::pair<Line, Line> diagonals() - пару его диагоналей.
    Прямоугольник можно сконструировать по двум точкам (его противоположным вершинам)
    и числу (отношению смежных сторон), причем из двух таких прямоугольников выбирается тот,
    у которого более короткая сторона расположена по левую сторону от диагонали, если смотреть от
    первой заданной точки в направлении второй.

    Класс Square - квадрат. Квадрат - частный случай прямоугольника.
    У квадрата можно спросить Circle circumscribedCircle(), Circle inscribedCircle().
    Квадрат можно задать двумя точками - противоположными вершинами.

    Класс Triangle - треугольник. Треугольник - частный случай многоугольника.
    У треугольника можно спросить Circle circumscribedCircle(), Circle inscribedCircle(), Point centroid() -
    его центр масс, Point orthocenter() - его ортоцентр, Line EulerLine() - его прямую Эйлера,
    Circle ninePointsCircle() - его окружность Эйлера.

У любой фигуры можно спросить:

    double perimeter() - периметр;
    double area() - площадь;
    operator==(const Shape& another) - совпадает ли эта фигура с другой;
    isCongruentTo(const Shape& another) - равна ли эта фигура другой в геометрическом смысле;
    isSimilarTo(const Shape& another) - подобна ли эта фигура другой;
    containsPoint(Point point) - находится ли точка внутри фигуры.

С любой фигурой можно сделать:

    rotate(Point center, double angle) - поворот на угол (в градусах, против часовой стрелки) относительно точки;
    reflex(Point center) - симметрию относительно точки;
    reflex(Line axis) - симметрию относительно прямой;
    scale(Point center, double coefficient) - гомотетию с коэффициентом coefficient и центром center.

 В вашем файле должна отсутствовать функция main(), а сам файл должен называться geometry.h.
 В качестве компилятора необходимо выбирать Make. Ваш код будет вставлен посредством #include в программу,
 содержащую тесты
 */

double EPS = 1e-6; // Необходимо учитывать неточности при работе с типом double

/*
 * Сравнение с учетом погрешности вычисления даблов
 */
int compere_double(const double& first, const double& second) {
    if (std::abs(first - second) < EPS) {
        return 0;
    } else {
        if (first - second < 0) {
            return -1;
        } else {
            return 1;
        }
    }
}
/*
 * Структура точка. Умеет создаваться
 */
struct Point {
    double x, y;
    Point(const double x_ = 0, const double y_ = 0) {
        x = x_;
        y = y_;
    }
    Point (const Point& other) {
        x = other.x;
        y = other.y;
    }
};

/*
 * Функция для рассчета расстояний между точками
 */
double calculate_distance(const Point& first, const Point& second) {
    return sqrt(pow((first.x - second.x), 2) + pow((first.y - second.y),2));
}
/*
 * Оператор сравнения точек
 */
bool operator ==(const Point& first, const Point& second) {
    return (compere_double(first.x, second.x) == 0 && compere_double(first.y, second.y) == 0);
}
/*
 * Оператор неравенства точек
 */
bool operator !=(const Point& first, const Point& second) {
    return !(first == second);
}
/*
 * Поворот первый точки относительно второй
 * Принимает синус и косинус, потому что в некоторых случаях мы вычисляем косинус или синус, и чтобы избежать
 * увеличения неточности мы избежим преобразования в угол, а потом снова получения косинуса и синуса
 */
Point rotate_points(const Point& first, const Point& second, double cos_angle, double sin_angle) {
    return Point(second.x + (first.x - second.x) *cos_angle - (first.y - second.y) * sin_angle,
                 second.y + (first.x - second.x) *sin_angle + (first.y - second.y) * cos_angle);
}

/*
 * Класс линия
 * Выполнены требуемые в задании условия
 */
class Line {
protected:
    double AngleCoefficient, FreePart;
public:
    Line(const Point& first, const Point& second) {
        AngleCoefficient = (first.y - second.y)/(first.x - second.x);
        FreePart = first.y - AngleCoefficient*first.x;
        if (AngleCoefficient == -0){ //горизонтальная прямая
            AngleCoefficient = 0;
        }
        if (AngleCoefficient == -INFINITY){ // вертикальная прямая
            AngleCoefficient = INFINITY;
        }
        if (AngleCoefficient == INFINITY) {// теперь уравнения вида х = FreePart
            FreePart = first.x;
        }
    }
    Line(const double& first, const double& second) {
        AngleCoefficient = first;
        FreePart = second;
    }
    Line(const Point& first, const double& second) {
        AngleCoefficient = second;
        FreePart = first.y - first.x*AngleCoefficient;
    }
    const double& GetAngleCoefficient() const {
        return AngleCoefficient;
    }
    const double& GetFreePart() const {
        return FreePart;
    }
};

/*
 * Сравнение линий
 */
bool operator ==(const Line& first, const Line& second) {
    return ((compere_double(first.GetAngleCoefficient(), second.GetAngleCoefficient()) == 0 ||
            compere_double(first.GetAngleCoefficient(), -second.GetAngleCoefficient()) == 0) &&
            compere_double(first.GetFreePart(), second.GetFreePart()) == 0);
}

bool operator !=(const Line& first, const Line& second) {
    return !(first == second);
}

/*
 * Абстрактный класс Shape
 * Все классы далее просто удовлетворяют условию, поэтому я буду пояснять только нетривиальные случаи
 */
class Shape {
public:
    virtual double perimeter() = 0;
    virtual double area() = 0;
    virtual bool operator==(const Shape& another) {
        return false;
    }
    virtual bool operator!=(const Shape& another) = 0;
    virtual bool isCongruentTo(const Shape& another) = 0;
    virtual bool isSimilarTo(const Shape& another) = 0;
    virtual bool containsPoint(Point point) = 0;
    virtual void rotate(Point center, double angle) = 0;
    virtual void reflex(Point center) = 0;
    virtual void reflex(Line axis) = 0;
    virtual void scale(Point center, double coefficient) = 0;
    virtual ~Shape() = default;
};

class Polygon : public Shape {
protected:
    std::vector<Point> points;
    /*
     * Ищем с какой стороны относительно предыдущего ребра находится меньший угол(в какую сторону поворот при обходе)
    */
    static bool turning_left(const Point& first_begin, const Point& first_end, const Point& second_end) {
        return (first_end.x - first_begin.x) * (second_end.y - first_end.y) -
               (first_end.y - first_begin.y) * (second_end.x - first_end.x) > 0;
    }
    /*
     * Находи косинус угла из скалярного произведения
     */
   double find_angle_cos(const Point& first_begin, const Point& first_end, const Point& second_end) {
       return ((pow(calculate_distance(points[0], points[1]),2) +
                pow(calculate_distance(points[1], points[2]),2) -
                pow(calculate_distance(points[2], points[0]),2))/(2 *
                pow(calculate_distance(points[0], points[1]),2) *
                pow(calculate_distance(points[1], points[2]),2)));
   }
   /*
    * Трассировка(пускаем луч в одну из сторон, если количество пересеченных сторон нечетное - точка внутри, вершины
    * многоугольника принадлежат 2 сторонам)
    */
    bool Cross(unsigned long long j, const Point& point) {
        unsigned long long first = j;
        unsigned long long second = (j == points.size() - 1 ? 0 : j + 1);
        double y = (point.x - points[first].x) * (points[second].y - points[first].y) /
                (points[second].x - points[first].x) + points[first].y;
        double minimal = std::min(points[first].x, points[second].x);
        double maximal = std::max(points[first].x, points[second].x);
        return (compere_double(points[first].x, points[second].x) && (compere_double(point.y, y) >= 0) &&
        (compere_double(point.x, minimal) >= 0)  && (compere_double(point.x, maximal) <= 0));
    }
public:
    explicit Polygon(const std::vector<Point>& points_) {
        points = points_;
    }
    template <typename ...T>
    explicit Polygon(const T& ... args) {
        bool flag = false;
        for (auto&& point: std::initializer_list<Point>{args...}) {
            for (const auto & i : points) {
                if (i == point) {
                    flag = true;
                    break;
                }
            }
            if (!flag)
                points.push_back(point);
        }
    }
    unsigned long long verticesCount() const {
        return points.size();
    }
    const std::vector<Point>& getVertices() const {
        return points;
    }
    /*
     * Если повороты были в одну сторону, а потом нашелся в другую - не выпуклая фигура
     */
    bool isConvex() const{
        if (points.size() < 3) {
            return false;
        }
        bool turn_left = turning_left(points[points.size() - 1], points[0], points[1]);
        for (unsigned long long i = 0; i < points.size() - 2; ++i) {
            if (turning_left(points[i], points[i+1], points[i+2]) != turn_left) {
                return false;
            }
        }
        return turning_left(points[points.size() - 2], points[points.size() - 1], points[0])
               == turn_left;
    }
    double perimeter() override {
        double result = sqrt(pow((points[points.size() - 1].x - points[0].x), 2) +
                pow((points[points.size() - 1].y - points[0].y),2));
        for (unsigned long long i = 0; i < points.size() - 1; ++i) {
            result += sqrt(pow((points[i].x - points[i + 1].x), 2) +
                           pow((points[i].y - points[i + 1].y),2));
        }
        return result;
    }
    double area() override {
        double result =(points[points.size() - 1].x - points[0].x) * (points[points.size() - 1].y + points[0].y) / 2;
        for (unsigned long long i = 0; i < points.size() - 1; ++i) {
            result += (points[i].x - points[i + 1].x) * (points[i].y + points[i + 1].y)/2;
        }
        if (result < 0) {
            result = -result;
        }
        return result;
    }
    /*
     * Находим есть ли совпадение по точкам в прямом или обратном обходе
     */
    bool operator==(const Shape& another) override {
        try {
            const auto &second = dynamic_cast<const Polygon&>(another);
            if (points.size() != second.points.size()) {
                return false;
            }
            for (unsigned long long i = 0; i < points.size(); ++i) {
                if (points[i] == second.points[0]) {
                    if (points[(i+1) % points.size()] == second.points[1]) {
                        for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                            if (points[(i + j) % points.size()] != second.points[j]) {
                                return false;
                            }
                        }
                        return true;
                    }
                    if (points[(points.size() + i + 1) % points.size()] ==
                        second.points[points.size()-1]) {
                        for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                            if (points[(points.size() + i + j) % points.size()] !=
                            second.points[(points.size()- j)]) {
                                return false;
                            }
                        }
                        return true;
                    }
                }
            }
            return false;
        } catch (...){
            return false;
        }
    }
    bool isCongruentTo(const Shape& another) override {
        try {
            const auto &second = dynamic_cast<const Polygon &>(another);
            if (verticesCount() != second.verticesCount()) {
                return false;
            }
            if (isConvex() != second.isConvex())
                return false;
            std::vector<double> edges, edges_second;
            edges.push_back(calculate_distance(points[0], points[points.size() - 1]));
            edges_second.push_back(calculate_distance(second.points[0], second.points[points.size() - 1]));
            for (unsigned long long i = 0; i < points.size() - 1; ++i) {
                edges.push_back(calculate_distance(points[i], points[i + 1]));
                edges_second.push_back(calculate_distance(second.points[i], second.points[i + 1]));
            }
            for (unsigned long long i = 0; i < points.size(); ++i) {
                if (compere_double(edges[i], edges_second[0]) == 0) {
                    if (compere_double(edges[(i+1) % edges.size()], edges_second[1]) == 0) {
                        for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                            if (compere_double(edges[(i + j) % edges.size()], edges_second[j]) != 0) {
                                return false;
                            }
                        }
                        return true;
                    }
                    if (compere_double(edges[(edges.size() + i + 1) % edges.size()],
                            edges_second[edges.size()-1]) == 0) {
                        for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                            if (compere_double(edges[(edges.size() + i + j) % edges.size()],
                                    edges_second[edges.size() - j]) != 0) {
                                return false;
                            }
                        }
                        return true;
                    }
                }
            }
            return false;
        } catch (...) {
            return false;
        }
    }
    /*
     * Подобны, когда углы совпадают
    */
    bool isSimilarTo(const Shape& another) override try {
        const auto &second = dynamic_cast<const Polygon &>(another);
        if (verticesCount() != second.verticesCount()) {
            return false;
        }
        std::vector<double> angles, angles_second;
        for (unsigned long long i = 0; i < points.size(); ++i) {
            angles.push_back(find_angle_cos(points[i],
                                            points[(i + 1) % points.size()],
                                            points[(i + 2) % points.size()]));
            angles_second.push_back(find_angle_cos(second.points[i],
                                                   second.points[(i + 1) % points.size()],
                                                   second.points[(i + 2) % points.size()]));
        }
        for (unsigned long long i = 0; i < points.size(); ++i) {
            if (! compere_double(angles[i], angles_second[0])) {
                if (! compere_double(angles[(i+1) % angles.size()], angles_second[1])) {
                    for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                        if (compere_double(angles[(i + j) % angles.size()], angles_second[j])) {
                            return false;
                        }
                    }
                    return true;
                }
                if (!compere_double(angles[(angles.size() + i-1) % angles.size()],
                    angles_second[angles.size()-1])) {
                    for (unsigned long long j = 2; j < second.verticesCount(); ++j) {
                        if (compere_double(angles[(angles.size() + i + j) % angles.size()],
                            angles_second[angles.size() - j])) {
                            return false;
                        }
                    }
                    return true;
                }
            }
        }
        return false;
    } catch (...) {
        return false;
    }
    bool containsPoint(Point point) override {
        unsigned long long counter = 0;
        for (unsigned long long i = 0; i < points.size(); ++i) {
            counter += Cross(i, point);
        }
        return counter % 2 == 1;
    }
    void rotate(Point center, double angle) override {
        for (auto & point : points) {
            point = rotate_points(point, center, cos(M_PI * angle/180), sin(M_PI * angle/180));
        }
    }
    void reflex(Point center) override {
        for (auto & point : points) {
            point = rotate_points(point, center, -1, 0);
        }
    }
    /*
     * Проведем прямую через искомую точку, проведя прямую симметричую прямой через известную точку относительно
     * данной в условии прямой. Проведем перпендикулярную прямую через известную точку и найдем пересечение
    */
    void reflex(Line axis) override {
        if (axis.GetAngleCoefficient() == 0) {
            for (auto & point: points) {
                point.y = 2 * axis.GetFreePart()  - point.y;
            }
        } else if (axis.GetAngleCoefficient() == INFINITY) {
            for (auto & point: points) {
                point.x = 2 * axis.GetFreePart()  - point.x;
            }
        } else {
            for (auto & point : points) {
                Line through_the_point(axis.GetAngleCoefficient(),
                        2 * axis.GetFreePart() - Line(point, axis.GetAngleCoefficient()).GetFreePart());
                Line through_the_point_second(point, -1/axis.GetAngleCoefficient());
                double x = (through_the_point_second.GetFreePart() - through_the_point.GetFreePart())/
                        (through_the_point.GetAngleCoefficient() - through_the_point_second.GetAngleCoefficient());
                double y = through_the_point.GetAngleCoefficient() * x + through_the_point.GetFreePart();
                point = Point(x, y);
            }
        }
    }
    void scale(Point center, double coefficient) override  {
        for (auto & point : points) {
            point.x = center.x + coefficient * (point.x - center.x);
            point.y = center.y + coefficient * (point.y - center.y);
        }
    }
    bool operator!=(const Shape& another) override {
        return !(*this == another);
    }
};

class Ellipse : public Shape {
protected:
    double distance;
    std::pair<Point, Point> focuses_;
public:
    Ellipse (const Point& first, const Point& second, const double& dist) {
        focuses_.first = first;
        focuses_.second = second;
        distance = dist;
    }
    const std::pair<Point, Point>& focuses() const {
        return focuses_;
    }
    Point center() {
        Point center((focuses_.first.x + focuses_.second.x)/2, (focuses_.first.y + focuses_.second.y)/2);
        return center;
    }
    double eccentricity() const {
        double c = calculate_distance(focuses_.first, focuses_.second)/ 2;
        return (2 * c / distance);
    }
    std::pair<Line, Line> directrices() {
        Line connect_focuses(focuses_.first, focuses_.second);
        Line middle_line(connect_focuses.GetAngleCoefficient(), center());
        double c = calculate_distance(focuses_.first, focuses_.second) / 2;
        double a = distance/2;
        Line first_dir(middle_line.GetAngleCoefficient(), middle_line.GetFreePart() - a*a/c/
        sqrt(1 + middle_line.GetAngleCoefficient()*middle_line.GetAngleCoefficient()));
        Line second_dir(middle_line.GetAngleCoefficient(), middle_line.GetFreePart() + a*a/c/
        sqrt(1 + middle_line.GetAngleCoefficient()*middle_line.GetAngleCoefficient()));
        return std::pair<Line, Line>(first_dir, second_dir);
    }
    /*
    * Используется приближенная формула Рамануджана
    */
    double perimeter() override {
        double c = calculate_distance(focuses_.first, focuses_.second) / 2;
        double a = distance/2;
        double b = a*sqrt(1 - c*c/(a*a));
        return M_PI*(a+b)*(1+(3*pow((a-b)/(a+b), 2))/(10 + sqrt(4 - 3*pow((a-b)/(a+b), 2))));
    }
    double area() override {
        double c = calculate_distance(focuses_.first, focuses_.second) / 2;
        double a = distance/2;
        return M_PI*a*a*sqrt(1 - c*c/(a*a));
    }
    bool operator==(const Shape& another) override {
        try {
            const auto &second = dynamic_cast<const Ellipse&>(another);
            return focuses_.first == second.focuses_.first &&
                   focuses_.first == second.focuses_.second &&
                   compere_double(distance ,second.distance) == 0;
        } catch (...){
            return false;
        }
    }
    bool isCongruentTo(const Shape& another) override {
        try {
            const auto &second = dynamic_cast<const Ellipse &>(another);
            return calculate_distance(focuses_.first, focuses_.second) ==
                   calculate_distance(second.focuses_.first, second.focuses_.second) &&
                   distance == second.distance;
        } catch (...){
            return false;
        }
    }
    bool isSimilarTo(const Shape& another) override {
        try {
            const auto &second = dynamic_cast<const Ellipse &>(another);
            return calculate_distance(focuses_.first, focuses_.second) * second.distance ==
                   calculate_distance(second.focuses_.first, second.focuses_.second) * distance;
        } catch (...){
            return false;
        }
    }
    bool containsPoint(Point point) override {
        return compere_double(calculate_distance(point, focuses_.first) + calculate_distance(point, focuses_.second),
                              distance) != 1;
    }
    void rotate(Point center, double angle) override {
        focuses_.first = rotate_points(focuses_.first, center, cos(angle/180*M_PI), sin(angle/180*M_PI));
        focuses_.second = rotate_points(focuses_.second, center, cos(angle/180*M_PI), sin(angle/180*M_PI));
    }
    void reflex(Point center) override {
        focuses_.first = Point(2 * center.x - focuses_.first.x, 2 * center.y - focuses_.first.y);
        focuses_.second = Point(2 * center.x - focuses_.second.x, 2 * center.y - focuses_.second.y);
    }
    void reflex(Line axis) override {
        focuses_.first.x = (Line(focuses_.first, axis.GetAngleCoefficient()).GetFreePart() - axis.GetFreePart()) *
                  axis.GetAngleCoefficient() / (axis.GetAngleCoefficient() * axis.GetAngleCoefficient() + 1);
        focuses_.first.y = axis.GetAngleCoefficient() * focuses_.first.x + axis.GetFreePart();
        focuses_.second.x = (Line(focuses_.second, axis.GetAngleCoefficient()).GetFreePart() - axis.GetFreePart()) *
                           axis.GetAngleCoefficient() / (axis.GetAngleCoefficient() * axis.GetAngleCoefficient() + 1);
        focuses_.second.y = axis.GetAngleCoefficient() * focuses_.second.x + axis.GetFreePart();
    }
    void scale(Point center, double coefficient) override {
        focuses_.first = Point(center.x + coefficient * (focuses_.first.x - center.x),
                               center.y + coefficient * (focuses_.first.y - center.y));
        focuses_.second = Point(center.x + coefficient * (focuses_.second.x - center.x),
                               center.y + coefficient * (focuses_.second.y - center.y));
        distance *= std::abs(coefficient);
    }
    bool operator!=(const Shape& another) override {
        return !(*this == another);
    }
    double distance_() const {
        return distance;
    }
};

class Circle : public Ellipse {
protected:
    Point center_;
    double radius_;
public:
    Circle(const Point &center, const double rad) : Ellipse(center, center, 2*rad) {
        center_ = center;
        radius_ = rad;
    }
    double radius() {
        return radius_;
    }
    Point getCenter() {
        return center_;
    }
    double perimeter() override {
        return 2 * M_PI * radius_;
    }
    double area() override {
        return M_PI * radius_ * radius_;
    }
    bool operator==(const Shape& another) override {
        try {
            const Ellipse& second = dynamic_cast<const Ellipse&>(another);
            return center_ == second.focuses().first &&
                    center_ == second.focuses().second &&
                    compere_double(second.eccentricity(), 0) == 0 &&
                    compere_double(radius_, second.distance_()/2) == 0;
        } catch (...){
            return false;
        }
    }
    bool isSimilarTo(const Shape& another) override {
        try {
            const Circle &second = dynamic_cast<const Circle &>(another);
            return compere_double(second.eccentricity(), 0) == 0;
        } catch (...){
            return false;
        }
    }
    bool containsPoint(Point point) override {
        return compere_double(calculate_distance(point, center_), radius_) != 1;
    }
    bool isCongruentTo(const Shape& another) override {
        try {
            const Circle &second = dynamic_cast<const Circle &>(another);
            return compere_double(second.eccentricity(), 0) == 0 && radius_ == second.radius_;
        } catch (...){
            return false;
        }
    }
    void rotate(Point center, double angle) override {
        center_ = rotate_points(center, center, cos(angle/180*M_PI), sin(angle/180*M_PI));
    }
    void reflex(Point center) override {
        center_ = Point(2 * center.x -center_.x, 2 * center.y - center_.y);
    }
    void reflex(Line axis) override {
        center_.x = (Line(center_, axis.GetAngleCoefficient()).GetFreePart() - axis.GetFreePart()) *
                           axis.GetAngleCoefficient() / (axis.GetAngleCoefficient() * axis.GetAngleCoefficient() + 1);
        center_.y = axis.GetAngleCoefficient() * center_.x + axis.GetFreePart();
    }
    void scale(Point center, double coefficient) override {
        center_ = Point(center.x + coefficient * (center_.x - center.x),
                     center.y + coefficient * (center_.y - center.y));
        radius_ *= std::abs(coefficient);
    }
    bool operator!=(const Shape& another) override {
        return !(*this == another);
    }
};

class Rectangle : public Polygon {
protected:
    std::vector<Point> points;
    double coefficient_;
public:
    Rectangle(const Point& first, const Point& second, double coefficient) :
    Polygon(first,
            Point(rotate_points(first,
                    Point((first.x + second.x)/2,
                    (first.y + second.y)/2),
                     1 - 2/(1 + (compere_double(coefficient, 1) != -1 ?
                                (coefficient * coefficient) :(1 / (coefficient * coefficient)))),
                     -sqrt(1 - pow(1 - 2/(1 + (compere_double(coefficient, 1) != -1 ?
                     (coefficient * coefficient) :(1 / (coefficient * coefficient)))), 2)))),
            second,
            Point(rotate_points(second,
                         Point((first.x + second.x)/2,
                               (first.y + second.y)/2),
                         1 - 2/(1 + (compere_double(coefficient, 1) != -1 ?
                         (coefficient * coefficient) : (1 / (coefficient * coefficient)))),
                         -sqrt(1 - pow(1 - 2/(1 + (compere_double(coefficient, 1) != -1 ?
                         (coefficient * coefficient) : (1 / (coefficient * coefficient)))), 2))))) {
        coefficient_ = coefficient;
        points = Polygon::getVertices();
    }
    Point center() {
        return Point((points[0].x + points[2].x)/2, (points[0].y + points[2].y)/2);
    }
    std::pair<Line, Line> diagonals() {
        return std::pair<Line, Line>(Line(points[0], points[2]), Line(points[1], points[3]));
    }
    double perimeter() override {
        return 2 * (calculate_distance(points[0], points[1]) + calculate_distance(points[1], points[2]));
    }
    double area() override {
        return (calculate_distance(points[0], points[1]) * calculate_distance(points[1], points[2]));
    }
    bool isSimilarTo(const Shape& another) override {
        try {
            const Rectangle &second = dynamic_cast<const Rectangle&>(another);
            return coefficient_ == second.coefficient_;
        } catch (...){
            return false;
        }
    }
};

class Square : public Rectangle {
protected:
    std::vector<Point> points;
public:
    Square (const Point& first, const Point& second) : Rectangle(first, second, 1) {
        points.push_back(Rectangle::points[0]);
        points.push_back(Rectangle::points[1]);
        points.push_back(Rectangle::points[2]);
        points.push_back(Rectangle::points[1]);
    }
    Circle circumscribedCircle() {
        return Circle(center(), calculate_distance(center(), points[0]));
    }
    Circle inscribedCircle() {
        return Circle(center(),
                calculate_distance(center(),
                                   Point((points[0].x + points[1].x)/2, (points[0].y + points[1].y)/2)));
    }
    double perimeter() override {
        return 4 * calculate_distance(points[0], points[2]);
    }
    double area() override {
        return pow(calculate_distance(points[0], points[2]), 2);
    }
    bool isSimilarTo(const Shape& another) override {
        try {
            const Square &second = dynamic_cast<const Square&>(another);
            return true;
        } catch (...){
            return false;
        }
    }
};

class Triangle : public Polygon {
protected:
    std::vector<Point> points;
public:
    Triangle(const Point& first, const Point& second, const Point& third) :
    Polygon(first, second, third) {
        points = Polygon::getVertices();
    }
    Circle circumscribedCircle() {
        double D = (points[0].x*(points[1].y - points[2].y) +
                points[1].x*(points[2].y - points[0].y) +
                points[2].x*(points[0].y - points[1].y));
        double x_coordinate = -((pow(points[1].x,2) + pow(points[1].y,2) -
                pow(points[2].x,2) - pow(points[2].y,2))* points[0].y + points[1].y *
                (pow(points[2].x,2) + pow(points[2].y,2) - pow(points[2].x,2) - pow(points[2].y,2)) +
                points[2].y * (pow(points[0].x,2) + pow(points[0].y,2) - pow(points[1].x,2) - pow(points[1].y,2)))/D;
        double y_coordinate = ((pow(points[1].x,2) + pow(points[1].y,2) -
                pow(points[2].x,2) - pow(points[2].y,2))* points[0].x + points[1].x *
                 (pow(points[2].x,2) + pow(points[2].y,2) - pow(points[2].x,2) - pow(points[2].y,2)) +
                  points[2].x * (pow(points[0].x,2) + pow(points[0].y,2) - pow(points[1].x,2) - pow(points[1].y,2)))/D;;
        Point center(x_coordinate, y_coordinate);
        return Circle(center,calculate_distance(center, points[0]));
    }
    Circle inscribedCircle() {
        double first_edge = calculate_distance(points[0], points[1]);
        double second_edge = calculate_distance(points[1], points[2]);
        double third_edge = calculate_distance(points[2], points[0]);
        Point centre((first_edge*points[2].x + second_edge*points[0].x + third_edge*points[1].x)/(first_edge +
        second_edge + third_edge), (first_edge*points[2].y + second_edge*points[0].y +
        third_edge*points[1].y)/(first_edge + second_edge + third_edge));
        Line edge(points[0], points[1]);
        double radius;
        if ((edge.GetAngleCoefficient() == 0) || (edge.GetAngleCoefficient() == INFINITY)) {
            radius = std::abs(edge.GetFreePart() - Line(centre, edge.GetAngleCoefficient()).GetFreePart());
        } else {
            radius = std::abs(edge.GetFreePart() - Line(centre, edge.GetAngleCoefficient()).GetFreePart())/
                    sqrt(1 + edge.GetAngleCoefficient() * edge.GetAngleCoefficient());
        }
        return Circle(centre, radius);
    }
    double perimeter() override {
        return calculate_distance(points[0], points[1]) +
               calculate_distance(points[1], points[2]) +
               calculate_distance(points[2], points[0]);
    }
    double area() override {
        return inscribedCircle().radius() * perimeter() / 2;
    }
    Point centroid() {
        return Point((points[0].x + points[1].x+points[2].x)/3, (points[0].y + points[1].y+points[2].y)/3);
    }
    /*
     *  Проводим ребра и высоты и ищем пересичение высот
     */
    Point orthocenter() {
        Line first_edge(points[0], points[1]);
        Line second_edge(points[1], points[2]);
        Line third_edge(points[2], points[0]);
        Line first_hight(points[2], -1/first_edge.GetAngleCoefficient());
        Line second_hight(points[0], -1/second_edge.GetAngleCoefficient());
        Line third_hight(points[1], -1/third_edge.GetAngleCoefficient());
        double x, y;
        if (first_hight.GetAngleCoefficient() != INFINITY && first_hight.GetAngleCoefficient() != -INFINITY) {
            if (second_hight.GetAngleCoefficient() != INFINITY && second_hight.GetAngleCoefficient() != -INFINITY) {
                x = (second_hight.GetFreePart() - first_hight.GetFreePart()) /
                        (first_hight.GetAngleCoefficient() - second_hight.GetAngleCoefficient());
                y = first_hight.GetAngleCoefficient()*x + first_hight.GetFreePart();
            } else {
                x = (third_hight.GetFreePart() - first_hight.GetFreePart()) /
                           (first_hight.GetAngleCoefficient() - third_hight.GetAngleCoefficient());
                y = first_hight.GetAngleCoefficient()*x + first_hight.GetFreePart();
            }
        } else {
            x = (third_hight.GetFreePart() - second_hight.GetFreePart()) /
                       (second_hight.GetAngleCoefficient() - third_hight.GetAngleCoefficient());
            y = second_hight.GetAngleCoefficient()*x + second_hight.GetFreePart();
        }
        return Point(x, y);
    }
    Line EulerLine() {
        return Line(orthocenter(), circumscribedCircle().getCenter());
    }
    Circle ninePointsCircle() {
        Circle circumscribed = circumscribedCircle();
        Point center_of_nine_points((orthocenter().x + circumscribed.center().x)/2,
                (orthocenter().y + circumscribed.center().y)/2);
        Circle answer(center_of_nine_points, circumscribed.radius()/2);
        return answer;
    }
};
