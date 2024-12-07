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

// Алгоритм для выбора центров кластеров с порогом h
vector<Point> selectClusterCenters(const vector<Point>& elements, double h, int numClusters) {
    vector<Point> centers;


    if (numClusters >= elements.size()) {
        return elements;
    }


    centers.push_back(elements[0]);

    // Ищем следующие центры
    while (centers.size() < numClusters) {
        double maxMinDist = -1;
        int selectedIndex = -1;

        // Для каждой точки выбираем минимальное расстояние до всех уже выбранных центров
        for (int i = 0; i < elements.size(); ++i) {
            Point point = elements[i];

            // Проверка, является ли эта точка уже центром
            bool isCenter = false;
            for (const auto& center : centers) {
                if (point.x == center.x && point.y == center.y) {
                    isCenter = true;
                    break;
                }
            }

            if (isCenter) continue; // Если точка уже является центром, пропускаем её

            // Находим минимальное расстояние до всех выбранных центров
            double minDist = numeric_limits<double>::max();
            for (const auto& center : centers) {
                double dist = calculateDistance(point, center);
                minDist = min(minDist, dist);
            }

            // Если минимальное расстояние больше порога h, выбираем эту точку
            if (minDist > h && minDist > maxMinDist) {
                maxMinDist = minDist;
                selectedIndex = i;
            }
        }

        // Добавляем выбранный центр в список центров
        if (selectedIndex != -1) {
            centers.push_back(elements[selectedIndex]);
        } else {
            // Если не удалось выбрать новый центр, выходим (это может быть связано с нехваткой точек)
            break;
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

    double h = 2.0;  // Порог расстояния
    int numClusters = 3;  // Количество кластеров

    // Получаем центры кластеров
    vector<Point> centers = selectClusterCenters(elements, h, numClusters);

    // Выводим выбранные центры
    cout << "Selected cluster centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ")" << endl;
    }

    return 0;
}
