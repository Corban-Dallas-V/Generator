#include <iostream>
#include <memory>
#include <optional>
#include <type_traits>
#include <list>
#include <string>
#include <vector>

namespace coro {

class GeneratorBase
{
public:
    enum class GeneratorStates
    {
        Creation,
        Waiting,
        Generation,
        Completion
    };
};

// Специализация для итерабельных диапазонов:
template <typename RangeType>
class itGenerator: public GeneratorBase
{
private:
    struct Data
    {
        RangeType                       range;
        typename RangeType::iterator    current;
        GeneratorStates                 state;

        Data(RangeType r)
            : range(std::move(r)), current(range.begin()), state(GeneratorStates::Creation) {}
    };

    std::unique_ptr<Data> data;

public:                                                                                
    using ValueType = std::decay_t<decltype(*std::declval<RangeType>().begin())>;
    static_assert(!std::is_reference_v<std::decay_t<decltype(*std::declval<RangeType>().begin())>>, "Val is a reference");

    explicit itGenerator(RangeType& range) : data(std::make_unique<Data>(range)) {
        data->state     = GeneratorStates::Generation;
    }

    explicit itGenerator(RangeType&& range) : data(std::make_unique<Data>(std::move(range))) {
        data->state     = GeneratorStates::Generation;
    }

    // Функция для получения следующего значения из диапазона
    std::optional<ValueType> next() {
        switch (data->state)  {
        case GeneratorStates::Generation:
        {
            if (data->current == data->range.end()) {
                data->state = GeneratorStates::Completion;
                return std::nullopt;
            }
            ValueType val    = *data->current;
            data->current++;
            return std::make_optional<ValueType>(val);
        }
            break;

        case GeneratorStates::Completion:
            return std::nullopt;

        default:
            throw std::runtime_error("Invalid generator state");
        }
    }

    // Принудительное завершение генератора
    void forceComplete() {
        data->state  = GeneratorStates::Completion;
    }

    // Перезапуск генератора
    void reset() {
        data->state = GeneratorStates::Generation;
        data->current = data->begin;
    }
};

// Специализация для целочисленных диапазонов:

template<typename T, typename = typename std::enable_if<std::is_integral<T>::value>::type>
class Generator: public GeneratorBase {
    struct Data {
        T               from;
        T               to;
        T               step;
        T               current;
        GeneratorStates state;

        Data(T from, T to, T step)
            : from(from), to(to), step(step), current(from)
            , state(GeneratorStates::Creation) {}
    };

    std::unique_ptr<Data> data;

public:    
    Generator(T from, T to, T step)
        : data(std::make_unique<Data>(from, to, step)) {
        data->state = (step == 0)? GeneratorStates::Completion: GeneratorStates::Generation;
    }

    std::optional<T> next() {
        switch (data->state) {
        case GeneratorStates::Generation:
        {
            if (data->current > data->to) {
                data->state = GeneratorStates::Completion;
                return std::nullopt;
            } else {
                auto val      = data->current;
                data->current = data->current + data->step;
                return std::optional<T>(val);
            }
        }
        case GeneratorStates::Completion:
            return std::nullopt;

        default:
            throw std::runtime_error("Invalid Generator state");
        }
    }

    void reset() {
        data->current   = data->from;
        data->state    = GeneratorStates::Generation;
    }
};

// Специализация для вещественных диапазонов:

template<typename T, typename = typename std::enable_if<std::is_floating_point<T>::value>::type>
class rGenerator: public GeneratorBase {
public:
    struct Data {
        double            from;
        double            to;
        double            step;
        double            current;
        GeneratorStates   state;

        Data(double from, double to, double step)
            : from(from), to(to), step(step), current(from)
            , state(GeneratorStates::Creation) {}
    };

    std::unique_ptr<Data> data;

public:    
    explicit rGenerator(double from, double to, double step)
        : data(std::make_unique<Data>(from, to, step)) {
        data->state = (step == 0)? GeneratorStates::Completion: GeneratorStates::Generation;
    }

    std::optional<double> next() {
        switch (data->state) {
        case GeneratorStates::Generation:
        {
            if (data->current > data->to) {
                data->state = GeneratorStates::Completion;
                return std::nullopt;
            } else {
                auto val      = data->current;
                data->current = data->current + data->step;
                return std::optional<T>(val);
            }
        }
        case GeneratorStates::Completion:
            return std::nullopt;

        default:
            throw std::runtime_error("Invalid Generator state");
        }
    }

    void reset() {
        data->current    = data->from;
        data->state      = GeneratorStates::Generation;
    }
};

/// @brief Функция для удобства работы с генератором
/// @tparam GeneratorType 
/// @tparam FuncType 
/// @param generator 
/// @param func 
template <typename GeneratorType, typename FuncType>
void for_in(GeneratorType& generator, FuncType func) {
    std::optional<typename GeneratorType::ValueType> value;
    while ((value = generator.next())) {
        func(*value); // Pass the value to the function
    }
}

template <typename GeneratorType, typename FuncType>
void for_in(GeneratorType&& generator, FuncType func) {
    std::optional<typename GeneratorType::ValueType> value;
    auto generator_ = std::move(generator);
    while ((value = generator_.next())) {
        func(*value); // Pass the value to the function
    }
}

} // namespace coro