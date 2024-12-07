#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <algorithm>

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

// Алгоритм FOREL для кластеризации
vector<vector<Point>> FOREL(const vector<Point>& points, double radius, int maxIterations) {
    vector<vector<Point>> clusters;
    vector<bool> visited(points.size(), false);  // Массив, чтобы отслеживать посещенные точки

    for (int i = 0; i < points.size(); ++i) {
        if (visited[i]) continue;  // Пропускаем уже посещенные точки

        vector<Point> cluster;  
        vector<int> toVisit;  
        toVisit.push_back(i);  

        // Строим кластер вокруг текущей точки
        while (!toVisit.empty()) {
            int idx = toVisit.back(); toVisit.pop_back();
            if (visited[idx]) continue;  // Если точка уже была обработана, пропускаем

            visited[idx] = true;  
            cluster.push_back(points[idx]);  

            // Ищем соседей текущей точки
            for (int j = 0; j < points.size(); ++j) {
                if (!visited[j] && calculateDistance(points[idx], points[j]) <= radius) {
                    toVisit.push_back(j);  // Добавляем точку в список для проверки
                }
            }
        }

        // Если кластер не пустой, добавляем его в список кластеров
        if (!cluster.empty()) {
            clusters.push_back(cluster);
        }

        // Если мы достигли максимального числа итераций, прекращаем
        if (clusters.size() >= maxIterations) {
            break;
        }
    }

    return clusters;
}

int main() {
    // Пример данных (выборка из точек)
    vector<Point> points = {
        Point(1.0, 2.0), Point(1.5, 2.5), Point(2.0, 3.0),
        Point(8.0, 8.0), Point(8.5, 8.5), Point(9.0, 9.0),
        Point(5.0, 5.0), Point(5.5, 5.5), Point(6.0, 6.0)
    };

    double radius = 1.5;  // Радиус для кластеризации
    int maxIterations = 3;  // Максимальное количество кластеров

    // Выполняем кластеризацию по алгоритму FOREL
    vector<vector<Point>> clusters = FOREL(points, radius, maxIterations);

    // Выводим результаты кластеризации
    cout << "Clustering results:" << endl;
    for (int i = 0; i < clusters.size(); ++i) {
        cout << "Cluster " << i + 1 << ":" << endl;
        for (const auto& p : clusters[i]) {
            cout << "(" << p.x << ", " << p.y << ") ";
        }
        cout << endl;
    }

    return 0;
}
