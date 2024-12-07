#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>

using namespace std;


struct Bounds {
    double x_min, x_max, y_min, y_max;


    bool isValid() const {
        return x_min <= x_max && y_min <= y_max;
    }
};

// Класс для управления элементами и границами
class ClassElements {
private:
    Bounds bounds; // Границы для класса
    vector<pair<double, double>> elements; 
    pair<double, double> center; 

public:
    // Конструктор, принимающий границы
    ClassElements(Bounds b) : bounds(b) {}

    // Метод для генерации элементов внутри заданных границ
    void generateElements(int numElements) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> distX(bounds.x_min, bounds.x_max);
        uniform_real_distribution<> distY(bounds.y_min, bounds.y_max);

        double sumX = 0, sumY = 0; // Для нахождения центра

        for (int i = 0; i < numElements; i++) {
            double x = distX(gen);
            double y = distY(gen);
            elements.push_back(make_pair(x, y)); // Добавляем элемент (точку) в вектор
            sumX += x;
            sumY += y;
        }


        center = make_pair(sumX / numElements, sumY / numElements);

        // Выводим Манхэттенские расстояния между первой точкой и остальными
        if (elements.size() > 1) {
            for (size_t i = 1; i < elements.size(); ++i) {
                double distance = manhattanDistance(elements[0], elements[i]);
                cout << "Манхэттенское расстояние между ("
                     << elements[0].first << ", " << elements[0].second << ") и ("
                     << elements[i].first << ", " << elements[i].second << ") = "
                     << distance << endl;
            }
        }
    }

    // Метод для вычисления Манхэттенского расстояния между двумя точками
    double manhattanDistance(const pair<double, double>& p1, const pair<double, double>& p2) const {
        return abs(p1.first - p2.first) + abs(p1.second - p2.second);
    }

    // Метод для записи элементов в файл
    void saveToFile(ofstream& outputFile, int classNumber) const {
        for (const auto& element : elements) {
            outputFile << element.first << " " << element.second << " " << classNumber << endl;
        }
    }

    // Метод для получения центра класса
    pair<double, double> getCenter() const {
        return center;
    }

    // Метод для проверки корректности границ
    bool checkBounds() const {
        return bounds.isValid();
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

    // Вектор для хранения всех классов
    vector<ClassElements> allClasses;

    for (int i = 0; i < numClasses; i++) {
        cout << "Класс " << i + 1 << ":\n";
        int numElements;
        cout << "Количество элементов: ";
        cin >> numElements;

        double x_min, x_max, y_min, y_max;
        Bounds bounds;
        do {
            cout << "Ограничения (x_min, x_max, y_min, y_max): ";
            cin >> bounds.x_min >> bounds.x_max >> bounds.y_min >> bounds.y_max;

            if (bounds.isValid()) {
                // Создаём объект класса с корректными границами и добавляем в вектор
                ClassElements newClass(bounds);
                newClass.generateElements(numElements);
                allClasses.push_back(newClass);
                break;
            } else {
                cout << "Ошибка: x_min должен быть <= x_max и y_min <= y_max. Попробуйте снова.\n";
            }
        } while (true);
    }

    // Вычисляем Манхэттенские расстояния между центрами всех классов
    cout << "\nМанхетовские расстояния между центрами классов:\n";
    for (size_t i = 0; i < allClasses.size(); i++) {
        for (size_t j = i + 1; j < allClasses.size(); j++) {
            double distance = allClasses[i].manhattanDistance(allClasses[i].getCenter(), allClasses[j].getCenter());
            cout << "Расстояние между центром класса " << i + 1 << " и центром класса " << j + 1 << " = " << distance << endl;
        }
    }

    // Запись всех данных в файл
    for (size_t i = 0; i < allClasses.size(); i++) {
        allClasses[i].saveToFile(outputFile, i + 1);
    }

    outputFile.close();
    cout << "Данные успешно записаны в файл output_data.txt!" << endl;

    return 0;
}
