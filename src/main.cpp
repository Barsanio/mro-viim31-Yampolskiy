#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>
#include <algorithm>

using namespace std;

// Структура для хранения границ (ограничений) каждого класса
struct Bounds {
    double x_min, x_max, y_min, y_max;

    // Метод проверки корректности ограничений
    bool isValid() const {
        return x_min <= x_max && y_min <= y_max;
    }
};

// Класс для управления элементами и границами
class ClassElements {
private:
    Bounds bounds; // Границы для класса
    vector<pair<double, double>> elements; // Вектор для хранения элементов (точек)

public:
    // Конструктор, принимающий границы
    ClassElements(Bounds b) : bounds(b) {}

    // Метод для генерации элементов внутри заданных границ
    void generateElements(int numElements) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> distX(bounds.x_min, bounds.x_max);
        uniform_real_distribution<> distY(bounds.y_min, bounds.y_max);

        for (int i = 0; i < numElements; i++) {
            double x = distX(gen);
            double y = distY(gen);
            elements.push_back(make_pair(x, y)); // Добавляем элемент (точку) в вектор
        }
    }

    // Метод для записи элементов в файл
    void saveToFile(ofstream& outputFile, int classNumber) const {
        for (const auto& element : elements) {
            outputFile << element.first << " " << element.second << " " << classNumber << endl;
        }
    }

    

    // Метод для проверки корректности границ
    bool checkBounds() const {
        return bounds.isValid();
    }
};

// Функция для вычисления евклидова расстояния между двумя точками
double calculateDistance(const pair<double, double>& a, const pair<double, double>& b) {
    return sqrt(pow(a.first - b.first, 2) + pow(a.second - b.second, 2));
}

// Функция для простейшей расстановки центров кластеров
vector<pair<double, double>> selectClusterCenters(const vector<pair<double, double>>& elements, double h) {
    vector<pair<double, double>> centers; // Вектор для хранения центров кластеров

    if (elements.empty()) return centers; // Если выборка пуста, возвращаем пустой список

    // Первый элемент выборки становится первым центром
    centers.push_back(elements[0]);

    for (const auto& point : elements) {
        bool isFarEnough = true;

        // Проверяем расстояние от текущей точки до всех существующих центров
        for (const auto& center : centers) {
            if (calculateDistance(point, center) <= h) {
                isFarEnough = false;
                break;
            }
        }

        // Если точка далеко от всех центров, добавляем её как новый центр
        if (isFarEnough) {
            centers.push_back(point);
        }
    }

    return centers;
}

// Функция для расчета плотности распределения f(x)
double calculateDensity(const vector<pair<double, double>>& elements, const pair<double, double>& x, double h) {
    double density = 0.0;
    double h_squared = h * h;

    for (const auto& xi : elements) {
        double distance_squared = calculateDistance(x, xi);
        if (distance_squared < h_squared) {
            density += h_squared - distance_squared;
        }
    }

    return density / (h_squared); // Нормализация на h^2
}

// Алгоритм кластеризации с учетом плотности
vector<pair<double, double>> densityBasedClustering(const vector<pair<double, double>>& elements, double h) {
    vector<pair<double, double>> centers; // Вектор для хранения центров кластеров
    if (elements.empty()) return centers; // Если выборка пуста, возвращаем пустой список

    // Вычисление плотности для каждого элемента выборки
    vector<pair<double, double>> sortedElements = elements;
    vector<double> densities;
    for (const auto& x : elements) {
        densities.push_back(calculateDensity(elements, x, h));
    }

    // Сортировка элементов по убыванию плотности
    vector<size_t> indices(elements.size());
    iota(indices.begin(), indices.end(), 0); // Индексы элементов
    sort(indices.begin(), indices.end(), [&](size_t i, size_t j) {
        return densities[i] > densities[j];
    });

    // Перестановка элементов в порядке убывания плотности
    vector<pair<double, double>> reorderedElements;
    for (size_t idx : indices) {
        reorderedElements.push_back(elements[idx]);
    }

    // Применение простейшей расстановки центров кластеров
    centers.push_back(reorderedElements[0]); // Первый элемент становится центром
    for (const auto& point : reorderedElements) {
        bool isFarEnough = true;

        for (const auto& center : centers) {
            if (calculateDistance(point, center) <= h) {
                isFarEnough = false;
                break;
            }
        }

        if (isFarEnough) {
            centers.push_back(point);
        }
    }

    return centers;
}

// Алгоритм максиминного расстояния
vector<pair<double, double>> maximinClustering(const vector<pair<double, double>>& elements, double gamma) {
    vector<pair<double, double>> centers; // Вектор для хранения центров кластеров

    if (elements.empty()) return centers; // Если выборка пуста, возвращаем пустой список

    // Шаг 1: Выбираем первый центр как первый элемент выборки
    centers.push_back(elements[0]);

    // Шаг 2: Выбираем второй центр, максимально удаленный от первого
    size_t secondCenterIndex = 0;
    double maxDistance = -1.0;
    for (size_t i = 1; i < elements.size(); ++i) {
        double dist = calculateDistance(elements[i], centers[0]);
        if (dist > maxDistance) {
            maxDistance = dist;
            secondCenterIndex = i;
        }
    }
    centers.push_back(elements[secondCenterIndex]);

    // Шаг 3: Последующие центры выбираются по максимальному минимальному расстоянию
    double previousDispersion = maxDistance;
    while (true) {
        size_t nextCenterIndex = 0;
        double maxMinDistance = -1.0;

        for (size_t i = 0; i < elements.size(); ++i) {
            // Считаем минимальное расстояние от точки до уже выбранных центров
            double minDistance = std::numeric_limits<double>::max();
            for (const auto& center : centers) {
                double dist = calculateDistance(elements[i], center);
                if (dist < minDistance) {
                    minDistance = dist;
                }
            }

            // Сравниваем с максимальным минимальным расстоянием
            if (minDistance > maxMinDistance) {
                maxMinDistance = minDistance;
                nextCenterIndex = i;
            }
        }

        // Проверяем условие останова
        double currentDispersion = maxMinDistance;
        if (previousDispersion > 0 && currentDispersion / previousDispersion < gamma) {
            break;
        }

        // Добавляем новый центр и обновляем дисперсию
        centers.push_back(elements[nextCenterIndex]);
        previousDispersion = currentDispersion;
    }

    return centers;
}

// Алгоритм FOREL
vector<vector<pair<double, double>>> forelClustering(vector<pair<double, double>> elements, double r) {
    vector<vector<pair<double, double>>> clusters; // Вектор для хранения кластеров

    while (!elements.empty()) {
        // Шаг 1: Выбираем случайную точку как начальный центр окружности
        pair<double, double> center = elements[0];

        vector<pair<double, double>> cluster; // Текущий кластер
        while (true) {
            // Шаг 2: Находим точки, попавшие в окружность с радиусом r
            cluster.clear();
            for (const auto& point : elements) {
                if (calculateDistance(point, center) <= r) {
                    cluster.push_back(point);
                }
            }

            // Шаг 3: Вычисляем центр масс для найденных точек
            pair<double, double> newCenter = {0.0, 0.0};
            for (const auto& point : cluster) {
                newCenter.first += point.first;
                newCenter.second += point.second;
            }
            newCenter.first /= cluster.size();
            newCenter.second /= cluster.size();

            // Шаг 4: Проверяем условие сходимости
            if (calculateDistance(center, newCenter) < 1e-6) { // Малое значение для точности
                break;
            }

            // Перемещаем центр окружности
            center = newCenter;
        }

        // Сохраняем текущий кластер
        clusters.push_back(cluster);

        // Шаг 5: Удаляем точки кластера из набора данных
        vector<pair<double, double>> remainingElements;
        for (const auto& point : elements) {
            if (calculateDistance(point, center) > r) {
                remainingElements.push_back(point);
            }
        }
        elements = remainingElements;
    }

    return clusters;
}

// Алгоритм ISODATA с отладочным выводом
vector<vector<pair<double, double>>> isodataClusteringDebug(vector<pair<double, double>> elements, int K, int maxIterations, int minClusterSize, double mergeThreshold, double splitThreshold) {
    vector<pair<double, double>> centers(K); // Начальные центры кластеров
    vector<vector<pair<double, double>>> clusters(K); // Кластеры
    int iteration = 0;

    // Инициализация: выбираем случайные точки как начальные центры кластеров
    for (int i = 0; i < K; ++i) {
        centers[i] = elements[i % elements.size()];
    }

    cout << "Инициализация центров кластеров:" << endl;
    for (int i = 0; i < K; ++i) {
        cout << "Центр " << i + 1 << ": (" << centers[i].first << ", " << centers[i].second << ")" << endl;
    }

    while (iteration < maxIterations) {
        iteration++;
        cout << "\nИтерация " << iteration << ":" << endl;

        // Шаг 2: Назначение точек ближайшему кластеру
        for (auto& cluster : clusters) cluster.clear();
        for (const auto& point : elements) {
            int nearestCenterIndex = 0;
            double minDistance = calculateDistance(point, centers[0]);
            for (int i = 1; i < K; ++i) {
                double dist = calculateDistance(point, centers[i]);
                if (dist < minDistance) {
                    minDistance = dist;
                    nearestCenterIndex = i;
                }
            }
            clusters[nearestCenterIndex].push_back(point);
        }

        cout << "Назначение точек кластерам:" << endl;
        for (int i = 0; i < K; ++i) {
            cout << "Кластер " << i + 1 << ": ";
            for (const auto& point : clusters[i]) {
                cout << "(" << point.first << ", " << point.second << ") ";
            }
            cout << endl;
        }

        // Шаг 3: Обновление центров кластеров
        for (int i = 0; i < K; ++i) {
            if (clusters[i].empty()) continue;
            pair<double, double> newCenter = {0.0, 0.0};
            for (const auto& point : clusters[i]) {
                newCenter.first += point.first;
                newCenter.second += point.second;
            }
            newCenter.first /= clusters[i].size();
            newCenter.second /= clusters[i].size();
            centers[i] = newCenter;
        }

        cout << "Обновленные центры кластеров:" << endl;
        for (int i = 0; i < K; ++i) {
            cout << "Центр " << i + 1 << ": (" << centers[i].first << ", " << centers[i].second << ")" << endl;
        }

        // Шаг 4: Объединение кластеров
        for (int i = 0; i < K; ++i) {
            for (int j = i + 1; j < K; ++j) {
                if (calculateDistance(centers[i], centers[j]) < mergeThreshold) {
                    // Объединяем кластеры i и j
                    clusters[i].insert(clusters[i].end(), clusters[j].begin(), clusters[j].end());
                    clusters[j].clear();
                    centers[j] = {0.0, 0.0}; // "Пустой" центр
                    cout << "Объединены кластеры " << i + 1 << " и " << j + 1 << endl;
                }
            }
        }

        // Шаг 5: Разделение кластеров
        for (int i = 0; i < K; ++i) {
            if (clusters[i].size() > minClusterSize) {
                // Вычисляем дисперсию кластера
                double variance = 0.0;
                for (const auto& point : clusters[i]) {
                    variance += pow(calculateDistance(point, centers[i]), 2);
                }
                variance /= clusters[i].size();
                if (variance > splitThreshold) {
                    // Разделяем кластер i на два
                    pair<double, double> offset = {0.1, 0.1};
                    centers.push_back({centers[i].first + offset.first, centers[i].second + offset.second});
                    K++;
                    cout << "Разделен кластер " << i + 1 << " из-за высокой дисперсии." << endl;
                }
            }
        }

        // Условие остановки: если изменения незначительны
        if (iteration >= maxIterations) {
            cout << "Достигнуто максимальное количество итераций." << endl;
            break;
        }
    }

    return clusters;
}

// Добавляем пример вызова алгоритма ISODATA с отладкой в main
int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}, {2.0, 2.5}, {9.0, 9.5}};
    int K = 2; // Начальное количество кластеров
    int maxIterations = 100; // Максимальное количество итераций
    int minClusterSize = 2; // Минимальный размер кластера
    double mergeThreshold = 2.0; // Порог для объединения кластеров
    double splitThreshold = 1.5; // Порог для разделения кластеров
    auto isodataClusters = isodataClusteringDebug(elements, K, maxIterations, minClusterSize, mergeThreshold, splitThreshold);
    cout << "\nКластеры (алгоритм ISODATA с отладкой):" << endl;
    for (size_t i = 0; i < isodataClusters.size(); ++i) {
        if (!isodataClusters[i].empty()) {
            cout << "Кластер " << i + 1 << ":" << endl;
            for (const auto& point : isodataClusters[i]) {
                cout << "(" << point.first << ", " << point.second << ")" << endl;
            }
        }
    }
    return 0;
}
// Добавляем пример вызова алгоритма ISODATA в main
/*int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}, {2.0, 2.5}, {9.0, 9.5}};
    double h = 5.0;
    double gamma = 0.95;
    double r = 3.0;
    int K = 2; // Начальное количество кластеров
    int maxIterations = 100; // Максимальное количество итераций
    int minClusterSize = 2; // Минимальный размер кластера
    double mergeThreshold = 2.0; // Порог для объединения кластеров
    double splitThreshold = 1.5; // Порог для разделения кластеров
    auto densityCenters = densityBasedClustering(elements, h);
    cout << "Центры кластеров (с учетом плотности):" << endl;
    for (const auto& center : densityCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    auto maximinCenters = maximinClustering(elements, gamma);
    cout << "\nЦентры кластеров (максиминный алгоритм):" << endl;
    for (const auto& center : maximinCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    auto forelClusters = forelClustering(elements, r);
    cout << "\nКластеры (алгоритм FOREL):" << endl;
    for (size_t i = 0; i < forelClusters.size(); ++i) {
        cout << "Кластер " << i + 1 << ":" << endl;
        for (const auto& point : forelClusters[i]) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
    }
    auto isodataClusters = isodataClustering(elements, K, maxIterations, minClusterSize, mergeThreshold, splitThreshold);
    cout << "\nКластеры (алгоритм ISODATA):" << endl;
    for (size_t i = 0; i < isodataClusters.size(); ++i) {
        if (!isodataClusters[i].empty()) {
            cout << "Кластер " << i + 1 << ":" << endl;
            for (const auto& point : isodataClusters[i]) {
                cout << "(" << point.first << ", " << point.second << ")" << endl;
            }
        }
    }
    return 0;
} */

// Добавляем пример вызова алгоритма FOREL в main
/*int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}, {2.0, 2.5}, {9.0, 9.5}};
    double h = 5.0;
    double gamma = 0.95;
    double r = 3.0;
    auto densityCenters = densityBasedClustering(elements, h);
    cout << "Центры кластеров (с учетом плотности):" << endl;
    for (const auto& center : densityCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    auto maximinCenters = maximinClustering(elements, gamma);
    cout << "\nЦентры кластеров (максиминный алгоритм):" << endl;
    for (const auto& center : maximinCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    auto forelClusters = forelClustering(elements, r);
    cout << "\nКластеры (алгоритм FOREL):" << endl;
    for (size_t i = 0; i < forelClusters.size(); ++i) {
        cout << "Кластер " << i + 1 << ":" << endl;
        for (const auto& point : forelClusters[i]) {
            cout << "(" << point.first << ", " << point.second << ")" << endl;
        }
    }
    return 0;
}
*/
/*
// Добавляем пример вызова алгоритма максиминного расстояния в main
int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}};
    double h = 5.0;
    double gamma = 0.95;
    auto densityCenters = densityBasedClustering(elements, h);
    cout << "Центры кластеров (с учетом плотности):" << endl;
    for (const auto& center : densityCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    auto maximinCenters = maximinClustering(elements, gamma);
    cout << "\nЦентры кластеров (максиминный алгоритм):" << endl;
    for (const auto& center : maximinCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    return 0;
}*/

//Алгоритм, основанный на методе просеивания
/*int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}};
    double h = 5.0;
    auto densityCenters = densityBasedClustering(elements, h);
    cout << "Центры кластеров (с учетом плотности):" << endl;
    for (const auto& center : densityCenters) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    return 0;
}*/

//Обычная кластеризация
/*int main() {
    vector<pair<double, double>> elements = {{1.0, 2.0}, {3.0, 4.0}, {8.0, 9.0}, {10.0, 11.0}};
    double h = 10.0;
    auto centers = selectClusterCenters(elements, h);
    cout << "Выбранные центры кластеров:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.first << ", " << center.second << ")" << endl;
    }
    return 0;
}
*/


// Основная программа
/*int main() {
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

    // Запись всех данных в файл
    for (size_t i = 0; i < allClasses.size(); i++) {
        allClasses[i].saveToFile(outputFile, i + 1);
    }

    outputFile.close();
    cout << "Данные успешно записаны в файл output_data.txt!" << endl;

    return 0;
}
*/