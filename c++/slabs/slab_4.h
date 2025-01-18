#pragma once

std::vector<double>& xslab(const std::vector<Point>& pts, std::vector<double>& arr)
{
    /*std::vector<double> allx(pts.size());
	for (size_t i = 0; i < pts.size(); ++i)
	{
		allx[i]=pts[i].x;
	}
    std::sort(begin(allx), end(allx));
    arr.push_back(allx[0]);
    for (size_t i = 0; i < allx.size() - 1; ++i)
    {
        if (abs(allx[i] - allx[i + 1]) > 1E-10)
        {
            arr.push_back(allx[i + 1]);
        }
    }
	return arr;*/
    std::set<double> unique_x;
    for (const auto& pt : pts)
        unique_x.insert(pt.x);

    arr.assign(unique_x.begin(), unique_x.end());
    return arr;
}

std::vector<std::vector<double>> slabsBoundaries(const std::vector<double>& xslabs)
{
    std::vector<std::vector<double>> boundaries(xslabs.size() - 1, std::vector<double>(2));
    for (size_t i = 0; i < boundaries.size(); ++i)
    {
        boundaries[i][0] = xslabs[i];
        boundaries[i][1] = xslabs[i+1];
    }
    return boundaries;
}



size_t binSearch(const std::vector<double>& arr, const double& pt)
{
	/*size_t lid = 0, rid = arr.size() - 1;
	while (lid <= rid)
	{
        size_t c = lid + (rid - lid) / 2;
        if (abs(arr[c] - pt) < 1E-10)
            return c;
		if (arr[c] < pt)
		{
			lid = c+1;
		}
		else if (arr[c] > pt)
		{
			rid = c - 1;
		}
	}
	return lid-1;*/
    auto it = std::lower_bound(arr.begin(), arr.end(), pt);
    if (it == arr.end() || abs(*it - pt) > 1E-10)
        return it - arr.begin() - 1;
    return it - arr.begin();
}

std::vector<double> xRange(const Cell& fig)
{
    std::vector<double> res(2);
    std::vector<double> xs(fig.vertices.size());
    for (size_t i = 0; i < xs.size(); ++i)
    {
        xs[i] = fig.vertices[i].x;
    }
    res[0] = *std::min_element(begin(xs), end(xs));
    res[1] = *std::max_element(begin(xs), end(xs));
    return res;
}

std::vector<double> MyMin(const Cell& cell, const std::vector<double>& slab)
{
    std::vector<std::vector<size_t>> pair;
    std::vector<double> y;
    for (size_t i = 0; i < cell.vertices.size(); ++i)
    {
        if (std::min(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) < slab[0]
            && (std::max(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) > slab[1] ||
                abs(std::max(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) - slab[1])<1E-10) ||
            std::max(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) > slab[1] && (
                std::min(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) < slab[0] ||
                abs(std::min(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) - slab[0]) < 1E-10) ||
            abs(std::min(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) - slab[0]) < 1E-10 && 
            abs(std::max(cell.vertices[i].x, cell.vertices[(i + 1) % (cell.vertices.size())].x) - slab[1]) < 1E-10)
        {
            pair.push_back({ i,(i + 1) % cell.vertices.size() });
        }
    }
    for (size_t i = 0; i < pair.size(); ++i)
    {
        for (size_t j = 0; j < 2; ++j)
        {
            y.push_back((slab[j]-cell.vertices[pair[i][0]].x)*(cell.vertices[pair[i][1]].y - cell.vertices[pair[i][0]].y)/
                (cell.vertices[pair[i][1]].x - cell.vertices[pair[i][0]].x)+cell.vertices[pair[i][0]].y);
        }
    }
    return y;
}


// Проверка пересечения горизонтального луча с отрезком
bool isIntersecting(const Point& p, const Point& a, const Point& b) {
    Point low = a, high = b;
    if (low.y > high.y) std::swap(low, high);
    if (p.y < low.y || p.y > high.y) return false;
    if (abs(low.y - high.y) < 1E-10) return false; // Горизонтальный отрезок
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

bool check(const Point& pt, const Cell& cell)
{
    int s = 0;
    for (size_t i = 0; i < cell.vertices.size(); ++i)
    {
        double det = (cell.vertices[i].x - pt.x) * (cell.vertices[(i + 1) % cell.vertices.size()].y - cell.vertices[i].y) -
            (cell.vertices[(i + 1) % cell.vertices.size()].x - cell.vertices[i].x) * (cell.vertices[i].y - pt.y);
        if (det > 0)
            s += 1;
        else if (det < 0)
            s -= 1;
    }
    if (abs(s) == cell.vertices.size())
        return true;
    return false;
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
                ans.push_back(j + 1);
                break;
            }
        }
        if (flag == 0)
            ans.push_back(-1);
    }
    return ans;
}
