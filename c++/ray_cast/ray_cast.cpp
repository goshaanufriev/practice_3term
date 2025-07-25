#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <ctime>

struct Point
{
    double x, y;
};

struct Cell {
    std::vector<Point> vertices;
};

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T> vec)
{
    out << vec.size() << "\n";
    if (vec.size() != 0)
    {
        for (size_t i = 0; i < vec.size() - 1; ++i)
        {
            out << vec[i] << (i == vec.size() - 1 ? "" : "\n");
        }
        out << vec.back() << "\n";
    }
    return out;
}

bool isIntersecting(const Point& p, const Point& a, const Point& b) {
    Point low = a, high = b;
    if (low.y > high.y) std::swap(low, high);
    if (p.y < low.y || p.y > high.y) return false;
    if (fabs(low.y - high.y) < 1E-10) return false; // Горизонтальный отрезок
    double intersectX = low.x + (p.y - low.y) * (high.x - low.x) / (high.y - low.y);
    return p.x < intersectX;
}

// Проверка принадлежности точки многоугольнику методом лучей(Ray-Casting)
bool ray_cast(const Point& point, const Cell& cell) {
    int intersections = 0;
    size_t n = cell.vertices.size();
    for (size_t i = 0; i < n; ++i) {
        Point a = cell.vertices[i];
        Point b = cell.vertices[(i + 1) % n];
        if (isIntersecting(point, a, b)) {
            intersections++;
        }
    }
    return intersections % 2 == 1; // Нечётное количество пересечений
}

std::vector<int> res2(const std::vector<Point>& test, const std::vector<Cell>& cells)
{
    std::vector<int> ans(test.size());
    for (int i = 0; i < test.size(); ++i)
    {
        int flag = 0;
        for (int j = 0; j < cells.size(); ++j)
        {
            bool s = ray_cast(test[i], cells[j]);
            if (s)
            {
                flag = 1;
                ans[i] = j+1;
                break;
            }
        }
        if (flag == 0)
            ans.push_back(-1);
    }
    return ans;
}

int main()
{

    setlocale(LC_ALL, "RU");

    std::ifstream fin("10k.txt");
    if (!fin)
    {
        std::cerr << "Не удалось открыть файл";
        return 1;
    }
    int npts;
    if (!(fin >> npts) || npts <= 0)
    {
        std::cerr << "Ошибка с числом точек";
        return 1;
    }

    std::vector<Point> pts(npts);
    for (int i = 0; i < npts; ++i)
    {
        if (!(fin >> pts[i].x >> pts[i].y))
        {
            std::cerr << "Ошибка считывания координат точки";
            return 1;
        }
    }
    int ncells;
    if (!(fin >> ncells) || ncells <= 0)
    {
        std::cerr << "Ошибка с числом ячеек";
        return 1;
    }
    //double*** cells = new double** [ncells];
    std::vector<Cell> cells(ncells);
    for (size_t i = 0; i < ncells; ++i)
    {
        int n;
        if (!(fin >> n) || n <= 0)
        {
            std::cerr << "Неизвестная фигура";
            return 1;
        }
        cells[i].vertices.resize(n);
        for (size_t j = 0; j < n; ++j)
        {
            size_t vertexIndex;
            if (!(fin >> vertexIndex) || vertexIndex < 0 || vertexIndex > npts)
            {
                std::cerr << "Некорректный индекс вершины: " << vertexIndex << std::endl;
                return 1;
            }
            cells[i].vertices[j] = pts[vertexIndex - 1];
        }
    }
    /*fin.close();

    std::ifstream fin1("testdata3.txt");*/

    int ntest;
    if (!(fin >> ntest) || ntest <= 0)
    {
        std::cerr << "Ошибка с числом тестовых данных";
        return 1;
    }
    std::vector<Point> testdata(ntest);
    for (int i = 0; i < ntest; ++i)
    {
        if (!(fin >> testdata[i].x >> testdata[i].y))
        {
            std::cerr << "Ошибка считывания координат точки";
            return 1;
        }
    }

    //std::vector<int> per = res2(testdata, cells);
    std::cout << testdata.size() << " " << cells.size() << "\n";
    std::vector<int> ans(testdata.size());
    //auto st = std::chrono::high_resolution_clock::now();
    auto st = clock();
    for (int i = 0; i < ans.size(); ++i)
    {
        
        int flag = 0;
        for (int j = 0; j < cells.size(); ++j)
        {
            bool s = ray_cast(testdata[i], cells[j]);
            if (s)
            {
                flag = 1;
                ans[i] = j + 1;
                //break;
            }
        }
        if (flag == 0)
            ans[i] = -1;
    }
    auto finish = clock();
    std::cout << "Time = " << (double)(finish - st) / 1000 << "\n";
    //auto finish = std::chrono::high_resolution_clock::now();
    //std::cout << std::chrono::duration_cast<std::chrono::microseconds>(finish - st).count() << "\n";
    //std::cout << per
    std::ofstream file1("res1.txt");
    if (!file1)
    {
        std::cerr << "Не удалось открыть файл для записи результатов." << std::endl;
        return 1;
    }
    file1 << ans;
}


