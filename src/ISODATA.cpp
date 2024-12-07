#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <ctime>
#include <cstdlib>

using namespace std;

// Структура для представления точки
struct Point {
    double x, y;
    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

// Функция для вычисления евклидова расстояния между двумя точками
double calculateDistance(const Point& p1, const Point& p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Класс для реализации алгоритма ISODATA
class ISODATA {
public:
    ISODATA(vector<Point>& points, int maxIterations, double mergeThreshold, double splitThreshold)
        : points(points), maxIterations(maxIterations), mergeThreshold(mergeThreshold), splitThreshold(splitThreshold) {}

    vector<vector<Point>> run(int k) {
        // Шаг 1: Инициализация кластеров (рандомные центры)
        srand(time(0));
        vector<vector<Point>> clusters(k);
        vector<Point> centers(k);
        
        // Случайная инициализация центров
        for (int i = 0; i < k; ++i) {
            int randIndex = rand() % points.size();
            centers[i] = points[randIndex];
        }

        vector<int> assignments(points.size(), -1);  // Присваиваем точкам кластеров (индексы)

        int iterations = 0;
        bool converged = false;

        while (!converged && iterations < maxIterations) {
            converged = true;
            iterations++;

            // Шаг 2: Присваиваем точки к ближайшим центрам
            for (int i = 0; i < points.size(); ++i) {
                int closestCluster = -1;
                double minDistance = numeric_limits<double>::max();

                for (int j = 0; j < k; ++j) {
                    double dist = calculateDistance(points[i], centers[j]);
                    if (dist < minDistance) {
                        minDistance = dist;
                        closestCluster = j;
                    }
                }

                if (assignments[i] != closestCluster) {
                    assignments[i] = closestCluster;
                    converged = false;
                }
            }

            // Шаг 3: Пересчитываем центры кластеров
            vector<Point> newCenters(k, Point(0, 0));
            vector<int> clusterSizes(k, 0);

            for (int i = 0; i < points.size(); ++i) {
                int clusterIdx = assignments[i];
                newCenters[clusterIdx].x += points[i].x;
                newCenters[clusterIdx].y += points[i].y;
                clusterSizes[clusterIdx]++;
            }

            for (int i = 0; i < k; ++i) {
                if (clusterSizes[i] > 0) {
                    newCenters[i].x /= clusterSizes[i];
                    newCenters[i].y /= clusterSizes[i];
                }
            }

            // Проверяем сходимость (если центры не изменились)
            for (int i = 0; i < k; ++i) {
                if (calculateDistance(centers[i], newCenters[i]) > 0.0001) {
                    converged = false;
                }
            }

            centers = newCenters;

            // Шаг 4: Мerging and splitting кластеров
            mergeClusters(centers);
            splitClusters(centers);
        }

        // Разбиваем точки на кластеры
        vector<vector<Point>> finalClusters(k);
        for (int i = 0; i < points.size(); ++i) {
            finalClusters[assignments[i]].push_back(points[i]);
        }

        return finalClusters;
    }

private:
    vector<Point>& points;
    int maxIterations;
    double mergeThreshold;
    double splitThreshold;

    // Функция для слияния кластеров
    void mergeClusters(vector<Point>& centers) {
        for (int i = 0; i < centers.size(); ++i) {
            for (int j = i + 1; j < centers.size(); ++j) {
                if (calculateDistance(centers[i], centers[j]) < mergeThreshold) {
                    // Если два кластера слишком близки, сливаем их
                    centers[i].x = (centers[i].x + centers[j].x) / 2;
                    centers[i].y = (centers[i].y + centers[j].y) / 2;
                    centers[j] = centers[i];  // Переназначаем центр второго кластера на первый
                }
            }
        }
    }

    // Функция для разделения кластеров
    void splitClusters(vector<Point>& centers) {
        for (int i = 0; i < centers.size(); ++i) {
            // Если кластер слишком расплывчатый, то разделяем его
            double maxDist = 0;
            Point farthestPoint(0, 0);
            for (const auto& p : points) {
                double dist = calculateDistance(p, centers[i]);
                if (dist > maxDist) {
                    maxDist = dist;
                    farthestPoint = p;
                }
            }

            if (maxDist > splitThreshold) {
                // Разделяем кластер на два
                Point newCenter1((centers[i].x + farthestPoint.x) / 2, (centers[i].y + farthestPoint.y) / 2);
                Point newCenter2(farthestPoint.x, farthestPoint.y);
                centers.push_back(newCenter1);
                centers.push_back(newCenter2);
            }
        }
    }
};

// Функция для визуализации результатов
void visualizeClusters(const vector<vector<Point>>& clusters) {
    for (size_t i = 0; i < clusters.size(); ++i) {
        for (const auto& p : clusters[i]) {
            cout << "Cluster " << i + 1 << ": (" << p.x << ", " << p.y << ")\n";
        }
    }
}

int main() {
    // Пример данных
    vector<Point> points = {
        Point(1.0, 2.0), Point(1.5, 2.5), Point(2.0, 3.0),
        Point(8.0, 8.0), Point(8.5, 8.5), Point(9.0, 9.0),
        Point(5.0, 5.0), Point(5.5, 5.5), Point(6.0, 6.0)
    };

    // Параметры
    int k = 2;  // Начальное количество кластеров
    int maxIterations = 10;
    double mergeThreshold = 1.5;  // Порог для слияния кластеров
    double splitThreshold = 4.0;  // Порог для разделения кластеров

    // Запуск алгоритма ISODATA
    ISODATA isodata(points, maxIterations, mergeThreshold, splitThreshold);
    vector<vector<Point>> clusters = isodata.run(k);

    // Визуализация результатов
    visualizeClusters(clusters);

    return 0;
}
