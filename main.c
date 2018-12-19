/* 
Программа: 
1) Открывает существующий бинарный файл или создаёт новый и заполняет его тестовыми данными. 
2) Выводит на консоль содержимое файла.
3) Сортирует людей по именам в алфавитном порядке.
4) Выводит на консоль содержимое файла.
5) Сортирует людей по убыванию возраста.
6) Выводит на консоль содержимое файла.
7) Удаляет людей с наибольшим возрастом.
8) Выводит на консоль содержимое файла.
*/

#define _CRT_NONSTDC_NO_DEPRECATE // Отключение сообщений об устаревших, с точки зрения Microsoft, функциях, не входящих в стандарт.
#define _CRT_SECURE_NO_WARNINGS   // Отключение сообщений о небезопасных, с точки зрения Microsoft, функциях.
#define _CRT_SECURE_NO_DEPRECATE  // Отключение сообщений об устаревших, с точки зрения Microsoft, небезопасных функциях.

#include <stdio.h>                // Для стуктуры FILE, функций работы с ней и функций fileno() и printf().
#include <string.h>               // Для работы со строками и функции strcmp() в частности.
#include <conio.h>                // Для функции getch().
#include <stdlib.h>               // Для системного вызова exit().
#include <io.h>                   // Для функции chsize().

#define SUCCESS 0                 // Код завершения программы, свидетельствующий о штатном завершении.
#define FILE_CREATE_ERROR -1      // Код завершения программы при аварийном событии, из-за неудачи при создании файла.
#define FILE_OPEN_ERROR -2        // Код завершения программы при аварийном событии, из-за неудачи при открытии файла.

#define FALSE 0                   // Объявим такое ключевое слово для удобства и читаемости кода.
#define CUSTOM_TRUE 1             // Аналогично с FALSE, только префикс CUSTOM, потому что можно использовать только в наших функциях.

#define FIELD_SIZE 100            // Размер текстовых полей в структурах.
#define TEST_RECORDS_COUNT 13     // Количество тестовых записей.

// Объявляем структуру t_person и для удобства работы в чистом Си через typedef позволяем не использовать каждый раз ключевое слово 
// struct при объявлении.
typedef struct t_person 
{
    char name[FIELD_SIZE];        // Имя.
    int age;                      // Возраст.
} t_person;

// Универсальная шаблонная функция чтения записи заданного типа из файла по её порядковому номеру.
template <typename T>
T read_record_from_file(FILE *pfile, int index)
{
    // Рассчитываем смещение, по которому от начала файла должна находиться нужная запись.
    int offset = index * sizeof(T);
    // Переносим указатель позиции на рассчитанное смещение от начала файла.
    fseek(pfile, offset, SEEK_SET);

    // Читаем запись заданного типа из файла.
    T readed_record;
    fread(&readed_record, sizeof(T), 1, pfile);

    // Для безопасности возвращаем указатель позиции в файле на начало файла.
    rewind(pfile);

    // Возвращаем прочитанную запись.
    return readed_record;
}

// Универсальная шаблонная функция записи записи заданного типа в файл по указанному порядковому номеру.
template <typename T>
void write_record_in_file(FILE *pfile, const T *record_to_write, int index)
{
    // Рассчитываем смещение, по которому от начала файла должна находиться нужная запись.
    int offset = index * sizeof(T);
    // Переносим указатель позиции на рассчитанное смещение от начала файла.
    fseek(pfile, offset, SEEK_SET);

    // Записываем запись заданного типа в файл.
    fwrite(record_to_write, sizeof(T), 1, pfile);

    // На всякий случай принуждаем подсистему ввода/вывода записать содержимое её буфера в файл прямо сейчас.
    fflush(pfile);

    // Для безопасности возвращаем указатель позиции в файле на начало файла.
    rewind(pfile);
}

// Универсальная шаблонная функция взаимного переноса двух записей в файле по их индексам.
template <typename T>
void swap_records_in_file(FILE *pfile, int record_index1, int record_index2)
{
    // Читаем обе записи из файла.
    T record1 = read_record_from_file<T>(pfile, record_index1);
    T record2 = read_record_from_file<T>(pfile, record_index2);

    // Перезаписываем их на места друг друга.
    write_record_in_file<T>(pfile, &record1, record_index2);
    write_record_in_file<T>(pfile, &record2, record_index1);
}

// Функция проверки существования файла. Возвращает ненулевое значение, если файл существует, иначе - 0.
int file_exists(const char *filename)
{
    int file_exists = FALSE;
    FILE *pfile = fopen(filename, "r");
    if (pfile != NULL)
    {
        // Если файл существует, то закрываем его, так как мы открывали его для этой проверки.
        fclose(pfile);     
        // Устанавливаем флаг существования файла.
        file_exists = CUSTOM_TRUE;
    }
    else
        file_exists = FALSE;

    return file_exists;
}

// Функция заполнения файла тестовыми данными.
void create_file_with_test_data(const char *filename)
{
    // Создаём файл, и закрываем программу, если не получилось.
    FILE *pfile = fopen(filename, "wb");
    if (pfile == NULL) 
    {
        printf("Can't create file '%s'. Press any key to exit.", filename);
        getch();
        exit(FILE_CREATE_ERROR);
    }

    // Инициализируем массив тестовых данных.
    t_person person_ar[TEST_RECORDS_COUNT] = {
        { "Frank", 27 },
        { "Dick", 35 },
        { "Bob", 17 },
        { "Mike", 41 },
        { "Anna", 23 },
        { "Pit", 31 },
        { "Den", 19 },
        { "Jenna", 35 },
        { "Jack", 41 },
        { "Ken", 16 },
        { "Barby", 18 },
        { "Jill", 41 },
        { "Zed", 20 }
    };

    // Записываем массив с тестовыми данными в файл 
    fwrite(person_ar, sizeof(t_person), TEST_RECORDS_COUNT, pfile);

    // (Могли бы в цикле воспользоваться нашей функцией write_record_in_file_by_index(), но это было бы медленнее):
    /*
    for (int i = 0; i < TEST_RECORDS_COUNT; i++)
        write_record_in_file_by_index<t_person>(pfile, &person_ar[i], i);
    */

    // Закрываем файл.
    fclose(pfile);
}

// Функция получения размера файла в байтах.
int get_file_size_in_bytes(FILE *pfile)
{
    int size = 0;
    fseek(pfile, 0, SEEK_END);    // Переносим указатель позиции в конец файла.
    size = ftell(pfile);          // Считываем число байт от начала файла до положения указателя позиции.
    rewind(pfile);                // Для безопасности переносим указатель позиции обратно на начало файла.
    return size;
}

// Универсальная шаблонная функция для получения количества записей заданного типа в файле.
template <typename T>
int get_records_count_in_file(FILE *pfile)
{
    return get_file_size_in_bytes(pfile) / sizeof(T);
}

// Функция печати на консоль одной записи типа t_person.
void print_person(const t_person *person)
{
    printf("Name: %s\n", person->name);
    printf("Age: %d\n", person->age);
}

// Функция вывода на консоль содержимого файла.
void print_file(FILE *pfile)
{
    // Получаем количество людей в файле.
    int persons_count = get_records_count_in_file<t_person>(pfile);

    for (int i = 0; i < persons_count; i++)
    {
        t_person current_person = read_record_from_file<t_person>(pfile, i);
        print_person(&current_person);
        printf("\n");
    }
}

// Функция-компаратор, которая сравнивает людей по имени. Возвращает: 
// 0 - имя у person1 больше, чем у person2
// ненулевое число - имя у person1 меньше, чем у person2
int compare_persons_by_name(const t_person *person1, const t_person *person2)
{
    return (strcmp(person1->name, person2->name) < 0) ? (1) : (0);
}

// Функция-компаратор, которая сравнивает людей по возрасту в обратном порядке. Возвращает: 
// 0 - возраст у person1 меньше, чем у person2
// ненулевое число - возраст у person1 больше, чем у person2.
int compare_persons_by_age(const t_person *person1, const t_person *person2)
{
    return (person1->age > person2->age) ? (1) : (0);
}

// Универсальная шаблонная функция поиска экстремума (максимума или минимума в зависимости от переданного указателя на 
// функцию-компаратор) в файле с записями заданного типа.
template <typename T>
int find_extremum_index_in_file(FILE *pfile,
    int total_records_count,
    int start_index,
    int (*compare_records)(const T *record1, const T *record2))
{
    int extremum_index = start_index;
    T extremum_record = read_record_from_file<T>(pfile, extremum_index);

    for (int i = start_index + 1; i < total_records_count; i++)
    {
        T current_record = read_record_from_file<T>(pfile, i);
        if (compare_records(&current_record, &extremum_record))
        {
            extremum_index = i;
            extremum_record = current_record;
        }
    }

    return extremum_index;
}

// Универсальная шаблонная функция сортировки записей в файле по признаку, передаваемому через указатель на функцию-компаратор.
template <typename T>
void sort_file(FILE *pfile, int (*compare_records)(const T *record1, const T *record2))
{
    // Получаем количество записей в файле.
    int records_count = get_records_count_in_file<t_person>(pfile);

    // Сортируем записи в файле алгоритмом "Сортировка выбором" ("поиском минимума").
    for (int i = 0; i < records_count; i++)
    {
        int extremum_index = find_extremum_index_in_file(pfile, records_count, i, compare_records);
        swap_records_in_file<t_person>(pfile, i, extremum_index);
    }
}

// Функция установления нового размера файла в байтах.
void set_file_size(FILE *pfile, int new_size)
{
    // Так как системной функции chsize(), которая изменяет размер файла, в качестве параметра нужен не указатель
    // на структуру FILE, а числовой дескриптор файла, получаем его из нашего указателя на FILE с помощью функции fileno().
    int file_descriptor = fileno(pfile);
    chsize(file_descriptor, new_size);
}

// Универсальная шаблонная функция удаления записи заданного типа из файла по её порядковому номеру.
template <typename T>
void remove_record_from_file(FILE *pfile, int record_index)
{
    // Получаем количество записей в файле.
    int records_count = get_records_count_in_file<t_person>(pfile);

    // Сдвигаем все записи, начиная со следующей после удаляемой, на одну запись влево, чтобы затереть её.
    for (int i = record_index + 1; i < records_count; i++)
    {
        T current_record = read_record_from_file<T>(pfile, i);
        write_record_in_file<T>(pfile, &current_record, i - 1);
    }

    // Рассчитываем новый размер файла (без удалённой записи) в байтах.
    int new_file_size = (records_count - 1) * sizeof(T);

    // Устанавливаем новый размер файла.
    set_file_size(pfile, new_file_size);
}

// Функция удаления людей с максимальным возрастом из файла.
void remove_oldest_persons_from_file(FILE *pfile)
{
    // Получаем количество людей в файле.
    int persons_count = get_records_count_in_file<t_person>(pfile);

    // Находим максимальный возраст. Для удобства используем уже существующую функцию нахождения экстремума, которую 
    // задействовали в функции сортировки файла.
    int person_with_max_age_index = find_extremum_index_in_file<t_person>(pfile, persons_count, 0, compare_persons_by_age);
    int max_age = read_record_from_file<t_person>(pfile, person_with_max_age_index).age;

    // Смотрим всех людей в файле и удаляем тех, чей возраст равен максимальному. 
    // Важно, что после каждого удаления persons_count и i надо уменьшать на 1.
    for (int i = 0; i < persons_count; i++)
    {
        t_person current_person = read_record_from_file<t_person>(pfile, i);
        if (current_person.age == max_age)
        {
            remove_record_from_file<t_person>(pfile, i);
            persons_count--;
            i--;
        }
    }
}


int main()
{
    int program_result = FILE_OPEN_ERROR;
    const char *filename = "persons.bin";

    // Проверяем существует ли файл, и если нет, то создаём его и заполняем тестовыми данными.
    if (!file_exists(filename))
    {
        create_file_with_test_data(filename);
        printf("Binary file '%s' successfully created and filled with test data.\n", filename);
    }

    // Открываем файл, и если удалось, то делаем все необходимые действия над ним.
    FILE *pfile = fopen(filename, "r+b");
    if (pfile != NULL)
    {
        printf("Existed binary file '%s' successfully opened.\n", filename);

        // Вывод на консоль начального содержимого файла.
        printf("Origin file '%s':\n\n", filename);
        print_file(pfile);

        // Сортировка людей в файле по именам в алфавитном порядке и вывод на консоль получившегося файла.
        sort_file<t_person>(pfile, compare_persons_by_name);
        printf("\nSorted by names file '%s':\n\n", filename);
        print_file(pfile);

        // Сортировка людей в файле по убыванию возраста и вывод на консоль получившегося файла.
        sort_file<t_person>(pfile, compare_persons_by_age);
        printf("\nSorted by age file '%s':\n\n", filename);
        print_file(pfile);

        // Удаление людей с самым большим возрастом из файла и вывод на консоль получившегося файла.
        remove_oldest_persons_from_file(pfile);
        printf("\nFile '%s' with removed oldest persons:\n\n", filename);
        print_file(pfile);

        // Закрываем файл и проставляем код успешного завершения программы.
        fclose(pfile);
        program_result = SUCCESS;
    }
    else
    {
        printf("Can't open file '%s'. Program will be closed.", filename);
        program_result = FILE_OPEN_ERROR;   // Не удалось открыть файл.
    }

    printf("\nPress any key to close the program.");
    getch();
    return program_result;
}

