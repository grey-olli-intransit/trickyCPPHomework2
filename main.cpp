#include <iostream>
#include <vector>
#include <algorithm>
#include <locale>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdlib>
//unicode stuff
#include <unicode/unistr.h>
// time measurement
#include "include/timer.h"
#include <functional>
// Реализуйте шаблонную функцию Swap, которая принимает два указателя и обменивает местами значения,
// на которые указывают эти указатели (нужно обменивать именно сами указатели, переменные должны
// оставаться в тех же адресах памяти).
template <typename T>
void swapPointers(T* & first, T* & second) {
    T* tmp;
    tmp = first;
    first = second;
    second = tmp;
}

//Реализуйте шаблонную функцию SortPointers, которая принимает вектор указателей и сортирует
// указатели по значениям, на которые они указывают.
template <typename T>
void SortVectorOfPointers(std::vector<T*> & vectorOfPointers) {
    std::sort(vectorOfPointers.begin(),vectorOfPointers.end(),[](T* left, T* right) {
        return *left < *right;
    });
}
template<typename T>
void printVectorOfPointersValues(const std::vector<T*> & vectorOfPointers) {
    for(auto value : vectorOfPointers) {
        std::cout << *value << " ";
    }
    std::cout << std::endl;
}
//Подсчитайте количество гласных букв в книге “Война и мир”.
// Для подсчета используйте 4 способа:
//○ count_if и find
//○ count_if и цикл for
//○ цикл for и find
//○ 2 цикла for
//Замерьте время каждого способа подсчета и сделайте выводы.

void resetFilePositionToZero(std::fstream & srcfile) {
    srcfile.clear();
    srcfile.seekg(0L,std::fstream::beg);
}

int main() {
//    char * code = setlocale(LC_ALL, "ru_RU.utf8");
//    std::cout << "code = " << code << std::endl;
    std::fstream srcfile{"../source-to-count.txt"};
//    if(srcfile.is_open())
//        std::cout << "File opened.\n";
    std::string line;
    size_t count{0},lineNum{0};
    std::array<char16_t,20>vowels{L'а',L'А',L'е',L'Е',
                                  L'ё',L'Ё',L'и',L'И',
                                  L'о',L'О',L'у',L'У',
                                  L'э',L'Э',L'ы',L'Ы',
                                  L'ю',L'Ю',L'я',L'Я'};
    //char16_t vowels[20]={L'а',L'А',L'е',L'Е',L'ё',L'Ё',L'и',L'И',L'о',L'О',L'у',L'У',L'э',L'Э',L'ы',L'Ы',L'ю',L'Ю',L'я',L'Я'};
    Timer timer("count_if & for");
    while (std::getline(srcfile,line) ) {
        std::istringstream strstream(line);
        while(! strstream.eof()) {
            size_t count_word;
            std::string word;
            strstream >> word;
            icu::UnicodeString wordUtfEncoded(word.c_str());
            std::vector<char16_t> wordCharacters;
            wordCharacters.reserve(50);
            for(auto index=0;index<wordUtfEncoded.length();++index)
                wordCharacters.emplace_back(wordUtfEncoded.charAt(index));
            count_word=std::count_if(wordCharacters.begin(),wordCharacters.end(),
                [&vowels](char16_t charToCheck)
                {
                // на этот цикл
                    for(auto vowel: vowels)
                        if (charToCheck == vowel) return true;
                    return false;
                // проставлен Warning Clang-Tidy:
                // Clang-Tidy: Replace loop by 'std::ranges::any_of()'
                // однако как он должен выглядеть с std::ranges::any_of()
                // я не понимаю. Это вопрос к проверяющему преподавателю
                //  - не могли бы Вы написать в комментарии версию с
                // std::ranges::any_of() ?
                // Итератора же нету у обычного массива.
                // После замены обычного массива на std::array вот этот
                // код для замены:
                // std::ranges::any_of(vowels.begin(),vowels.end(),[charToCheck](char16_t vowel) -> bool {
                //    return (charToCheck == vowel);
                // });
                // выдаёт ошибку:
                // "In template: functional-style cast from 'void' to 'bool' is not allowed"
                // или же, если попытаться собрать, то "error: void value not ignored as it ought to be"
                // Как правильно заменить этот цикл?
                });
            count+=count_word;
        }
        ++lineNum;
        std::cout << "Line " << lineNum << " vowels: " << count << ". ";
    }
    std::cout << std::endl << "Total vowels in file: " << count << std::endl;
    timer.print();

    resetFilePositionToZero(srcfile);
    count = lineNum = 0;

    timer.start("for & for");
    while (std::getline(srcfile,line) ) {
        std::istringstream strstream(line);
        while(! strstream.eof()) {
            size_t count_word=0;
            std::string word;
            strstream >> word;
            icu::UnicodeString wordUtfEncoded(word.c_str());
            std::vector<char16_t> wordCharacters;
            wordCharacters.reserve(50);
            for(auto index=0;index<wordUtfEncoded.length();++index)
                wordCharacters.emplace_back(wordUtfEncoded.charAt(index));
            for (char16_t wordCharacter : wordCharacters)
                for (auto vowel: vowels)
                    if (wordCharacter == vowel) ++count_word;
            count+=count_word;
        }
        ++lineNum;
        std::cout << "Line " << lineNum << " vowels: " << count << ". ";
    }
    std::cout << std::endl << "Total vowels in file: " << count << "." << std::endl;
    timer.print();

    resetFilePositionToZero(srcfile);
    count = lineNum = 0;

    timer.start("for & find");
    // задаём строками, так как символы русские и способ из vowels[20] для std::string::find() не подойдёт
    std::vector<std::string> vowelsVect{"а","А","е","Е","ё","Ё","и","И","о","О","у","У","э","Э","ы","Ы","ю","Ю","я","Я"};
    while (std::getline(srcfile,line) ) {
        std::istringstream strstream(line);
        while(! strstream.eof()) {
            size_t count_word=0;
            std::string word;
            strstream >> word;
            for(auto & vowel : vowelsVect) {
                std::size_t pos=0;
                while (pos != std::string::npos) {
                    pos = word.find(vowel, pos);
                    if(pos != std::string::npos) {
                     ++count_word;
                     ++pos;
                    }
                }
            }
            count+=count_word;
        }
        ++lineNum;
        std::cout << "Line " << lineNum << " vowels: " << count << ". ";
    }
    std::cout << std::endl << "Total vowels in file: " << count << "." << std::endl;
    timer.print();

    std::cout << "Способа использовать std::string::find вместе с STL count_if я не придумал - find тут лишнее." << std::endl;
    std::cout << "Так что в сравнении участвуют только три способа подсчёта." << std::endl;
    std::cout << "Просто на циклах for быстрее чем с использованием STL count_if(),  а использование" << std::endl;
    std::cout << " std::string::find делает способ самым медленным из трёх." <<  std::endl;
    std::cout << "------" << std::endl;
    int a=1, b=2, c=4, d=3;
    std::vector<int *> vectorOfPointers{&a,&b,&c,&d};
    std::cout << "List of values pointed by vector before sort of vector:" << std::endl;
    printVectorOfPointersValues(vectorOfPointers);
    SortVectorOfPointers(vectorOfPointers);
    std::cout << "List of values pointed by vector after sort of vector:" << std::endl;
    printVectorOfPointersValues(vectorOfPointers);
    std::cout << "------" << std::endl;
    int * aPointer = &a;
    int * bPointer = &b;
    std::cout << "*aPointer: " << *aPointer << std::endl;
    std::cout << "*bPointer: " << *bPointer << std::endl;
    swapPointers(aPointer,bPointer);
    std::cout << "After swapPointers():" << std::endl;
    std::cout << "*aPointer: " << *aPointer << std::endl;
    std::cout << "*bPointer: " << *bPointer << std::endl;
    return 0;
}
