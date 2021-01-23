/**
 * Задача: реализовать прямое и обратное FFT и применить к исследованию
 * wav файла
 * результаты в файле result и можно прослушать сами полученные преобразования
 *
 */
#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>
#include <complex>
#include <cstring>
/*
 * Класс ФФТ
 * Хранит:
 *     -коэффициенты многочлена, который является значением числа
 *     -коэффициенты многочлена, который является значением многочлена в точках e^{i*2*pi*k/n}
 *
 * Методы:
 *     -установка кожффициентов многочлена и преобразованного(если такое известно кому-то)
 *     -прямое и обратное преобразование ффт
 *
 * (Для вспомогательных функций и деталей реализации см. комментарии к коду)
 */
class FFT {
public:
    FFT(std::vector<std::complex<double>> coef, uint32_t real_size = 0, bool rev = false);

    void fft();

    void reversed_fft();

    const std::vector<std::complex<double>> &coef() const;

    const std::vector<std::complex<double>> &harm() const;

    const size_t &real_size() const;

    void ignore_harmonics(uint32_t percent);

private:
    /*
     * Находит степень двойки. 2^k > n
     * необходимо для выдиления массива для перевернутых чисел
     * (коэффициенты в переставленном порядке)
     */
    void find_degree();

    void set_rev_num();

    void swap_coefficients();

    std::vector<std::complex<double>> coefficients;
    std::vector<std::complex<double>> harmonics;

    size_t length = 0;     //Длина массива как степень двойки
    size_t real_length = 0;// настоящая длина - количество коэффициетнов
    size_t degree = 0;     // Степень двойки для длины

    std::vector<uint32_t> reversed_numbers;
};

void FFT::ignore_harmonics(uint32_t percent) {
    uint32_t last_zero = (100 - percent)*harmonics.size()/100;
    for (size_t i = harmonics.size() - 1; i > last_zero; --i) {
        harmonics[i] = 0;
    }
}

const size_t &FFT::real_size() const {
    return real_length;
}
/*
 * два вида построения. Прямое и обратное. Для прямого мы вычисляем размер. Для обратного получаем
 */
FFT::FFT(std::vector<std::complex<double>> coef, uint32_t real_size, bool rev) {
    if (rev) {
        harmonics = std::move(coef);
        real_length = real_size;
        length = harmonics.size();
        find_degree();
    } else {
        coefficients = std::move(coef);
        real_length = coefficients.size();
        find_degree();
        length = 1ull << degree;
        if (length > real_length) {
            coefficients.resize(length);
        }
    }
}

const std::vector<std::complex<double>> &FFT::harm() const {
    return harmonics;
}

const std::vector<std::complex<double>> &FFT::coef() const {
    return coefficients;
}

void FFT::find_degree() {
    while ((1ull << degree) < real_length) {
        ++degree;
    }
}

/*
 * Если расписать все математические преобразования,
 * то можно заметить, что мы на каждом шаге вычисления, то можно
 * заметить что позиции коэффициентов, участвущих в вычислении многочлена образуют
 * увеличивающиеся в бинарном виде числа, которые перевернуты
 * 000 100 010 110 001 101 011 111
 * Эта функция сопоставляет положения и коэффициенты
 */
void FFT::set_rev_num() {
    reversed_numbers.resize(length, 0);

    size_t highest_degree = 0;
    for (size_t i = 1; i < length; ++i) {
        if (!(i & (i - 1))) {
            ++highest_degree;
        }
        reversed_numbers[i] = reversed_numbers[i ^ (1ull << (highest_degree - 1))];
        reversed_numbers[i] |= (1ull << (degree - highest_degree));
    }
}

// Помещаем в найденные позиции соответствующие коэффициенты
void FFT::swap_coefficients() {
    harmonics.resize(1ull << degree);
    coefficients.resize(1ull << degree);
    for (size_t i = 0; i < length; ++i) {
        harmonics[i] = coefficients[reversed_numbers[i]];
    }
}

void FFT::fft() {
    double angle;
    std::complex<double> complex_root;
    std::complex<double> root_difference;
    std::complex<double> first_part;
    std::complex<double> second_part;

    set_rev_num();

    swap_coefficients();

    // По аналогии с мердж сортом можно избавиться от рекурсии
    // проводя действия с массивом, увеличиваю длину рассматриваемых
    // кусков.  Угол сдвига между корнями определяется как 2пи/длину.
    // Воспользуемся преобразованием бабочки для вычисления
    // первый многочлен + х * второй многочлен
    for (size_t len = 2; len <= length; len <<= 1ull) {
        angle = 2 * M_PI / len;
        root_difference = std::complex<double>(cos(angle), sin(angle));
        for (size_t i = 0; i < length; i += len) {
            complex_root = 1;
            for (size_t j = 0; j < len / 2; ++j) {
                first_part = harmonics[i + j];
                second_part = harmonics[i + j + len / 2] * complex_root;
                harmonics[i + j] = first_part + second_part;
                harmonics[i + j + len / 2] = first_part - second_part;
                complex_root *= root_difference;
            }
        }
    }
}
/*
 * Из математики легко получить, что обратное преобразование
 * это прямое, деленное на длину и затем перевернуть корни с 1 по n.
 * Это получается из свойств e^x так как теперь обход будет в другую сторону
 */
void FFT::reversed_fft() {
    coefficients = std::move(harmonics);
    fft();

    for (size_t i = 1; i < length; ++i) {
        auto tmp = coefficients[i];
        coefficients[i] = harmonics[length - i];
        coefficients[i] /= length;
        harmonics[length - i] = tmp;
    }

    auto tmp = coefficients[0];
    coefficients[0] = harmonics[0];
    coefficients[0] /= length;
    harmonics[0] = tmp;
}


std::vector<std::complex<double>> translate_from_char(const unsigned char *data, size_t size) {
    std::vector<std::complex<double>> result;
    result.reserve(size);
    for (size_t i = 0; i < size; ++i) {
        result.emplace_back(data[i]);
    }
    return result;
}

unsigned char *translate_to_char(const std::vector<std::complex<double>> &data) {
    auto *result = new unsigned char[data.size()];
    for (size_t i = 0; i < data.size(); ++i) {
        result[i] = int(data[i].real());
    }
    return result;
}

// Структура, описывающая заголовок WAV файла.
struct WAVHEADER {
    // WAV-формат начинается с RIFF-заголовка:

    // Содержит символы "RIFF" в ASCII кодировке
    // (0x52494646 в big-endian представлении)
    char chunkId[4];

    // 36 + subchunk2Size, или более точно:
    // 4 + (8 + subchunk1Size) + (8 + subchunk2Size)
    // Это оставшийся размер цепочки, начиная с этой позиции.
    // Иначе говоря, это размер файла - 8, то есть,
    // исключены поля chunkId и chunkSize.
    unsigned int chunkSize;

    // Содержит символы "WAVE"
    // (0x57415645 в big-endian представлении)
    char format[4];

    // Формат "WAVE" состоит из двух подцепочек: "fmt " и "data":
    // Подцепочка "fmt " описывает формат звуковых данных:

    // Содержит символы "fmt "
    // (0x666d7420 в big-endian представлении)
    char subchunk1Id[4];

    // 16 для формата PCM.
    // Это оставшийся размер подцепочки, начиная с этой позиции.
    unsigned int subchunk1Size;

    // Аудио формат, полный список можно получить здесь http://audiocoding.ru/wav_formats.txt
    // Для PCM = 1 (то есть, Линейное квантование).
    // Значения, отличающиеся от 1, обозначают некоторый формат сжатия.
    unsigned short audioFormat;

    // Количество каналов. Моно = 1, Стерео = 2 и т.д.
    unsigned short numChannels;

    // Частота дискретизации. 8000 Гц, 44100 Гц и т.д.
    unsigned int sampleRate;

    // sampleRate * numChannels * bitsPerSample/8
    unsigned int byteRate;

    // numChannels * bitsPerSample/8
    // Количество байт для одного сэмпла, включая все каналы.
    unsigned short blockAlign;

    // Так называемая "глубиная" или точность звучания. 8 бит, 16 бит и т.д.
    unsigned short bitsPerSample;

    // Подцепочка "data" содержит аудио-данные и их размер.

    // Содержит символы "data"
    // (0x64617461 в big-endian представлении)
    char subchunk2Id[4];

    // numSamples * numChannels * bitsPerSample/8
    // Количество байт в области данных.
    unsigned int subchunk2Size;

    // Далее следуют непосредственно Wav данные.
};

int main(int argc, char *argv[]) {

    char * file_name = argv[1];
    if (argc == 1) {
        std::cout << "No file entered\n";
    }
    FILE *file = fopen(file_name, "rb");
    if (!file) {
        std::cout << "Failed open file";
        return 0;
    }

    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, file);
/*
    // Выводим полученные данные
    std::cout << header.chunkId[0] << header.chunkId[1] << header.chunkId[2] << header.chunkId[3] << std::endl;
    printf("Chunk size: %d\n", header.chunkSize);
    std::cout << header.format[0] << header.format[1] << header.format[2] << header.format[3] << std::endl;
    std::cout << header.subchunk1Id[0] << header.subchunk1Id[1] << header.subchunk1Id[2] << header.subchunk1Id[3]
              << std::endl;
    printf("SubChunkId1: %d\n", header.subchunk1Size);
    printf("Audio format: %d\n", header.audioFormat);
    printf("Channels: %d\n", header.numChannels);
    printf("Sample rate: %d\n", header.sampleRate);
    printf("Bits per sample: %d\n", header.bitsPerSample);
    std::cout << header.subchunk2Id[0] << header.subchunk2Id[1] << header.subchunk2Id[2] << header.subchunk2Id[3]
              << std::endl;

    // Посчитаем длительность воспроизведения в секундах
    float fDurationSeconds =
            1.f * header.subchunk2Size / (header.bitsPerSample / 8) / header.numChannels / header.sampleRate;
    int iDurationMinutes = (int) std::floor(fDurationSeconds) / 60;
    fDurationSeconds = fDurationSeconds - (iDurationMinutes * 60);
    printf("Duration: %02d:%02.f\n", iDurationMinutes, fDurationSeconds);
*/
    auto *data = new unsigned char[header.subchunk2Size];
    fread(data, sizeof(unsigned char), header.subchunk2Size, file);

    std::cout << "Data is successfully loaded." << std::endl;

   // std::cout << data << '\n';

    FFT forward(translate_from_char(data, header.subchunk2Size));
    forward.fft();

    delete[] data;
    fclose(file);

    char new_file_name[4096];
    char template_for_wav[4096];
    strncpy(template_for_wav, file_name, strlen(file_name) - 4); // без .wav
    strcat(template_for_wav, "remake_%d.wav");

    for (size_t i = 0; i < 100; i += 10) {
        forward.ignore_harmonics(i);
        FFT backward(forward.harm(), forward.real_size(), true);
        backward.reversed_fft();

        auto *new_data = translate_to_char(backward.coef());

        sprintf(new_file_name, template_for_wav, i);
        FILE *new_file = fopen(new_file_name, "wb");
        fwrite(&header, sizeof(WAVHEADER), 1, new_file);
        fwrite(new_data, header.subchunk2Size, 1, new_file);

        std::cout << "Data is successfully uploaded." << std::endl;
        delete[] new_data;
        fclose(new_file);
    }

/*
 *  Небольшая зона для тестирования FFT
    FFT fft({1,2,3, 4, 5, 123, 11, 312, 0, 21});

    fft.fft();
    for (auto i : fft.coef()) {
        std::cout << i << '\n';
    }

    std::cout << "\n --------------\n";
    for (auto i : fft.harm()) {
        std::cout << i << '\n';
    }
    std::cout << "\n --------------\n";
    FFT rev(fft.harm(), fft.real_size(), true);
    rev.reversed_fft();
    for (auto i : rev.harm()) {
        std::cout << i << '\n';
    }
    std::cout << "\n --------------\n";
    for (size_t i = 0; i < rev.real_size(); ++i) {
        std::cout << round(rev.coef()[i].real()) << '\n';
    }*/
    return 0;
}

