#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>

class Perceptron {
public:
    Perceptron(int input_size, double learning_rate = 0.1)
        : weights(input_size + 1), learning_rate(learning_rate) {
        // Инициализируем веса случайными значениями
        std::srand(std::time(nullptr));
        for (double &weight : weights) {
            weight = ((double)std::rand() / RAND_MAX) * 2 - 1; // От -1 до 1
        }
    }

    // Функция активации (пороговая)
    int activation(double x) {
        return x >= 0 ? 1 : 0;
    }

    // Прогнозирование на одном экземпляре
    int predict(const std::vector<double> &inputs) {
        double sum = weights[0]; // Смещение (bias)
        for (size_t i = 0; i < inputs.size(); ++i) {
            sum += weights[i + 1] * inputs[i];
        }
        return activation(sum);
    }

    // Обучение персептрона на наборе данных
    void train(const std::vector<std::vector<double>> &training_inputs,
               const std::vector<int> &labels,
               int epochs) {
        for (int epoch = 0; epoch < epochs; ++epoch) {
            for (size_t i = 0; i < training_inputs.size(); ++i) {
                int prediction = predict(training_inputs[i]);
                int error = labels[i] - prediction;

                // Обновление весов
                weights[0] += learning_rate * error; // Обновление смещения
                for (size_t j = 0; j < training_inputs[i].size(); ++j) {
                    weights[j + 1] += learning_rate * error * training_inputs[i][j];
                }
            }
        }
    }

private:
    std::vector<double> weights;
    double learning_rate;
};

int main() {
    // Пример данных для обучения (логическая функция AND)
    std::vector<std::vector<double>> training_inputs = {
        {0, 0},
        {0, 1},
        {1, 0},
        {1, 1}
    };

    std::vector<int> labels = {0, 0, 0, 1};

    Perceptron perceptron(2);

    perceptron.train(training_inputs, labels, 10);

    // Тестирование персептрона
    std::cout << "Тестирование персептрона:\n";
    for (const auto &inputs : training_inputs) {
        int output = perceptron.predict(inputs);
        std::cout << inputs[0] << " AND " << inputs[1] << " = " << output << "\n";
    }

    return 0;
}
