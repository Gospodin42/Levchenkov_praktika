#include <iostream>
#include <fstream>

using namespace std;

// Максимальное количество самолётов
const int MAX_PLANES = 100;

// Максимальная длина строковых полей
const int MAX_STR_LEN = 50;

// Структура - самолёт
struct Airplane {
    char time[MAX_STR_LEN];         // Время входа формата ЧЧ:ММ
    char model[MAX_STR_LEN];        // Марка ЛА
    char tailNumber[MAX_STR_LEN];    // Бортовой номер
    int  flightNumber;              // Номер рейса

};

// Прототипы функций

bool isTimeLess(const char* timeA, const char* timeB);
void indexSortByTime(const Airplane planes[], int indices[], int count);
int readFromFile(const char* filename, Airplane planes[], int maxCount);
void printSeparator();
void printPlane(const Airplane& plane);
void printAllPlanes(const Airplane planes[], int count);
void printByIndices(const Airplane planes[], const int indices[], int count);

// Функция сравнения строк
int stringCompare(const char* a, const char* b){
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0'){
        if (a[i] != b[i]){
            return a[i] - b[i];
        }// if
        i++;
    }// while
    return a[i] - b[i];
}

// Функция для копирования С-строки
void stringCopy(
    char* dest,     // Строка, в которую копируем
    const char* src // Исходная строка
){
    int i = 0;
    while (src[i] != '\0'){
        dest[i] = src[i];
        i++;
    }// while
    dest[i] = '\0';
}

// Функция пропуска пробелов в строке
const char* skipSpaces(const char* str){
    while (*str == ' ' || *str == '\t'){
        str++;
    }//while
    return str;
}

// Парсинг строки 
bool parseLine(const char* line, Airplane& plane){
    const char* ptr = line;

    // 1. Считывание времени (до пробела)
    int i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1){
        plane.time[i] = *ptr;
        i++;
        ptr++;
    }// while
    plane.time[i] = '\0';
    if (i == 0) return false;

    // Пропуск пробелов
    ptr = skipSpaces(ptr);

    // 2. Считывание марки ЛА
    i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1){
        plane.model[i] = *ptr;
        i++;
        ptr++;
    }//while
    plane.model[i] = '\0';
    if (i == 0) return false;

    // Пропуск пробелов
    ptr = skipSpaces(ptr);

    // 3. Считывание бортового номера
    i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1){
        plane.tailNumber[i] = *ptr;
        i++;
        ptr++;
    }
    plane.tailNumber[i] = '\0';
    if (i == 0) return false;

    // Пропуск пробелов
    ptr = skipSpaces(ptr);

    // 4. Считывание номера рейса (как целое число)
    int flightNum = 0;
    while (*ptr >= '0' && *ptr <= '9'){
        flightNum = flightNum * 10 + (*ptr - '0');
        ptr++;
    }// while
    plane.flightNumber = flightNum;

    return true;
}// parseLine

// Функция чтения данных из файла
int readFromFile(const char* filename, Airplane planes[], int maxCount){
    ifstream file(filename);

    if (!file.is_open()){
        cout << "Ошибка: не удалось открыть файл" << filename << endl;
        return 0;
    }// if

    int count = 0;
    char line[256];

    while (file.getline(line,256) && count < maxCount){
        // Пропускаем пустые строки
        if (line[0] == '\0'){
            continue;
        }// if

        Airplane temp;
        if (parseLine(line, temp)){
            planes[count] = temp;
            count++;
        } else{
            cout << "Ошибка формата в строке: " << line << endl;
        }// else
    }// while

    file.close();
    return count;

}//readFromFile

// Функция вывода разделителя
void printSeparator() {
    cout << "------------------------------------------------------------" << endl;
}

// Функция вывода одного самолёта
void printPlane(const Airplane& plane) {
    cout << plane.time << " | " 
         << plane.model << " | " 
         << plane.tailNumber << " | " 
         << plane.flightNumber << endl;
}

// Функция вывода всех самолётов
void printAllPlanes(const Airplane planes[], int count) {
    printSeparator();
    cout << "Время | Марка ЛА | Бортовой № | Рейс" << endl;
    printSeparator();
    for (int i = 0; i < count; i++) {
        printPlane(planes[i]);
    }
    printSeparator();
    cout << endl;
}

// Функция вывода с использованием индексного массива
void printByIndices(const Airplane planes[], const int indices[], int count) {
    printSeparator();
    cout << "Время | Марка ЛА | Бортовой № | Рейс" << endl;
    printSeparator();
    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        printPlane(planes[idx]);
    }
    printSeparator();
    cout << endl;
}

// Функция сравнения времени (возвращает true, если время a < b)

bool isTimeLess(const char* timeA, const char* timeB){
    // Сравниваем часы (первые 2 символа)
    int hourA = (timeA[0] - '0') * 10 + (timeA[1] - '0');
    int hourB = (timeB[0] - '0') * 10 + (timeB[1] - '0');

    if (hourA != hourB){
        return hourA < hourB;
    }

    // Если часы равны, сравниваем минуты (3-4 символы)
    int minA = (timeA[3] - '0') * 10 + (timeA[4] - '0');
    int minB = (timeB[3] - '0') * 10 + (timeB[4] - '0');

    return minA < minB;

} //isTimeLess


// Индексная сортировка методом выбора по времени
void indexSortByTime(const Airplane planes[], int indices[], int count){
    for (int i = 0; i < count - 1; i++){
        int minIndex = i;

        // Ищем индекс с наименьшим временем
        for (int j = i + 1; j < count; j++){
            if (isTimeLess(planes[indices[j]].time, planes[indices[minIndex]].time)){
                minIndex = j;
            }
        }

        // Меняем местами indices[i] и indices[minIndex]
        if (minIndex != i){
            int temp = indices[i];
            indices[i] = indices[minIndex];
            indices[minIndex] = temp;
        }

    }// for

}// indexSortByTime




int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    //setlocale(LC_ALL, "RUSSIAN");

    // Массив структур
    Airplane planes[MAX_PLANES];

    // Индексный массив 
    int indices[MAX_PLANES];

    // Чтение из файла
    int planeCount = readFromFile("input.txt", planes, MAX_PLANES);

    if (planeCount == 0){
        cout << "Нет данных для обработки. Программа завершена." << endl;
        return 1;
    }// if

    cout << "Прочитано самолётов: " << planeCount << endl;

    // Инициализация индексного массива
    for (int i = 0; i < planeCount; i++){
        indices[i] = i;
    }

    // Вывод исходных данных
    cout << "\n=== Исходные данные (порядок как в файле) ===" << endl;
    printAllPlanes(planes, planeCount);

    // Сортировка методом выбора по времени
    indexSortByTime(planes, indices, planeCount);

    // Вывод отсортированных данных
    cout << "\n ==== Отсортированные данные по времени входа в зону ===" << endl;
    printByIndices(planes, indices, planeCount);

    return 0;
}// main




