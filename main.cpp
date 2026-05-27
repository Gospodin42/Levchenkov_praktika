/************************************************************************************************
 *                      Ознакомительная практика                                                *
 *----------------------------------------------------------------------------------------------*
 * Project Type  : Win32 Console Application                                                    *
 * Project Name  : C:\Users\andre\OneDrive\Documents\CPP МАИ\Levchenkov_praktika\main.cpp       *
 * File Name     : main.cpp                                                                     *
 * Programmer(s) : Левченков А.                                                                 *
 * Modifyed By   :                                                                              *
 * Created       : 09/05/26                                                                     *
 * Last Revision : 27/05/26                                                                     *
 * Comment(s)    : Программа, сортирующая записи с использованием индексной сортировки методом  *
 *                 выбора по времени входа в зону; результаты печатаются в виде таблицы         *
 ************************************************************************************************/


#include <iostream>
#include <fstream>

using namespace std;

// Максимальная длина строковых полей при чтении
const int MAX_STR_LEN = 100;

const char* CURRENT_INPUT_FILE = "tests\\positive\\test1.txt";    // Менять тесты вруяную

// Структура - самолёт
struct Airplane {
    char* time;         // Время входа формата ЧЧ:ММ
    char* model;        // Марка ЛА
    char* tailNumber;   // Бортовой номер
    int flightNumber;   // Номер рейса
};

//                                           === ПРОТОТИПЫ ФУНКЦИЙ ===

bool isValidTime(           // Проверяет корректность ввода времени
    const char* timeStr     // Строка времени
);

bool isValidTailNumber(     // Проверяет корректность бортового номера (Б-хххх)
    const char* tailNumber  // Строка бортового номера
);

bool isValidFlightNumber(   // Проверяет корректность номера рейса
    const char* flightStr   // Строка номера рейса
);

bool isTimeLess(            // Сравнивает две строки времени (ЧЧ:ММ)
    const char* timeA,      // Строка времени A
    const char* timeB);     // Строка времени B

char* allocateAndCopy(       // Выделяет динамическую память под строку и копирует содержимое
    const char* src         // Исходная строка
);

void freePlane(             // Освобождает память структуры Airplane
    Airplane* plane         // Указатель на структуру
);

bool parseLine(             // Парсит строку из файла и заполняет структуру Airplane
    const char* line,       // Строка для парсинга
    Airplane& plane,        // Структура для заполнения
    char* errorMsg,         // Буфер для сообщения об ошибке
    int erroerMsgSize       // Размер буфера errorMsg
);

bool hasDuplicate(           // Проверяет дубли: одинаковый бортовой номер в одно время, одинаковый бортовой номер с разными моделями
    const Airplane planes[], // Массив самолётов
    int indices[], 
    int count, 
    int planeIndex
);

void indexSortByTime(        // Сортировка методом выбора по времени
    const Airplane planes[], // Массив структур
    int indices[],           // Массив индексов
    int count                // Количество элементов
);

const char* skipSpaces(      // Пропускает пробелы в строке
    const char* str          // Указатель на  строку
);

int readFromFile(            // Чтение данных из файла
    const char* filename,    // Имя файла
    Airplane planes[],       // Указатель на массив структур
    char errors[][256],      // Массив строк для ошибок
    int maxCount             // Максимальное количество
);

void printSeparator();       // Вывод разделителя таблицы

void printPlane(             // Вывод одного самолёта в таблицу
    const Airplane& plane    // структура самолёта
);

void printTable(
    const Airplane planes[], // Массив структур
    const int indices[],     // Массив индексов
    int count,               // Количество
    const char* title        // Заголовок таблицы
);

void printErrors(
    char errors[][256],      // Массив ошибок строк
    int count                // Количество ошибок
);

//                                    === MAIN ===

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    //setlocale(LC_ALL, "RUSSIAN");

    // Массив структур
    const int MAX_PLANES = 100;
    Airplane* planes = new Airplane[MAX_PLANES];

    // Массив для хранения ошибок
    char errors[100][256];

    // Индексный массив 
    int* indices = new int[MAX_PLANES];

    // Чтение из файла
    int planeCount = readFromFile(CURRENT_INPUT_FILE, planes, errors, MAX_PLANES);

    if (planeCount == 0){
        cout << "Нет корректных данных для обработки. Программа заврешена" << endl;
        // Освобождаем память
        delete[] planes;
        delete[] indices;
        return 1;
    }// if

    cout << "Корректно прочитанных самолётов: " << planeCount << endl;

    // Инициализация индексного массива
    for (int i = 0; i < planeCount; i++){
        indices[i] = i;
    }

    // Вывод исходных данных
    printTable(planes, indices, planeCount, "\n=== Исходные данные (порядок как в файле) ===");

    // Сортировка методом выбора по времени
    indexSortByTime(planes, indices, planeCount);

    // Вывод отсортированных данных
    printTable(planes, indices, planeCount, "Отсортированные данные по времени входа в зону");

    // Очистка памяти
    for (int i = 0; i < planeCount; i++){
        freePlane(&planes[i]);
    }
    delete[] planes;
    delete[] indices;

    return 0;
}// main

//                                          === РЕАЛИЗАЦИЯ ФУНКЦИЙ ===

bool isValidTime(               // Проверяет корректность ввода времени
    const char* timeStr         // Строка времени
){
    // Проверяем длину (5 символов: ЧЧ:ММ)
    int len = 0;
    while (timeStr[len] != '\0') len++;
    if (len != 5) return false;

    // Проверяем часы: позиции 0 и 1
    if (timeStr[0] < '0' || timeStr[0] > '2') return false;
    if (timeStr[1] < '0' || timeStr[1] > '9') return false;

    // Разделитель: позиция 2
    if (timeStr[2] != ':') return false;

    // Минуты: позиции 3 и 4
    if (timeStr[3] < '0' || timeStr[3] > '5') return false;
    if (timeStr[4] < '0' || timeStr[4] > '9') return false;

    // Проверяем часы: ЧЧ:ММ
    int hour = (timeStr[0] - '0') * 10 + (timeStr[1] - '0');
    if (hour < 0 || hour > 23) return false;

    return true;

}// isValidTime

bool isValidTailNumber(         // Проверяет корректность бортового номера (Б-хххх)
    const char* tailNumber      // Строка бортового номера
){
    
    int len = 0;
    while (tailNumber[len] != '\0') len++;
    if (len != 6) return false;


    // Проверяем букву и дефис
    if (tailNumber[0] != 'B') return false;
    if (tailNumber[1] != '-') return false;

    // Проверям, что дальше только 4 цифры
    for (int i = 2; i < 6; i++){
        if (tailNumber[i] < '0' || tailNumber[i] > '9') return false;
    }//for

    return true;
}// isValidTailNumber

bool isValidFlightNumber(       // Проверяет корректность номера рейса
    const char* flightStr       // Строка номера рейса
){
    // Номер рейса должен состоять только из цифр
    int i = 0;
    while (flightStr[i] != '\0') {
        if (flightStr[i] < '0' || flightStr[i] > '9') return false;
        i++;
    }
    // Номер рейса не должен быть пустым
    return i > 0;

}// isValidFlightNumber

bool isTimeLess(        // Сравнивает две строки времени (ЧЧ:ММ)
    const char* timeA,  // Строка времени A
    const char* timeB   // Строка времени B
){
    int hourA = (timeA[0] - '0') * 10 + (timeA[1] - '0');
    int hourB = (timeB[0] - '0') * 10 + (timeB[1] - '0');
    
    if (hourA != hourB) {
        return hourA < hourB;
    }
    
    int minA = (timeA[3] - '0') * 10 + (timeA[4] - '0');
    int minB = (timeB[3] - '0') * 10 + (timeB[4] - '0');
    
    return minA < minB;
}// isTimeLess

char* allocateAndCopy(      // Выделяет динамическую память под строку и копирует содержимое
    const char* src         // Исходная строка
){
    if (src == NULL) return NULL;
    
    // Считаем длину
    int len = 0;
    while (src[len] != '\0') len++;
    
    // Выделяем память
    char* dest = new char[len + 1];
    
    // Копируем
    for (int i = 0; i <= len; i++) {
        dest[i] = src[i];
    }
    
    return dest;
}// allocateAndCopy

void freePlane(       // Освобождает память структуры Airplane
    Airplane* plane   // Указатель на структуру
){
    if (plane->time) {
        delete[] plane->time;
        plane->time = NULL;
    }
    if (plane->model) {
        delete[] plane->model;
        plane->model = NULL;
    }
    if (plane->tailNumber) {
        delete[] plane->tailNumber;
        plane->tailNumber = NULL;
    }
}// freePlane

const char* skipSpaces(     // Пропускает пробелы в строке
    const char* str         // Указатель на строку
){
    while (*str == ' ' || *str == '\t') {
        str++;
    }
    return str;
}// skipSpaces

bool parseLine(         // Парсит строку из файла и заполняет структуру Airplane
    const char* line,   // Строка для парсинга
    Airplane& plane,    // Структура для заполнения
    char* errorMsg,     // Буфер для сообщения об ошибке
    int errorMsgSize    // Размер буфера errorMsg
){
    const char* ptr = line;
    
    // Инициализируем указатели
    plane.time = NULL;
    plane.model = NULL;
    plane.tailNumber = NULL;
    plane.flightNumber = 0;
    
    // 1. Считываем время
    char timeBuffer[MAX_STR_LEN];
    int i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1) {
        timeBuffer[i] = *ptr;
        i++;
        ptr++;
    }
    timeBuffer[i] = '\0';
    
    if (i == 0) {
        for (int k = 0; k < errorMsgSize; k++) errorMsg[k] = '\0';
        for (int k = 0; k < errorMsgSize - 1 && "Ошибка: время не указано"[k]; k++)
            errorMsg[k] = "Ошибка: время не указано"[k];
        return false;
    }
    
    if (!isValidTime(timeBuffer)) {
        for (int k = 0; k < errorMsgSize; k++) errorMsg[k] = '\0';
        const char* err = "Ошибка: неверный формат времени";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    plane.time = allocateAndCopy(timeBuffer);
    
    // Пропускаем пробелы
    ptr = skipSpaces(ptr);
    
    // 2. Считываем марку
    i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1) {
        timeBuffer[i] = *ptr;
        i++;
        ptr++;
    }
    timeBuffer[i] = '\0';
    if (i == 0) {
        const char* err = "Ошибка: марка ЛА не указана";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    plane.model = allocateAndCopy(timeBuffer);
    
    // Пропускаем пробелы
    ptr = skipSpaces(ptr);
    
    // 3. Считываем бортовой номер
    i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1) {
        timeBuffer[i] = *ptr;
        i++;
        ptr++;
    }
    timeBuffer[i] = '\0';
    if (i == 0) {
        const char* err = "Ошибка: бортовой номер не указан";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    
    if (!isValidTailNumber(timeBuffer)) {
        const char* err = "Ошибка: неверный формат бортового номера (нужно Б-XXXX)";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    plane.tailNumber = allocateAndCopy(timeBuffer);
    
    // Пропускаем пробелы
    ptr = skipSpaces(ptr);
    
    // 4. Считываем номер рейса
    i = 0;
    while (*ptr != '\0' && *ptr != ' ' && *ptr != '\t' && i < MAX_STR_LEN - 1) {
        timeBuffer[i] = *ptr;
        i++;
        ptr++;
    }
    timeBuffer[i] = '\0';
    
    if (!isValidFlightNumber(timeBuffer)) {
        const char* err = "Ошибка: неверный формат номера рейса (только цифры)";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    
    // Преобразуем строку в число
    int flightNum = 0;
    for (int j = 0; timeBuffer[j] != '\0'; j++) {
        flightNum = flightNum * 10 + (timeBuffer[j] - '0');
    }
    plane.flightNumber = flightNum;
    
    // Проверяем, что рейс > 0
    if (plane.flightNumber <= 0) {
        const char* err = "Ошибка: номер рейса должен быть положительным";
        for (int k = 0; k < errorMsgSize - 1 && err[k]; k++) errorMsg[k] = err[k];
        return false;
    }
    
    return true;
}// parseLine

bool hasDuplicate(              // Проверяет дубли: одинаковый бортовой номер в одно время, 
                                // одинаковый бортовой номер с разными моделями
    
    const Airplane planes[],    // Массив самолетов
    int indices[],              // Массив индексов
    int count,                  // Количество самолётов
    int planeIndex              // Индекс проверяемого самолёта
){
    for (int i = 0; i < count; i++) {
        if (i == planeIndex) continue;
        
        // Проверяем: одинаковый БН + одинаковое время
        bool sameTail = false;
        for (int t = 0; planes[indices[i]].tailNumber[t] != '\0' || planes[planeIndex].tailNumber[t] != '\0'; t++) {
            if (planes[indices[i]].tailNumber[t] != planes[planeIndex].tailNumber[t]) {
                sameTail = false;
                break;
            }
            if (planes[indices[i]].tailNumber[t] == '\0' && planes[planeIndex].tailNumber[t] == '\0') {
                sameTail = true;
                break;
            }
        }
        
        if (sameTail) {
            // Проверяем время
            bool sameTime = true;
            for (int t = 0; t < 5; t++) {
                if (planes[indices[i]].time[t] != planes[planeIndex].time[t]) {
                    sameTime = false;
                    break;
                }
            }
            if (sameTime) {
                return true; // Дубль: одинаковый БН в одно время
            }
            
            // Проверяем модель
            bool sameModel = true;
            for (int t = 0; ; t++) {
                if (planes[indices[i]].model[t] != planes[planeIndex].model[t]) {
                    sameModel = false;
                    break;
                }
                if (planes[indices[i]].model[t] == '\0' && planes[planeIndex].model[t] == '\0') break;
            }
            if (!sameModel) {
                return true; // Дубль: одинаковый БН с разной моделью
            }
        }
    }
    return false;
}//hasDuplicate

void indexSortByTime(           // Сортировка методом выбора по времени
    const Airplane planes[],    // Массив структур
    int indices[],              // Массив индексов
    int count                   // Количество элементов
){
    for (int i = 0; i < count - 1; i++) {
        int minIndex = i;
        for (int j = i + 1; j < count; j++) {
            if (isTimeLess(planes[indices[j]].time, planes[indices[minIndex]].time)) {
                minIndex = j;
            }
        }
        if (minIndex != i) {
            int temp = indices[i];
            indices[i] = indices[minIndex];
            indices[minIndex] = temp;
        }
    }
}// indexSortByTime

int readFromFile(           // Чтение данных из файла
    const char* filename,   // Имя файла
    Airplane planes[],      // Указатель на массив структур
    char errors[][256],     // Массив строк для ошибок
    int maxCount            // Максимальное количество
){
    ifstream file(filename);
    
        if (!file.is_open()) {
        cout << "Ошибка: не удалось открыть файл " << filename << endl;
        return 0;
    }
    
    int count = 0;
    int errorCount = 0;
    char line[512];
    
    while (file.getline(line, 512) && count < maxCount) {
        // Пропускаем пустые строки
        if (line[0] == '\0') {
            continue;
        }
        
        Airplane temp;
        char errorMsg[256];
        
        if (parseLine(line, temp, errorMsg, 256)) {
            planes[count] = temp;
            count++;
        } else {
            // Сохраняем ошибку
            int k = 0;
            while (errorMsg[k] != '\0' && k < 255) {
                errors[errorCount][k] = errorMsg[k];
                k++;
            }
            errors[errorCount][k] = '\0';
            errorCount++;
        }
    }
    
    file.close();
    
    // Выводим накопленные ошибки
    if (errorCount > 0) {
        cout << "\n=== Обнаружены ошибки в файле ===" << endl;
        for (int i = 0; i < errorCount; i++) {
            cout << "  " << errors[i] << endl;
        }
        cout << endl;
    }
    
    return count;
}// readFormFile

void printSeparator() {     // Вывод разделителя таблицы
    cout << "------------------------------------------------------------" << endl;
}// printSeparator

void printPlane(            // Вывод одного самолёта в таблицу
    const Airplane& plane   // Структура самолёта
){
    cout << plane.time << " | " 
         << plane.model << " | " 
         << plane.tailNumber << " | " 
         << plane.flightNumber << endl;
}// printPlane

void printTable(                // Вывод таблицы самолётов
    const Airplane planes[],    // Массив структур
    const int indices[],        // Массив индексов
    int count,                  // Количество
    const char* title           // Заголовок таблицы
){
    cout << "\n=== " << title << " ===" << endl;
    printSeparator();
    cout << "Время | Марка ЛА | Бортовой № | Рейс" << endl;
    printSeparator();
    for (int i = 0; i < count; i++) {
        int idx = indices[i];
        printPlane(planes[idx]);
    }
    printSeparator();
    cout << endl;
}// printTable

void printErrors(           // Вывод накопленных ошибок перед таблицей
    char errors[][256],     // Массив строк ошибок
    int count               // Количество ошибок
){
    if (count > 0) {
        cout << "=== Ошибки при разборе файла ===" << endl;
        for (int i = 0; i < count; i++) {
            cout << "  " << errors[i] << endl;
        }
        cout << endl;
    }
}// printErrors