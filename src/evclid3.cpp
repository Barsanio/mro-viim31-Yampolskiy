#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>

using namespace std;


class ClassElements {
private:
    vector<pair<double, double>> bounds; // Границы для каждого измерения
    vector<vector<double>> elements; // Список точек (каждая точка - вектор координат)
    vector<double> center; // Центр класса в n-мерном пространстве

public:
    // Конструктор принимает вектор границ (для каждого измерения: min и max)
    ClassElements(const vector<pair<double, double>>& b) : bounds(b) {}

    // Метод для генерации элементов внутри заданных границ
    void generateElements(int numElements) {
        random_device rd;
        mt19937 gen(rd());

        int dimensions = bounds.size(); // Число измерений
        center.resize(dimensions, 0.0); // Инициализируем центр нулями

        for (int i = 0; i < numElements; ++i) {
            vector<double> point(dimensions); // Создаём точку в n-мерном пространстве
            for (int d = 0; d < dimensions; ++d) {
                uniform_real_distribution<> dist(bounds[d].first, bounds[d].second);
                point[d] = dist(gen); // Генерируем случайную координату для измерения d
                center[d] += point[d]; // Накопление суммы для расчёта центра
            }
            elements.push_back(point); // Добавляем точку в список элементов
        }

        // Находим центр как среднее арифметическое координат
        for (int d = 0; d < dimensions; ++d) {
            center[d] /= numElements;
        }
    }

    // Метод для вычисления Манхэттенского расстояния между двумя точками
    double manhattanDistance(const vector<double>& p1, const vector<double>& p2) const {
        double distance = 0.0;
        for (size_t i = 0; i < p1.size(); ++i) {
            distance += abs(p1[i] - p2[i]);
        }
        return distance;
    }

    // Метод для вычисления Евклидова расстояния между двумя точками
    double euclideanDistance(const vector<double>& p1, const vector<double>& p2) const {
        double distance = 0.0;
        for (size_t i = 0; i < p1.size(); ++i) {
            distance += pow(p1[i] - p2[i], 2);
        }
        return sqrt(distance);
    }

    // Метод для записи элементов в файл
    void saveToFile(ofstream& outputFile, int classNumber) const {
        for (const auto& element : elements) {
            for (double coord : element) {
                outputFile << coord << " ";
            }
            outputFile << classNumber << endl;
        }
    }

    // Получение центра класса
    const vector<double>& getCenter() const {
        return center;
    }
};

int main() {
    int numClasses;
    cout << "Введите количество классов: ";
    cin >> numClasses;

    ofstream outputFile("output_data.txt");
    if (!outputFile) {
        cerr << "Ошибка: Не удалось создать файл" << endl;
        return 1;
    }

    vector<ClassElements> allClasses;

    for (int i = 0; i < numClasses; ++i) {
        cout << "Класс " << i + 1 << ":\n";
        int numElements, dimensions;
        cout << "Количество элементов: ";
        cin >> numElements;
        cout << "Количество измерений: ";
        cin >> dimensions;

        vector<pair<double, double>> bounds(dimensions);
        for (int d = 0; d < dimensions; ++d) {
            cout << "Границы для измерения " << d + 1 << " (min, max): ";
            cin >> bounds[d].first >> bounds[d].second;
        }

        ClassElements newClass(bounds);
        newClass.generateElements(numElements);
        allClasses.push_back(newClass);
    }

    cout << "\nМанхэттенские расстояния между центрами классов:\n";
    for (size_t i = 0; i < allClasses.size(); ++i) {
        for (size_t j = i + 1; j < allClasses.size(); ++j) {
            double distance = allClasses[i].manhattanDistance(
                allClasses[i].getCenter(), allClasses[j].getCenter()
            );
            cout << "Расстояние между центром класса " << i + 1
                 << " и центром класса " << j + 1 << " = " << distance << endl;
        }
    }

    // Записываем все данные в файл
    for (size_t i = 0; i < allClasses.size(); ++i) {
        allClasses[i].saveToFile(outputFile, i + 1);
    }

    outputFile.close();
    cout << "Данные успешно записаны в файл output_data.txt!" << endl;

    return 0;
}
