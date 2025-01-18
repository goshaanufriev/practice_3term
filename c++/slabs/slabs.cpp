#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <map>
#include <set>
#include <chrono>
#include "structures_4.h"
#include "slab_4.h"


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

static void perebor(const std::vector<Point>& testdata, const std::vector<Cell>& cells)
{
    std::vector<int> per = res2(testdata, cells);
    std::cout << per;
    std::ofstream file1("res1.txt");
    if (!file1)
    {
        std::cerr << "Не удалось открыть файл для записи результатов." << std::endl;
        return;
    }
    file1 << per;
}

int main()
{
    setlocale(LC_ALL, "RU");

    std::ifstream fin("problem.txt");
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
    fin.close();
    
    std::ifstream fin1("testdata3.txt");

    int ntest;
    if (!(fin1 >> ntest) || ntest <= 0)
    {
        std::cerr << "Ошибка с числом тестовых данных";
        return 1;
    }
    std::vector<Point> testdata(ntest);
    for (int i = 0; i < ntest; ++i)
    {
        if (!(fin1 >> testdata[i].x >> testdata[i].y))
        {
            std::cerr << "Ошибка считывания координат точки";
            return 1;
        }
    }

    //auto st = std::chrono::high_resolution_clock::now();
    //std::vector<int> per = res2(testdata, cells);
    //auto finish = std::chrono::high_resolution_clock::now();
    //std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - st).count() << "\n";
    ////std::cout << per;
    //std::ofstream file1("res1.txt");
    //if (!file1)
    //{
    //    std::cerr << "Не удалось открыть файл для записи результатов." << std::endl;
    //    return 1;
    //}
    //file1 << per;

    auto st = std::chrono::high_resolution_clock::now();
    std::vector<double> setxslab;
    xslab(pts, setxslab);
    std::vector<std::vector<double>> slabs = slabsBoundaries(setxslab);
    std::map<int, std::vector<int>> cellsInSlabs;
    std::vector<std::vector<double>> rangeTable(ncells, std::vector<double>(2));
    for (size_t i = 0; i < ncells; ++i)
    {
        rangeTable[i] = xRange(cells[i]);
    }
    std::vector<std::vector<int>> cellsRange(ncells, std::vector<int>(2));
    for (size_t i = 0; i < ncells; ++i)
    {
        /*auto it1 = std::find_if(begin(setxslab), end(setxslab), [&](double x) {
            return abs(x - rangeTable[i][0]) < 1E-10;
            });
        auto it2 = std::find_if(begin(setxslab), end(setxslab), [&](double x) {
            return abs(x - rangeTable[i][1]) < 1E-10;
            });*/
        cellsRange[i][0] = std::lower_bound(begin(setxslab), end(setxslab), rangeTable[i][0]) - begin(setxslab);
        cellsRange[i][1] = std::lower_bound(begin(setxslab), end(setxslab), rangeTable[i][1]) - begin(setxslab) - 1;
    }
    for (size_t i = 0; i < ncells; ++i)
    {
        for (size_t j = cellsRange[i][0]; j <= cellsRange[i][1]; ++j)
        {
            cellsInSlabs[j].push_back(i);
        }
    }
    for (size_t i = 0; i < cellsInSlabs.size(); ++i)
    {
        std::sort(begin(cellsInSlabs[i]), end(cellsInSlabs[i]), [&](int a, int b) {
            std::vector<double> ay = MyMin(cells[a], slabs[i]);
            std::vector<double> by = MyMin(cells[b], slabs[i]);
            return *std::min_element(begin(ay),end(ay)) <
                *std::min_element(begin(by), end(by));
            });
    }
    std::vector<int> res(testdata.size());
    for (size_t i = 0; i < res.size(); ++i)
    {
        int slabInd = binSearch(setxslab, testdata[i].x);
        std::vector<double> minYs(cellsInSlabs[slabInd].size());
        std::vector<std::vector<double>> ally(minYs.size());
        for (size_t j = 0; j < ally.size(); ++j)
        {
            ally[j] = MyMin(cells[cellsInSlabs[slabInd][j]], slabs[slabInd]);
            minYs[j] = *std::min_element(begin(ally[j]),end(ally[j]));
        }
        size_t cellInd = binSearch(minYs, testdata[i].y);
        std::vector<double> y1s = {ally[cellInd][0],ally[cellInd][2] };
        std::vector<double> y2s = {ally[cellInd][1],ally[cellInd][3] };
        double y1 = *std::min_element(begin(y1s),end(y1s));
        double y2 = *std::min_element(begin(y2s),end(y2s));
        double y = (y2-y1) * (testdata[i].x - slabs[slabInd][0]) / (slabs[slabInd][1] - slabs[slabInd][0]) + y1;
        if(testdata[i].y > y)
        {
            res[i] = cellsInSlabs[slabInd][cellInd] + 1;
        }
        else if (testdata[i].y < y)
        {
            res[i] = cellsInSlabs[slabInd][cellInd - 1] + 1;
        }
        
    }
    auto finish = std::chrono::high_resolution_clock::now();
    std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(finish - st).count() << "\n";

    //std::cout << res << "\n";
    return 0;
}
