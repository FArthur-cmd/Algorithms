/*
 *

В этой задаче разрешается подключать <iostream>, <vector> и <string> и только их.

Напишите класс BigInteger для работы с длинными целыми числами. Должны поддерживаться операции:

    сложение, вычитание, умножение, деление, остаток по модулю, работающие так же, как и для int;
    составное присваивание с этими операциями. Умножение должно работать за o-малое от n2.
    унарный минус, префиксный и постфиксный инкремент и декремент. Префиксный инкремент и декремент
    должны работать за O(1) в среднем.
    операторы сравнения == != < > <= >=
    вывод в поток и ввод из потока
    метод toString(), возвращающий строковое представление числа
    конструирование из int (в том числе неявное преобразование, когда это надо)
    неявное преобразование в bool, когда это надо (должно работать в условных выражениях)
    опционально - литеральный суффикс bi для написания литералов типа BigInteger, см. справку
    здесь https://en.cppreference.com/w/cpp/language/user_literal

Используя класс BigInteger, напишите класс Rational для работы с рациональными числами сколь угодно
 высокой точности. Числа Rational должны представляться в виде несократимых обыкновенных дробей,
 где числитель и знаменатель – сколь угодно длинные целые числа. Должны поддерживаться операции:

    конструктор из BigInteger, из int
    сложение, вычитание, умножение, деление, составное присваивание с этими операциями
    унарный минус
    операторы сравнения == != < > <= >=
    метод toString(), возвращающий строковое представление числа (вида [минус]числитель/знаменатель),
    где числитель и знаменатель - взаимно простые числа; если число на самом деле целое, то знаменатель выводить не надо
    метод asDecimal(sizet precision=0), возвращающий строковое представление числа в виде десятичной
    дроби с precision знаками после запятой
    оператор приведения к double

В вашем файле должна отсутствовать функция main(), а сам файл должен называться biginteger.h.
 В качестве компилятора необходимо указывать Make. Ваш код будет вставлен посредством команды
 #include<biginteger.h> в программу, содержащую тесты; вследствие этого код необходимо отправлять
 в файле со строго соответствующим именем!

 */
#include <iostream>
#include <string>
#include <vector>

class BigInteger;

BigInteger operator+(const BigInteger& first, const BigInteger& second);
BigInteger operator+(const int32_t& first, const BigInteger& second);
BigInteger operator+(const BigInteger& first, const int32_t& second);

BigInteger operator-(const BigInteger& first, const BigInteger& second);
BigInteger operator-(const int32_t& first, const BigInteger& second);
BigInteger operator-(const BigInteger& first, const int32_t& second);

BigInteger operator*(const BigInteger& first, const BigInteger& second);
BigInteger operator*(const int32_t& first, const BigInteger& second);
BigInteger operator*(const BigInteger& first, const int32_t& second);

BigInteger operator/(const BigInteger& first, const BigInteger& second);
BigInteger operator/(const int32_t& first, const BigInteger& second);
BigInteger operator/(const BigInteger& first, const int32_t& second);
std::istream& operator>>(std::istream& in, BigInteger &value);
std::ostream& operator<<(std::ostream& out, const BigInteger &value);
bool operator==(const BigInteger& first, const BigInteger& second);

bool operator<(const BigInteger& first, const BigInteger& second);

bool operator > (const BigInteger& first, const BigInteger& second);

bool operator<=(const BigInteger& first, const BigInteger& second);

bool operator >= (const BigInteger& first, const BigInteger& second);

bool operator!=(const BigInteger& first, const BigInteger& second);

class BigInteger {
private:
    std::vector<unsigned long long int> number; //Просто массив с нашими числами

    static const unsigned long long base = 1000000000;//основание нашей системы
    //выбор его не случаен, проще работать с такой базой, квадрат которой будет вмещаться
    //в тип, а именно в умножении не придется переносить в следующие разряды при умножении
    bool positive;
    //хранит знак

    //удаление не значимых нулей
    void RemoveZeros (){
        size_t pointer = number.size() - 1;
        while ((number[pointer] == 0) && pointer > 0) {
            --pointer;
            number.resize(number.size() - 1);
        }
    }

    //делает из строки число(используется в создании числа из строки)
    unsigned long long int MakeNumber(const std::string& str){
        unsigned long long int answer = 0;
        if (str[0] == '-') {
            return MakeNumber({&str[1], &str[str.size() - 1]});
        }
        for(size_t i = 0; i < str.size(); ++i) {
            answer = answer * 10 + (str[i] - '0');
        }
        return answer;
    }
    //делает запись корректной(убирает ведущие нули и приводит все числа к виду меньше базы
    void MakeRigth () {
        for (size_t i = 0; i < number.size(); ++i) {
            if (base <= number[i]) {
                if (i == number.size() - 1) {
                    number.push_back(number[i]/base);
                } else {
                    number[i + 1] += number[i]/base;
                }
                number[i] %= base;
            }
        }
        if (base < number[number.size() - 1]) {
            number.push_back(number[number.size() - 1]/base);
            number[number.size() - 2] %= base;
        }
        RemoveZeros();
    }

    //простое умножение для Карацубы
    static std::vector<unsigned long long int> SimpleMultiplication(
            const std::vector<unsigned long long int> &first,
            const std::vector<unsigned long long int> &second) {
        std::vector<unsigned long long int> result(first.size() * 2);
        for (size_t i = 0; i < first.size(); ++i) {
            for (size_t j = 0; j < second.size(); ++j) {
                result[i + j] += first[i]*second[j];
            }
        }
        return result;
    }

    /*
     * Умножение Карацубы
     * Алгоритм основан на умножении (A0*база + A1)(B0*база + B1)
     * при раскрытии скобок мы получим умножения вида
     * (A0+A1)(B0+B1), A0*B1 и A1*B0
     * A * B = A0 * B0 + (( A0 + A1 )*( B0 + B1 ) — A0 * B0 — A1 * B1 ) * BASE^m + A1 * B1 * BASE2 ^m
     * Асимптотика O(n^log(3)) логарифм берется по основанию 2, поэтому меньше чем n^2
     */
    static std::vector<unsigned long long int> KaratsubaMultiplication (
            const std::vector<unsigned long long int>& first,
            const std::vector<unsigned long long int>& second
    ) {
        size_t length = first.size();
        std::vector<unsigned long long int> result(2 * length);
        if (length <= 5) {
            return SimpleMultiplication(first, second);
        }
        size_t partition = length / 2;
        std::vector<unsigned long long int> FirstRigthPart, FirstLeftPart, SecondRightPart, SecondLeftPart;
        //A0,A1,B0,B1
        std::vector<unsigned long long int> SumOfFirstParts(partition), SumOfSecondParts(partition);
        // суммы этих частей
        for (size_t i = 0; i < partition; ++i) {
            FirstRigthPart.push_back(first[i]);
            FirstLeftPart.push_back(first[i + partition]);
            SecondRightPart.push_back(second[i]);
            SecondLeftPart.push_back(second[i + partition]);
        }
        std::vector<unsigned long long int> FirstMult = KaratsubaMultiplication(FirstLeftPart, SecondLeftPart);
        std::vector<unsigned long long int> SecondMult = KaratsubaMultiplication(FirstRigthPart, SecondRightPart);
        for (size_t i = 0; i < partition; ++i) {
            SumOfFirstParts[i] = FirstLeftPart[i] + FirstRigthPart[i];
            SumOfSecondParts[i] = SecondLeftPart[i] + SecondRightPart[i];
        }
        std::vector<unsigned long long int> ThirdMult = KaratsubaMultiplication(SumOfFirstParts, SumOfSecondParts);
        for (size_t i = 0; i < length; ++i) {
            ThirdMult[i] -= SecondMult[i] + FirstMult[i];
        }
        for (size_t i = 0; i < length; ++i) {
            result[i] = SecondMult[i];
        }
        for (size_t i = length; i < 2 * length; ++i) {
            result[i] = FirstMult[i - length];
        }
        for (size_t i = partition; i < length + partition; ++i) {
            result[i] += ThirdMult[i - partition];
        }
        return result;
    }

    //сделать размеры степени двойки(упрощает лагоритм Карацубы, потому что деление на 2 всегда целое)
    void MakeEqualSize(std::vector<unsigned long long int> &second) {
        size_t length = second.size() < number.size() ? number.size() : second.size();

        while (length & (length - 1)) {
            ++length;
        }
        number.resize(length, 0);
        second.resize(length, 0);
    }

public:
    //конструктор по умолчанию. Делает 0
    BigInteger() {
        number.push_back(0);
        positive = true;
    }
    //для всех используемых типов существуют встроенные деструкторы
    ~BigInteger() = default;
    //коструктор из строки
    explicit BigInteger(const std::string& strNumber) {
        for (size_t i = 0; i < strNumber.size() ; ++i) {
            if ((strNumber[i] <= '0' || strNumber[i] >='9') && (strNumber[i] == '-' && i != 0)) {
                std::cout << "Incorrect way of building number. Nothing was done" << std::endl; //ассерты запрещены,
                                                                                                // поэтому вот
            }
        }
        positive = strNumber[0] != '-';
        number.resize((strNumber.size() - !positive)/9 + bool((strNumber.size()  - !positive) %9));
        if ((strNumber.size() - !positive) % 9) {
            for (size_t i = 0; i < number.size() - 1; ++i) {
                number[i] = MakeNumber({&strNumber[strNumber.size() - 9 - 9 * i],
                                        &strNumber[strNumber.size() - 9 * i]}); //делаем из кусочка строки число
            }
            if (strNumber.size() % 9) {
                number[number.size() - 1] = MakeNumber({&strNumber[0 + !positive],
                                                        &strNumber[strNumber.size() % 9]}); //делаем из кусочка строки число
            } else {
                number[number.size() - 1] = MakeNumber({&strNumber[0 + !positive],
                                                        &strNumber[!positive + (strNumber.size() - !positive) % 9]});
                //делаем из кусочка строки число
            }
        } else {
            for (size_t i = 0; i < number.size(); ++i) {
                number[i] = MakeNumber({&strNumber[strNumber.size() - 9 - 9 * i],
                                        &strNumber[strNumber.size() - 9 * i]});//делаем из кусочка строки число
            }
        }
        if (number.size() == 1 && number[0] == 0) {
            positive = true;
        }
        RemoveZeros();
    }

    //Конструтор из инта
    BigInteger(const int32_t& Number) {
        if (Number == -0) {
            number.push_back(0);
            positive = true;
            return;
        }
        positive = Number > 0;
        unsigned long long int Num = positive ? Number : -Number;
        while (Num >= base) {
            number.push_back(Num % base);
            Num /= base;
        }
        number.push_back(Num);
    }

    //копирование
    BigInteger(const BigInteger& Num) {
        number = Num.Number();
        positive = Num.is_positive();
    }

    //Если захотим еще и знак задать сам
    BigInteger(const BigInteger& Num, const bool& Positive) {
        number = Num.Number();
        positive = Positive;
    }

    //приводим число к строке
    std::string toString() const {
        if (*this == 0) {
            return "0";
        }
        std::string strNumber;
        if (!positive) {
            strNumber += "-";
        }
        strNumber += std::to_string(number[number.size() - 1]);
        for (size_t i = number.size() - 1; i > 0; --i) {
            if (base / 10 <= number[i - 1]) {
                strNumber += std::to_string(number[i - 1]);
            } else { //добавляем нулики
                std::string tmp = std::to_string(number[i - 1]);
                while (tmp.size() < 9) {
                    tmp = "0" + tmp;
                }
                strNumber += tmp;
            }
        }
        return strNumber;
    }

    //Просто посмотреть на наше содержание
    const std::vector<unsigned long long int>& Number() const {
        return number;
    }

    //Посмотреть на знак
    const bool& is_positive() const{
        return positive;
    }

    //Сравнение по Модулю
    bool HasBiggerAbsoluteValue(const BigInteger& CompareWith) const {
        const std::vector<unsigned long long int>& Number = CompareWith.Number();
        if (number.size() < Number.size())
            return false;
        if (number.size() > Number.size())
            return true;
        for (size_t i = number.size(); i > 0; --i) {
            if (number[i - 1] < Number[i - 1])
                return false;
            if (number[i - 1] > Number[i - 1])
                return true;
        }
        return false;
    }

    //Сравнение модулей на равенство
    bool HasEqualAbsoluteValue(const BigInteger& CompareWith) const {
        const std::vector<unsigned long long int>& Number = CompareWith.Number();
        if (number.size() < Number.size())
            return false;
        if (number.size() > Number.size())
            return false;
        for (size_t i = number.size(); i > 0; --i) {
            if (number[i - 1] < Number[i - 1])
                return false;
            if (number[i - 1] > Number[i - 1])
                return false;
        }
        return true;
    }

    //Унарный минус
    BigInteger operator-() const{
        if (number.size() == 1 && number[0] == 0) {
            return *this;
        }
        BigInteger tmp(*this, !positive);
        return tmp;
    }

    //Сумма
    BigInteger& operator+=(const BigInteger& second) {
        if (second == 0) {
            return *this;
        }
        bool sec_is_pos = second.is_positive();
        if (sec_is_pos != positive) { //если чсила разных знаков, можно просто вычесть противоположное
            return (*this -=(-(second)));
        } else {
            const std::vector<unsigned long long int>& Second = second.Number();
            if (Second.size() > number.size())
                number.resize(Second.size(), 0);
            for (size_t i = 0; i < Second.size(); ++i) {
                number[i] += Second[i];
                if (number[i] >= base) {
                    if (i == number.size() - 1) {
                        number.push_back(1);
                    } else {
                        number[i+1] += 1;
                    }
                    number[i] -= base;
                }
            }
            return *this;
        }
    }
    BigInteger& operator+=(const int32_t& Number) {
        return (*this += BigInteger(Number));
    }

    BigInteger& operator-=(const BigInteger& second) {
        if (*this == second) { //если числа равны - ответ 0
            number.clear();
            number.push_back(0);
            positive = true;
            return *this;
        }
        bool sec_is_pos = second.is_positive();
        if (sec_is_pos != positive) {
            return (*this +=(-(second))); // если число другого знака, то достаточно прибавить противоположное
        } else {
            const std::vector<unsigned long long int>& Second = second.Number();
            if (HasBiggerAbsoluteValue(second)) { //Если больше то просто вычитаем, причем мы гарантировано найдем
                for (size_t i = 0; i < Second.size(); ++i) {//откуда занять, когда необходимо
                    if (number[i] >= Second[i]) {
                        number[i] -= Second[i];
                    } else {
                        number[i] += base - Second[i];
                        size_t FindToDecrease = i + 1;
                        while (number[FindToDecrease] == 0) {
                            number[FindToDecrease] = base - 1;
                            ++FindToDecrease;
                        }
                        --number[FindToDecrease];
                    }
                }
            } else { //Если меньше, вычитаем из второго первое и меняем знак. Всегда найдем во втором откуда занять
                positive = !positive; //так как второе больще
                number.resize(Second.size());
                bool GetNumberFromNext = false;
                for (size_t i = 0; i < number.size();) {
                    if (number[i] <= Second[i] - GetNumberFromNext) {
                        number[i] = Second[i] - number[i] - GetNumberFromNext;
                        GetNumberFromNext = false;
                        ++i;
                    } else {
                        number[i] = base + Second[i] - number[i] - GetNumberFromNext;
                        ++i;
                        while (Second[i] == 0) {
                            number[i] = base - 1 - number[i];
                            ++i;
                        }
                        GetNumberFromNext = true;
                    }
                }
            }
            MakeRigth();
            return *this;
        }
    }
    BigInteger& operator-=(const int32_t& Number) {
        return (*this -= BigInteger(Number));
    }

    // Просто умножение Карацубой
    BigInteger& operator*=(const BigInteger& second) {
        if ((second == 0) || (*this == 0)) {
            number.clear();
            number.push_back(0);
            positive = true;
            return *this;
        }
        std::vector<unsigned long long int> Num = second.Number();
        MakeEqualSize(Num);
        number = KaratsubaMultiplication(number, Num);
        positive = (positive && second.is_positive()) || (!positive && !second.is_positive());
        MakeRigth();
        return *this;
    }

    BigInteger& operator*=(const int32_t& second) {
        if (second < 0)
            positive = !positive;
        long long int Num = positive ? second : -second;
        for (unsigned long long & i : number) {
            i *= Num;
        }
        MakeRigth();
        return *this;
    }

    /*
     * При делении очевидно, что ответ не превосходит само число и больше 1
     * Сделаем бин.поиск по этому промежутку
     * Асимптотика O(log(*this))
     * пример 10 в сотой
     * получим 100log10
     * Для деления в столбик будет работать
     *  (10^100/10^9)log(10^9) > 100log10
     */
    BigInteger& operator/=(const BigInteger& second) {
        if (second.Number().size() == 1 && second.Number()[0] == 0) {
            std::cout <<"Division by zero was called. Nothing Changed" << std::endl;
            return *this;
        }
        if (second == 1) {
            return *this/=1;
        }
        if (!HasBiggerAbsoluteValue(second)) {
            unsigned long long put = HasEqualAbsoluteValue(second) ? 1 : 0;
            number.clear();
            number.push_back(put);
            positive = true;
        } else {
            if (!second.is_positive()) {
                positive = !positive;
                return (*this /= (-second));
            }
            bool Positive_answer = (positive && second.is_positive());
            positive = true;
            BigInteger low(0);
            BigInteger upper(*this);
            BigInteger middle;
            while (low <= upper) {
                middle = (low + upper) / 2;
                if (middle * second < *this) {
                    if ((middle+1) * second > *this) {
                        *this = middle;
                        MakeRigth();
                        positive = Positive_answer;
                        return *this;
                    }
                    low = middle;
                } else if (middle * second > *this) {
                    if ((middle-1) * second < *this) {
                        *this = middle - 1;
                        MakeRigth();
                        positive = Positive_answer;
                        return *this;
                    }
                    upper = middle;
                } else {
                    *this = middle;
                    MakeRigth();
                    positive = Positive_answer;
                    return *this;
                }
            }
        }
        return *this;
    }

    BigInteger& operator/=(const int32_t& second) {
        for (size_t i = number.size(); i > 0; --i) {
            if (i > 1)
                number[i-2] += (number[i-1] % second) * base;
            number[i - 1] /= second;
        }
        return (*this);
    }

    BigInteger& operator%=(const BigInteger& second) {
        return (*this -= ((BigInteger(*this) /= second) *= second));
    }
    BigInteger& operator%=(const int32_t& second) {
        return (*this -= ((BigInteger(*this) /= second) *= second));
    }

    //Префиксный инкремент
    BigInteger& operator++(){
        if (positive) {
            number[0] += 1;
            size_t j = 0;
            while (number[j] >= base) {
                if (j < number.size() - 1) {
                    ++number[j + 1];
                    number[j] %= base;
                    ++j;
                } else {
                    number.push_back(1);
                    ++j;
                }
            }
            return *this;
        } else {
            if (*this != 0) {
                size_t j = 0;
                while (number[j] == 0) {
                    number[j] = base - 1;
                    ++j;
                }
                number[j] -= 1;
            } else {
                positive = true;
                number[0] = 1;
            }
            if (number.size() == 1 && number[0] == 0) {
                positive = true;
            }
            return *this;
        }
    }

    //Префиксный декремент
    BigInteger& operator--(){
        if (!positive) {
            number[0] += 1;
            size_t j = 0;
            while (number[j] >= base) {
                if (j < number.size() - 1) {
                    ++number[j + 1];
                    number[j] %= base;
                    ++j;
                } else {
                    number.push_back(1);
                    ++j;
                }
            }
            return *this;
        } else {
            if (*this != 0) {
                size_t j = 0;
                while (number[j] == 0) {
                    number[j] = base - 1;
                    ++j;
                }
                number[j] -= 1;
            } else {
                positive = false;
                number[0] = 1;
            }
            return *this;
        }
    }

    //Постфексный инкремент
    BigInteger operator++(int) {
        BigInteger tmp(*this);
        ++*this;
        return tmp;
    }

    //Постфиксный декремент
    BigInteger operator--(int) {
        BigInteger tmp(*this);
        --*this;
        return tmp;
    }

    //Перевод в бул
    explicit operator bool() const{
        return (number.size() > 1 || number[0] != 0);
    }

    friend std::istream& operator>>(std::istream& in, BigInteger &value);
};

BigInteger operator+(const BigInteger& first, const BigInteger& second) {
    return (BigInteger (first) += second);
}
BigInteger operator+(const int32_t& first, const BigInteger& second) {
    return (BigInteger(first) += second);
}
BigInteger operator+(const BigInteger& first, const int32_t& second) {
    return (BigInteger(first) += second);
}
BigInteger operator-(const BigInteger& first, const BigInteger& second) {
    return (BigInteger(first) -= second);
}
BigInteger operator-(const int32_t& first, const BigInteger& second) {
    return (BigInteger(first) -= second);
}
BigInteger operator-(const BigInteger& first, const int32_t& second) {
    return (BigInteger(first) -= second);
}
BigInteger operator*(const BigInteger& first, const BigInteger& second) {
    return (BigInteger(first) *= second);
}
BigInteger operator*(const int32_t& first, const BigInteger& second) {
    return (BigInteger(first) *= second);
}
BigInteger operator*(const BigInteger& first, const int32_t& second) {
    return (BigInteger(first) *= second);
}

BigInteger operator/(const BigInteger& first, const BigInteger& second) {
    return (BigInteger(first) /= second);
}
BigInteger operator/(const int32_t& first, const BigInteger& second) {
    return (BigInteger(first) /= second);
}
BigInteger operator/(const BigInteger& first, const int32_t& second) {
    return (BigInteger(first) /= second);
}
BigInteger operator%(const BigInteger& first, const BigInteger& second) {
    return (BigInteger(first) %= second);
}
BigInteger operator%(const int32_t& first, const BigInteger& second) {
    return (BigInteger(first) %= second);
}
BigInteger operator%(const BigInteger& first, const int32_t& second) {
    return (BigInteger(first) %= second);
}

bool operator==(const BigInteger& first, const BigInteger& second) {
    const std::vector<unsigned long long int>& First = first.Number();
    const std::vector<unsigned long long int>& Second = second.Number();
    bool First_sign = first.is_positive();
    bool Second_sign = second.is_positive();
    if (First_sign != Second_sign)
        return false;
    if (First.size() == Second.size()) {
        for(size_t i = 0; i < First.size(); ++i) {
            if (First[i] != Second[i]) {
                return false;
            }
        }
        return true;
    }
    return false;
}
bool operator == (const BigInteger& first, const int & second) {
    return first == BigInteger(second);
}
bool operator!=(const BigInteger& first, const BigInteger& second) {
    return !(first == second);
}

bool operator<(const BigInteger& first, const BigInteger& second) {
    bool First_sign = first.is_positive();
    bool Second_sign = second.is_positive();
    if (First_sign && !Second_sign)
        return false;
    if (!First_sign && Second_sign)
        return true;
    return !first.HasBiggerAbsoluteValue(second) && !first.HasEqualAbsoluteValue(second);
}

bool operator>(const BigInteger& first, const BigInteger& second) {
    return !((first < second) || (first == second));
}

bool operator<=(const BigInteger& first, const BigInteger& second) {
    return !(first > second);
}

bool operator>=(const BigInteger& first, const BigInteger& second) {
    return !(first < second);
}

std::istream& operator>>(std::istream& in, BigInteger &value) {
    std::string strNum;
    in >> strNum;
    value = BigInteger(strNum);
    return in;
}

std::ostream& operator<<(std::ostream& out, const BigInteger &value) {
    out << value.toString();
    return out;
}

class Rational {
private:
    BigInteger numerator, denumerator;

    //Делает Дробь правильного вида, то есть сокращает на НОД и оставляет знаменатель положительным
    void MakeRight() {
        if (denumerator < 0) {
            numerator = -numerator;
            denumerator = -denumerator;
        }
        BigInteger nod = NOD(numerator, denumerator);
        numerator /= nod;
        denumerator /= nod;
    }

    //НОД методом Евклида на остатках
    static BigInteger NOD(BigInteger first, BigInteger second) {
        if (!first.is_positive()) {
            first = -first;
        }
        if (!second.is_positive()) {
            second = -second;
        }
        while (first && second) {
            first > second ? first %= second : second %= first;
        }
        return first += second;
    }
public:
    //Конструктор по умолчанию
    Rational() {
        numerator = 0;
        denumerator = 1;
    }

    //Деструктор сработает сам, так как есть деструктор у составных частей

    //Конструктор от двух Больших чисел
    Rational(const BigInteger& Num, const BigInteger& Den) {
        if (Den == 0) {
            std::cout<<"Division by zero was called. Rational wasn't made"<<std::endl;
            return;
        }
        numerator = Num;
        denumerator = Den;
        MakeRight();
    }

    //Конструктор от одного большого числа
    Rational(const BigInteger& Num) {
        numerator = Num;
        denumerator = 1;
    }

    //Конструктор от интов
    Rational(const int32_t & Num, const int32_t & Den) {
        if (Den == 0) {
            std::cout<<"Division by zero was called. Rational wasn't made"<<std::endl;
            return;
        }
        numerator = Num;
        denumerator = Den;
        MakeRight();
    }

    //Конструктор от инта
    Rational(const int32_t & Num) {
        numerator = Num;
        denumerator = 1;
    }

    //Конструктор копирования
    Rational(const Rational& other) {
        numerator = other.numerator;
        denumerator = other.denumerator;
        MakeRight();
    }

    Rational& operator=(const int32_t& Other) {
        numerator = Other;
        denumerator = 1;
        return *this;
    }

    //Перевод в строку
    std::string toString() const {
        std::string strNumber;
        if (denumerator == 1) {
            strNumber = numerator.toString();
            //Если знаменатель 1, то его выводить не надо
        } else {
            strNumber = numerator.toString() + "/" + denumerator.toString();
        }
        return strNumber;
    }

    //Можно посмотреть на числитель
    const BigInteger& Numerator() const {
        return numerator;
    }

    //Можно посмотреть на знаменательё
    const BigInteger& Denumerator() const {
        return denumerator;
    }
    //Унарный минус
    Rational operator-() const{
        Rational tmp(-this->numerator, this->denumerator);
        return tmp;
    }

    //Просто сложение
    Rational& operator+=(const Rational& second) {
        numerator *= second.denumerator;
        numerator += second.numerator * denumerator;
        denumerator *= second.denumerator;
        MakeRight();
        return *this;
    }

    Rational& operator+=(const BigInteger& Number) {
        return (*this += Rational(Number));
    }

    Rational& operator+=(const int32_t& Number) {
        return (*this += Rational(Number));
    }

    //Минус
    Rational& operator-=(const Rational& second) {
        numerator *= second.denumerator;
        numerator -= second.numerator * denumerator;
        denumerator *= second.denumerator;
        MakeRight();
        return *this;
    }

    Rational& operator-=(const BigInteger& Number) {
        return (*this -= Rational(Number));
    }

    Rational& operator-=(const int32_t& Number) {
        return (*this -= Rational(Number));
    }

    Rational& operator*=(const Rational& second) {
        numerator *= second.numerator;
        denumerator *= second.denumerator;
        MakeRight();
        return *this;
    }

    Rational& operator*=(const BigInteger& second) {
        numerator *= second;
        MakeRight();
        return *this;
    }

    Rational& operator*=(const int32_t& second) {
        numerator *= second;
        MakeRight();
        return *this;
    }

    Rational& operator/=(const Rational& second) {
        if (second.numerator.toString() == "0") {
            std::cout << "Division by zero. Nothig happend";
            return *this;
        }
        numerator *= second.denumerator;
        denumerator *= second.numerator;
        MakeRight();
        return *this;
    }

    Rational& operator/=(const BigInteger& second) {
        if (second.toString() == "0") {
            std::cout << "Division by zero. Nothig happend";
            return *this;
        }
        denumerator *= second;
        MakeRight();
        return *this;
    }

    Rational& operator/=(const int32_t& second) {
        if (second == 0) {
            std::cout << "Division by zero. Nothing happend" <<std::endl;
            return *this;
        }
        denumerator *= second;
        MakeRight();
        return *this;
    }

    //Представить с точностью
    //Целая часть плюс умножаем на 10 и делим в столбик для каждой цифры из точности
    std::string asDecimal(size_t precision = 0) const {
        std::string answer;
        Rational tmp(*this);
        tmp.MakeRight();
        if (tmp.numerator < 0) {
            answer+="-";
            tmp.numerator = - tmp.numerator;
        }
        answer += (tmp.numerator / tmp.denumerator).toString();
        tmp.numerator %= tmp.denumerator;
        if (precision > 0) {
            answer += ".";
            for (size_t i = 0; i < precision; ++i) {
                std::string SmallAnswer;
                tmp.numerator *= 10;
                if ((i == precision - 1) && ((10 * (tmp.numerator % tmp.denumerator))/ tmp.denumerator) >= 5) {
                    SmallAnswer += (tmp.numerator / tmp.denumerator + 1).toString();
                } else {
                    SmallAnswer += (tmp.numerator / tmp.denumerator).toString();
                }
                answer += SmallAnswer;
                if ((tmp.numerator / tmp.denumerator).toString() != "0") {
                    tmp.numerator %= tmp.denumerator;
                }
            }
        }
        return answer;
    }

    //Преобразование в дабл
    explicit operator double() const{
        return (std::stod(this->asDecimal(18)));
    }

};
Rational operator+(const Rational& first, const Rational& second) {
    return Rational(first) += second;
}
Rational operator+(const BigInteger& first, const Rational& second) {
    return Rational(first) += second;
}
Rational operator+(const Rational& first, const BigInteger& second) {
    return Rational(first) += second;
}
Rational operator+(const int32_t& first, const Rational& second) {
    return Rational(first) += second;
}
Rational operator+(const Rational& first, const int32_t& second) {
    return Rational(first) += second;
}
Rational operator-(const Rational& first, const Rational& second) {
    return Rational(first) -= second;
}
Rational operator-(const int32_t& first, const Rational& second) {
    return Rational(first) -= second;
}
Rational operator-(const Rational& first, const int32_t& second) {
    return Rational(first) -= second;
}
Rational operator-(const BigInteger& first, const Rational& second) {
    return Rational(first) -= second;
}
Rational operator-(const Rational& first, const BigInteger& second) {
    return Rational(first) -= second;
}


Rational operator*(const Rational& first, const Rational& second) {
    return Rational(first) *= second;
}
Rational operator*(const int32_t& first, const Rational& second) {
    return Rational(first) *= second;
}
Rational operator*(const Rational& first, const int32_t& second) {
    return Rational(first) *= second;
}
Rational operator*(const BigInteger& first, const Rational& second) {
    return Rational(first) *= second;
}
Rational operator*(const Rational& first, const BigInteger& second) {
    return Rational(first) *= second;
}

Rational operator/(const Rational& first, const Rational& second) {
    return Rational(first) /= second;
}
Rational operator/(const int32_t& first, const Rational& second) {
    return Rational(first) /= second;
}
Rational operator/(const Rational& first, const int32_t& second) {
    return Rational(first) /= second;
}
Rational operator/(const BigInteger& first, const Rational& second) {
    return Rational(first) /= second;
}
Rational operator/(const Rational& first, const BigInteger& second) {
    return Rational(first) /= second;
}

bool operator <(const Rational& first, const Rational& second){
    return (first.Numerator() * second.Denumerator() < first.Denumerator() * second.Numerator());
}
bool operator >(const Rational& first, const Rational& second) {
    return (first.Numerator() * second.Denumerator() > first.Denumerator() * second.Numerator());
}
bool operator ==(const Rational& first, const Rational& second) {
    return (first.Numerator() * second.Denumerator() == first.Denumerator() * second.Numerator());
}
bool operator <=(const Rational& first, const Rational& second) {
    return (first.Numerator() * second.Denumerator() <= first.Denumerator() * second.Numerator());
}
bool operator >=(const Rational& first, const Rational& second) {
    return (first.Numerator() * second.Denumerator() >= first.Denumerator() * second.Numerator());
}
bool operator !=(const Rational& first, const Rational& second){
    return (first.Numerator() * second.Denumerator() != first.Denumerator() * second.Numerator());
}

