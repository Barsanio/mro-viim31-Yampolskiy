#include <iostream>
#include <vector>
#include <fstream>
#include <random>
#include <cmath>
#include <limits>

using namespace std;

// Структура для хранения границ (ограничений) каждого класса
struct Bounds {
    double x_min, x_max, y_min, y_max, z_min, z_max;

    // Метод проверки корректности ограничений
    bool isValid() const {
        return x_min <= x_max && y_min <= y_max && z_min <= z_max;
    }
};

// Класс для управления элементами и границами
class ClassElements {
private:
    Bounds bounds; // Границы для класса
    vector<vector<double>> elements; // Вектор для хранения элементов (точек)

public:
    // Конструктор, принимающий границы
    ClassElements(Bounds b) : bounds(b) {}

    // Метод для генерации элементов внутри заданных границ
    void generateElements(int numElements) {
        random_device rd;
        mt19937 gen(rd());
        uniform_real_distribution<> distX(bounds.x_min, bounds.x_max);
        uniform_real_distribution<> distY(bounds.y_min, bounds.y_max);
        uniform_real_distribution<> distZ(bounds.z_min, bounds.z_max);

        for (int i = 0; i < numElements; i++) {
            double x = distX(gen);
            double y = distY(gen);
            double z = distZ(gen);
            elements.push_back({ x, y, z, 1.0 }); // Добавляем 1.0 для учета смещения (bias)
        }
    }

    // Метод для записи элементов в файл
    void saveToFile(ofstream& outputFile, int classNumber) const {
        for (const auto& element : elements) {
            outputFile << element[0] << " " << element[1] << " " << element[2] << " " << classNumber << endl;
        }
    }

    // Метод для получения элементов
    const vector<vector<double>>& getElements() const {
        return elements;
    }

    // Метод для проверки корректности границ
    bool checkBounds() const {
        return bounds.isValid();
    }
};

// Функция для чтения данных из файла
void readData(const string& filename, vector<vector<double>>& data, vector<int>& labels) {
    ifstream inputFile(filename);
    if (!inputFile) {
        cerr << "Ошибка: Не удалось открыть файл " << filename << endl;
        return;
    }

    double x1, x2, x3;
    int label;
    while (inputFile >> x1 >> x2 >> x3 >> label) {
        data.push_back({ x1, x2, x3, 1.0 }); // Добавляем 1.0 для учета смещения (bias)
        labels.push_back(label);
    }
    inputFile.close();
}

// Функция для транспонирования матрицы
vector<vector<double>> transpose(const vector<vector<double>>& A) {
    size_t rows = A.size();
    size_t cols = A[0].size();
    vector<vector<double>> result(cols, vector<double>(rows));
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result[j][i] = A[i][j];
    return result;
}

// Функция для умножения матриц
vector<vector<double>> multiply(const vector<vector<double>>& A, const vector<vector<double>>& B) {
    size_t rows = A.size();
    size_t cols = B[0].size();
    size_t inner = B.size();
    vector<vector<double>> result(rows, vector<double>(cols, 0.0));
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            for (size_t k = 0; k < inner; ++k)
                result[i][j] += A[i][k] * B[k][j];
    return result;
}

// Функция для умножения матрицы на вектор
vector<double> multiply(const vector<vector<double>>& A, const vector<double>& x) {
    size_t rows = A.size();
    size_t cols = A[0].size();
    vector<double> result(rows, 0.0);
    for (size_t i = 0; i < rows; ++i)
        for (size_t j = 0; j < cols; ++j)
            result[i] += A[i][j] * x[j];
    return result;
}

// Функция для решения системы линейных уравнений методом Гаусса
vector<double> gaussElimination(vector<vector<double>> A, vector<double> b) {
    int n = A.size();

    // Прямой ход
    for (int k = 0; k < n; ++k) {
        // Поиск максимального элемента для частичного выбора главного элемента
        double maxElem = abs(A[k][k]);
        int maxRow = k;
        for (int i = k + 1; i < n; ++i) {
            if (abs(A[i][k]) > maxElem) {
                maxElem = abs(A[i][k]);
                maxRow = i;
            }
        }

        // Перестановка строк
        if (maxRow != k) {
            swap(A[k], A[maxRow]);
            swap(b[k], b[maxRow]);
        }

        // Проверка на нулевой элемент на диагонали
        if (abs(A[k][k]) < numeric_limits<double>::epsilon()) {
            cerr << "Ошибка: Матрица вырождена." << endl;
            return vector<double>();
        }

        // Приведение к верхнетреугольному виду
        for (int i = k + 1; i < n; ++i) {
            double factor = A[i][k] / A[k][k];
            for (int j = k; j < n; ++j) {
                A[i][j] -= factor * A[k][j];
            }
            b[i] -= factor * b[k];
        }
    }

    // Обратный ход
    vector<double> x(n);
    for (int i = n - 1; i >= 0; --i) {
        double sum = b[i];
        for (int j = i + 1; j < n; ++j) {
            sum -= A[i][j] * x[j];
        }
        x[i] = sum / A[i][i];
    }

    return x;
}

// Функция для решения системы нормальных уравнений методом наименьших квадратов
vector<double> solveLeastSquares(const vector<vector<double>>& A, const vector<double>& b) {
    vector<vector<double>> At = transpose(A);
    vector<vector<double>> AtA = multiply(At, A);
    vector<double> Atb = multiply(At, b);

    // Решаем систему AtA * x = Atb
    vector<double> x = gaussElimination(AtA, Atb);
    return x;
}

// Функция реализации алгоритма Хо-Кашьяпа
vector<double> hoKashyap(const vector<vector<double>>& data, const vector<int>& labels, double eta = 0.5, int max_iter = 10000, double tol = 1e-5) {
    int n_samples = data.size();
    int n_features = data[0].size();

    vector<vector<double>> A(n_samples, vector<double>(n_features));
    for (int i = 0; i < n_samples; ++i) {
        for (int j = 0; j < n_features; ++j) {
            A[i][j] = labels[i] * data[i][j];
        }
    }

    vector<double> b(n_samples, 1.0);
    vector<double> w(n_features, 0.0);

    for (int iter = 0; iter < max_iter; ++iter) {
        // Решаем уравнение Aw = b (методом наименьших квадратов)
        w = solveLeastSquares(A, b);

        // Вычисляем ошибку e = Aw - b
        vector<double> e(n_samples);
        vector<double> Aw = multiply(A, w);
        for (int i = 0; i < n_samples; ++i) {
            e[i] = Aw[i] - b[i];
        }

        // Проверяем условие сходимости
        bool converged = true;
        for (int i = 0; i < n_samples; ++i) {
            if (e[i] < -tol) {
                converged = false;
                break;
            }
        }
        if (converged) {
            cout << "Сошлось за " << iter << " итераций" << endl;
            return w;
        }

        // Обновляем b
        for (int i = 0; i < n_samples; ++i) {
            b[i] = b[i] + eta * (e[i] + abs(e[i]));
        }
    }

    cout << "Не сошлось за максимальное число итераций" << endl;
    return w;
}

// Основная программа
int main() {
    int numClasses;
    cout << "Введите количество классов: ";
    cin >> numClasses;

    if (numClasses != 2) {
        cerr << "Ошибка" << endl;
        return 1;
    }

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

        double x_min, x_max, y_min, y_max, z_min, z_max;
        Bounds bounds;
        do {
            cout << "Ограничения (x_min, x_max, y_min, y_max, z_min, z_max): ";
            cin >> bounds.x_min >> bounds.x_max >> bounds.y_min >> bounds.y_max >> bounds.z_min >> bounds.z_max;

            if (bounds.isValid()) {
                // Создаём объект класса с корректными границами и добавляем в вектор
                ClassElements newClass(bounds);
                newClass.generateElements(numElements);
                allClasses.push_back(newClass);
                break;
            } else {
                cout << "Ошибка: x_min <= x_max, y_min <= y_max и z_min <= z_max. Попробуйте снова.\n";
            }
        } while (true);
    }

    // Запись всех данных в файл
    for (size_t i = 0; i < allClasses.size(); i++) {
        allClasses[i].saveToFile(outputFile, i + 1);
    }

    outputFile.close();
    cout << "Данные успешно записаны в файл output_data.txt!" << endl;

    // Читаем данные обратно
    vector<vector<double>> data;
    vector<int> labels;
    readData("output_data.txt", data, labels);

    // Преобразуем метки классов в +1 и -1
    int class1 = labels[0];
    int class2 = -1;
    for (size_t i = 0; i < labels.size(); ++i) {
        if (labels[i] != class1) {
            class2 = labels[i];
            break;
        }
    }

    for (size_t i = 0; i < labels.size(); ++i) {
        if (labels[i] == class1) {
            labels[i] = 1;
        } else if (labels[i] == class2) {
            labels[i] = -1;
        } else {
            cerr << "Ошибка" << endl;
            return 1;
        }
    }

    // Запускаем алгоритм Хо-Кашьяпа
    vector<double> w = hoKashyap(data, labels);

    // Сохраняем веса в файл
    ofstream wFile("weights.txt");
    if (!wFile) {
        cerr << "Ошибка: Не удалось создать файл weights.txt" << endl;
        return 1;
    }
    for (double weight : w) {
        wFile << weight << " ";
    }
    wFile << endl;
    wFile.close();

    cout << "Веса сохранены в weights.txt" << endl;

    return 0;
}
