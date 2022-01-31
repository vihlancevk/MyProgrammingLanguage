#include "../include/FileOperations.h"

static void *findNextLine(char *str);
static void moveToNextLine(FILE *foutput);

//================================================================================
//! @brief Функция подсчёта размера файла в байтах.
//!
//! @param [in] finput файл, размер кототого считается.
//!
//! @warning В случае ошибки при работе с файлом будет выведено
//!          соответствующее сообщение и завершена работа программы.
//!
//! @return Размер файла в байтах.
//--------------------------------------------------------------------------------

int getFileSize(FILE *finput)
{
    assert(finput != nullptr);

    if (fseek(finput, 0, SEEK_END))
    {
        printf("Error fseek\n");
        return -1;
    }

    long numberBytesFile = ftell(finput);
    if (numberBytesFile == -1L)
    {
        printf("Error ftell\n");
        return -1;
    }

    rewind(finput);

    return (int)numberBytesFile;
}

//================================================================================
//! @brief Функция считывания файла в буфер.
//!
//! @param [in] finput файл, данные которого считываются.
//! @param [out] str буфер, в который происходит считывание.
//! @param [in] numberBytesFile размер файла finput в байтах.
//!
//! @warning В случае ошибки при работе с файлом будет выведено
//!          соответствующее сообщение и завершена работа программы.
//!
//! @return Указатель на буфер.
//--------------------------------------------------------------------------------

void *readFile(FILE *finput, char *str, int numberBytesFile)
{
    assert(finput != nullptr);
    assert(str != nullptr);
    assert(numberBytesFile > 0);

    if (((int)fread(str, sizeof(char), numberBytesFile, finput) != numberBytesFile) && !feof(finput))
    {
        printf("Error fread\n");
        return nullptr;
    }

    return str;
}

//================================================================================
//! @brief Функция подсчёта количества строк в тексте.
//!
//! @return [in] str буфер с текстом.
//!
//! @return Количество строк в тексте.
//--------------------------------------------------------------------------------

int countNumberLines(char *str, int numberBytesFile)
{
    assert(str != nullptr);

    int linesCount = 0;

    for (int i = 0; i < numberBytesFile; i++)
    {
        if (str[i] == '\n' || (i + 1) == numberBytesFile)
        {
            linesCount++;
            str[i] = '\0';
        }
    }

    return linesCount;
}

static void *findNextLine(char *str)
{
    assert(str != nullptr);

    char *ptrStr = str;

    while (*ptrStr != '\0')
    {
        ptrStr++;
    }

    return ptrStr + 1;
}

//================================================================================
//! @brief Функция разделения текста на строки.
//!
//! @return [out] lines массив структур Line.
//! @return [in] linesCount количество строк в тексте.
//! @return [in] str буфер с текстом.
//!
//! @note В массив структур lines происходит построчная запись текста.
//--------------------------------------------------------------------------------

void splitToLines(Line *lines, int linesCount, char *str)
{
    assert(lines != nullptr);
    assert(linesCount > 0);
    assert(str != nullptr);

    char *ptrStr = str;

    for (int i = 0; i < linesCount; i++)
    {
        lines[i].str = ptrStr;
        lines[i].sizeStr = (int) strlen(lines[i].str);
        ptrStr = (char *)findNextLine(ptrStr);
    }
}

//================================================================================
//! @brief Функция перевода позиции указателя в файле вывода на новую строку.
//!
//! @param [in] foutput файл, в котором нужно переместить позицию указателя
//!             на новую строку.
//--------------------------------------------------------------------------------

static void moveToNextLine(FILE *foutput)
{
    assert(foutput != nullptr);

    fputs("\n", foutput);
}

//================================================================================
//! @brief Функция записи информации о тексте в файл.
//!
//! @param [in] lines массив структур Line, в котором хранится текст.
//! @param [in] linesCount количество строк в тексте.
//! @param [in] foutput файл, в который происходит запись.
//! @param [in] nameSort название сортировки для текста.
//--------------------------------------------------------------------------------

void writeFile(Line *lines, int linesCount, FILE *foutput, const char *nameSort)
{
    assert(lines != nullptr);
    assert(linesCount > 0);
    assert(foutput != nullptr);
    assert(nameSort != nullptr);

    fputs(nameSort, foutput);
    moveToNextLine(foutput);

    for (int i = 0; i < linesCount; i++)
    {
        fputs(lines[i].str, foutput);
        moveToNextLine(foutput);
    }

    moveToNextLine(foutput);
}

