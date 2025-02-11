# Generator
Генератор, наподобие корутины

Упрощенные версии генераторов с поведением как у корутин.
На данный момент реализованы два подхода:
- последовательности значений передаются через итерируемые объекты
- задается диапазон и шаг для генерации значений налету

Так же имеются две версии функции for_in для удобства использования генераторов


## Использование

К cpp-файлу подключается файл generators.hpp и через пространство имен **coro** 
открывается доступ к генераторам.

Примеры использования расположены в файле usage.cpp

Выглядит так:
```C++
#include generators.hpp

using namespace coro;

int main() {
    try {   
        // Создание списка значений list (rvalue)
        itGenerator<std::list<int>> genr(std::list<int>({1, 2, 3}));

        while (auto value = genr.next()) {
            std::cout << "Next value: " << *value << std::endl;
        }
        
        // Создание списка значений строк
        std::cout << "vector<string>" << std::endl;

        std::vector<std::string> strings = {"3;", "2;", "1;"};
        auto genv = itGenerator<std::vector<std::string>>(strings);

        while (auto value = genv.next()) {
            std::cout << "Next value: " << *value << std::endl;
        } 
        
        // Создание списка целочисленных значений с шагом, налету
        std::cout << "Generator<int>" << std::endl;
        
        auto geni = Generator<int>(1, 15, 3);

        while (auto value = geni.next()) {
            std::cout << *value << std::endl;
        }  
        
        // Создание списка вещественных значений с шагом, налету
        std::cout << "rGenerator<float>" << std::endl;

        auto genf = rGenerator<float>(-3, 1, 0.2);
        while (auto value = genf.next()) {
            std::cout << *value << std::endl;
        } 
        
        // Использование for_in
        std::vector<int> numbers = {1, 2, 3, 4, 5};

        int sum = 0;
        for_in(itGenerator<std::vector<int>>(numbers), [&](int num) {
            sum += num;
            std::cout << "Current number: " << num << std::endl;
        });

        std::cout << "Sum: " << sum << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
```