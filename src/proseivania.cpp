#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <limits>

using namespace std;

// Структура для представления точки в двумерном пространстве
struct Point {
    double x, y;
    double density; 
    
    Point(double x_val, double y_val) : x(x_val), y(y_val), density(0) {}
};

// Функция для вычисления евклидова расстояния между двумя точками
double calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Алгоритм для вычисления плотности точек в выборке
void calculateDensity(vector<Point>& elements, double r) {
    for (auto& point : elements) {
        point.density = 0;
        // Считаем количество точек в пределах радиуса r от текущей точки
        for (const auto& other : elements) {
            if (calculateDistance(point, other) <= r) {
                point.density += 1;
            }
        }

        point.density -= 1;
    }
}

// Алгоритм для выбора центров кластеров на основе плотности
vector<Point> selectClusterCenters(vector<Point>& elements, int numClusters, double r) {

    calculateDensity(elements, r);

    // Сортируем точки по плотности (по убыванию)
    sort(elements.begin(), elements.end(), [](const Point& p1, const Point& p2) {
        return p1.density > p2.density;
    });

    // Выбираем первые numClusters точек с наибольшей плотностью
    vector<Point> centers;
    for (int i = 0; i < numClusters; ++i) {
        centers.push_back(elements[i]);
    }

    return centers;
}

int main() {
    // Пример выборки точек
    vector<Point> elements = {
        Point(1.0, 2.0), Point(2.0, 3.0), Point(3.0, 4.0),
        Point(5.0, 6.0), Point(8.0, 8.0), Point(9.0, 10.0)
    };

    double r = 2.0;  // Радиус для вычисления плотности
    int numClusters = 3;  // Количество кластеров

    // Получаем центры кластеров
    vector<Point> centers = selectClusterCenters(elements, numClusters, r);

    // Выводим выбранные центры
    cout << "Selected cluster centers:" << endl;
    for (const auto& center : centers) {
        cout << "(" << center.x << ", " << center.y << ") with density: " << center.density << endl;
    }

    return 0;
}
