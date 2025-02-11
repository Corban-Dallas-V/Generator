#include "generators.hpp"

using namespace coro;

int main() {
    try {   
        // Создание списка значений list (rvalue)
        itGenerator<std::list<int>> genr(std::list<int>({1, 2, 3}));

        while (auto value = genr.next()) {
            std::cout << "Next value: " << *value << std::endl;
        }
        
        std::cout << "list<int>" << std::endl;
        // Создание списка значений
        std::list<int> values = {1, 2, 3};

        // Создание генератора для списка
        auto genl = std::make_unique<itGenerator<std::list<int>>>(values);

        while (auto value = genl->next()) {
            std::cout << "Next value: " << *value << std::endl;
        }
        
        std::cout << "vector<string>" << std::endl;

        std::vector<std::string> strings = {"3;", "2;", "1;"};
        auto genv = itGenerator<std::vector<std::string>>(strings);

        while (auto value = genv.next()) {
            std::cout << "Next value: " << *value << std::endl;
        }
        
        std::cout << "Generator<int>" << std::endl;
        
        auto geni = Generator<int>(1, 15, 3);

        while (auto value = geni.next()) {
            std::cout << *value << std::endl;
        }
        
        std::cout << "rGenerator<float>" << std::endl;

        // Использование нового конструктора с шагом.
        auto genf = rGenerator<float>(-3, 1, 0.2);
        while (auto value = genf.next()) {
            std::cout << *value << std::endl;
        }

        std::cout << "rGenerator<double>" << std::endl;

        // Использование нового конструктора с шагом.
        auto gend = rGenerator<double>(-2, 2, 0.5);
        while (auto value = gend.next()) {
            std::cout << *value << std::endl;
        }

        // Использование for_in
        std::vector<int> numbers = {1, 2, 3, 4, 5};
        //itGenerator<std::vector<int>> gen(numbers);

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