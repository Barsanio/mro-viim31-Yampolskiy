#include <iostream>
#include <vector>
#include <cmath>
#include <limits>

using namespace std;

// Структура для представления точки в двумерном пространстве
struct Point {
    double x, y;
    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

// Функция для вычисления евклидова расстояния между двумя точками
double calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Алгоритм максиминного расстояния
vector<Point> maximinClustering(const vector<Point>& elements, int numClusters) {
    vector<Point> centers;

    if (numClusters >= elements.size()) {
        return elements;
    }

    centers.push_back(elements[0]);

    while (centers.size() < numClusters) {
        double maxMinDistance = -1;
        int selectedIndex = -1;

        // Для каждой точки в выборке
        for (int i = 0; i < elements.size(); ++i) {
            const Point& point = elements[i];
            // Проверяем, является ли эта точка уже центром
            bool isCenter = false;
            for (const auto& center : centers) {
                if (point.x == center.x && point.y == center.y) {
                    isCenter = true;
                    break;
                }
            }
            if (isCenter) continue; // Пропускаем уже выбранные центры

            // Вычисляем минимальное расстояние от этой точки до всех уже выбранных центров
            double minDistance = numeric_limits<double>::max();
            for (const auto& center : centers) {
                double distance = calculateDistance(point, center);
                minDistance = min(minDistance, distance);
            }

            // Если минимальное расстояние больше текущего максимального минимального расстояния, обновляем выбор
            if (minDistance > maxMinDistance) {
                maxMinDistance = minDistance;
                selectedIndex = i;
            }
        }

        // Добавляем точку с максимальным минимальным расстоянием в центры
        if (selectedIndex != -1) {
            centers.push_back(elements[selectedIndex]);
        } else {
            break; // Если не удалось найти подходящую точку, завершаем
        }
    }

    return centers;
}

int main() {
    // Пример выборки точек
    vector<Point> elements = {
        Point(1.0, 2.0), Point(2.0, 3.0), Point(3.0, 4.0),
        Point(5.0, 6.0), Point(8.0, 8.0), Point(9.0, 10.0)
    };

    int numClusters = 3; // Количество кластеров

    // Получаем центры кластеров
    vector<Point> centers = maximinClustering(elements, numClusters);

    // Выводим выбранные центры
    cout << "Selected cluster centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ")" << endl;
    }

    return 0;
}
