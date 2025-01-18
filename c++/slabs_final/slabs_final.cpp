#include <map>
#include <iostream>
#include <vector>
#include <fstream>
#include <algorithm>
#include <iomanip>
#include <ctime>

using namespace std;

class Cell;
class Vertex;

class Point
{
protected:
    double x;
    double y;
public:
    friend ostream& operator<<(ostream& out, Point& p)
    {
        out << "( " << p.x << " ; " << p.y << " )";
        return out;
    }
    friend ifstream& operator>>(ifstream& fin, Point& p)
    {
        fin >> p.x >> p.y;
        return fin;
    }

    double get_y() const
    {
        return y;
    }

    double get_x() const
    {
        return x;
    }
};

class Segment
{
private:
    int n;
    double k;
    double b;
    bool f;
public:
    Segment() :n(), k(), b(), f(false) {};
    Segment(int n_, double k_, double b_) :n(n_), k(k_), b(b_), f(false) {};
    double gt_x(double y)
    {
        return (y - b) / k;
    }
    void true_()
    {
        f = true;
    }
    int get_num() const
    {
        return n;
    }
    double get_k() const
    {
        return k;
    }
    friend Vertex;
};

class Vertex : public Point
{
private:
    Segment* seg1;
    Segment* seg2;
public:
    Vertex() :seg1(nullptr), seg2(nullptr) {};
    Point& operator=(const Point& p)
    {
        x = p.get_x();
        y = p.get_y();
        return *this;
    }
    bool check_seg1()
    {
        return seg1->f;
    }
    bool check_seg2()
    {
        return seg2->f;
    }
    Segment* get_seg1()
    {
        return seg1;
    }
    Segment* get_seg2()
    {
        return seg2;
    }
    ~Vertex() { delete seg2; };
    friend Cell;
};

class Cell
{
private:
    int Num;
    int m;
    Vertex* vert;
public:
    Cell() :Num(0), m(0), vert(nullptr) {};
    Vertex& operator[](int i)
    {
        if (i == m) return vert[0];
        else if (i == -1) return vert[m - 1];
        else return vert[i];
    }
    void init(int num_, int m_)
    {
        Num = num_;
        m = m_;
        vert = new Vertex[m_];
    }
    void create_seg()
    {
        for (int i = 0; i < m - 1; ++i)
        {
            if ((vert[i + 1].x - vert[i].x) != 0) {
                vert[i].seg2 = new Segment(Num, (vert[i + 1].y - vert[i].y) / (vert[i + 1].x - vert[i].x), vert[i].y -
                    ((vert[i + 1].y - vert[i].y) / (vert[i + 1].x - vert[i].x)) * vert[i].x);
                vert[i + 1].seg1 = vert[i].seg2;
            }
            else
            {
                vert[i].seg2 = new Segment(Num, -1000000000., 1000000000. * vert[i].x);
                vert[i + 1].seg1 = vert[i].seg2;
            }
        }
        if ((vert[0].x - vert[m - 1].x) != 0) {
            vert[m - 1].seg2 = new Segment(Num, (vert[0].y - vert[m - 1].y) / (vert[0].x - vert[m - 1].x), vert[m - 1].y -
                ((vert[0].y - vert[m - 1].y) / (vert[0].x - vert[m - 1].x)) * vert[m - 1].x);
            vert[0].seg1 = vert[m - 1].seg2;
        }
        else
        {
            vert[m - 1].seg2 = new Segment(Num, -1000000000., 1000000000. * vert[m - 1].x);
            vert[0].seg1 = vert[m - 1].seg2;
        }
    }
    ~Cell() { delete[] vert; }
};







int main() {


    ifstream fin("problem.txt");
    ofstream fout("result.txt");
    int n;
    fin >> n;
    vector<Point> vec1(n);
    for (int i = 0; i < n; ++i)
        fin >> vec1[i];
    int p;
    fin >> p;
    vector<Cell> vec2(p);
    map<double, Point*> PFL_map;           //мэп для полос
    multimap<double, Vertex*> PFC_map;  //мэп для точек событий
    int a; int b;
    bool f;
    for (int i = 0; i < p; ++i)
    {
        fin >> a;
        vec2[i].init(i + 1, a);
        for (int j = 0; j < a; ++j)
        {
            fin >> b;
            vec2[i][j] = vec1[b - 1];
            PFL_map.emplace(vec1[b - 1].get_y(), &vec1[b - 1]);
        }
        vec2[i].create_seg();
        for (int k = 0; k < a; ++k)
            PFC_map.emplace(vec2[i][k].get_y(), &vec2[i][k]);
    }

    //считываение точек для поиска из файла
    int enum_point_for_search;
    fin >> enum_point_for_search;
    vector<Point> point_for_search(enum_point_for_search);
    for (int i = 0; i < enum_point_for_search; ++i)
        fin >> point_for_search[i];
    //---------------------------------
    clock_t tStart1 = clock();
    //определение полос, в которых лежат точки
    multimap<int, Point*> map_for_search_point;                        //ключ-номер полосы, значение точка для поиска в ней
    vector<int> num_of_slubs_with_point(enum_point_for_search);        //номер полос с точками (так, как они в файле)
    vector<int> sort_num_of_slubs_with_point(enum_point_for_search);   //номер полос с точками (отсортированный)
    map<double, int> slab_numbers;   //ключ - игрик полосы, значение - ее номер
    int k = 0;
    for (auto& it : PFL_map)
        slab_numbers.emplace(it.first, k++);
    for (int i = 0; i < enum_point_for_search; ++i)
    {
        auto it = slab_numbers.emplace(point_for_search[i].get_y(), 0);
        ++it.first;
        map_for_search_point.emplace(it.first->second, &point_for_search[i]);
        num_of_slubs_with_point[i] = it.first->second;     // номера полос, соответствующие порядку точек в файле
        sort_num_of_slubs_with_point[i] = it.first->second;
        --it.first;
        if (it.first->second == 0)
            slab_numbers.erase(point_for_search[i].get_y());
    }
    clock_t tFinish1 = clock();
    sort(sort_num_of_slubs_with_point.begin(), sort_num_of_slubs_with_point.end());
    //------------------------------------------------------

     //вектор с игриками всех полос по возрастанию
    vector<double> vec_line(PFL_map.size());
    int ii = 0;
    for (auto& p : PFL_map)
        vec_line[ii++] = p.first;
    //-----------------------------------------------------

    multimap<double, Segment*> last_map;
    Segment fict;

    map<int, Point*> num_point_cell;
    //проход по деревьям!
    vector<multimap<double, Segment*>> vec_mp(sort_num_of_slubs_with_point[enum_point_for_search - 1]);
    int schet = 0;
    double x = 0;

    double t1 = clock();
    while (schet < sort_num_of_slubs_with_point[enum_point_for_search - 1])
    {
        auto it = PFC_map.equal_range(vec_line[schet]);
        for (auto itr = it.first; itr != it.second; ++itr)
        {
            if (itr->second->check_seg1() && !itr->second->check_seg2())
            {
                vec_mp[schet - 1].erase(itr->second->get_seg1()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet].emplace(itr->second->get_seg2()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_seg2());
                itr->second->get_seg2()->true_();
            }
            if (itr->second->check_seg2() && !itr->second->check_seg1())
            {
                vec_mp[schet - 1].erase(itr->second->get_seg2()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet].emplace(itr->second->get_seg1()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_seg1());
                itr->second->get_seg1()->true_();
            }
            if (itr->second->check_seg2() && itr->second->check_seg1())
            {
                vec_mp[schet - 1].erase(itr->second->get_seg1()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
                vec_mp[schet - 1].erase(itr->second->get_seg2()->gt_x((vec_line[schet] + vec_line[schet - 1]) / 2));
            }
            if (!itr->second->check_seg2() && !itr->second->check_seg1())
            {
                vec_mp[schet].emplace(itr->second->get_seg2()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_seg2());
                vec_mp[schet].emplace(itr->second->get_seg1()->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2),
                    itr->second->get_seg1());

                if ((itr->second->get_seg1()->get_k() == 0) && (itr->second->get_seg2()->get_k() != 0))
                    itr->second->get_seg2()->true_(); else
                    if ((itr->second->get_seg2()->get_k() == 0) && (itr->second->get_seg1()->get_k() != 0))
                        itr->second->get_seg1()->true_(); else
                    {
                        itr->second->get_seg2()->true_();
                        itr->second->get_seg1()->true_();
                    }
            }
        }

        if (schet != 0)
        {
            for (auto& itrmap : vec_mp[schet - 1])
                vec_mp[schet].emplace(itrmap.second->gt_x((vec_line[schet + 1] + vec_line[schet]) / 2), itrmap.second);
        }


        for (int i = 0; i < sort_num_of_slubs_with_point.size(); ++i)
        {
            if (schet + 1 == sort_num_of_slubs_with_point[i]) {

                for (auto itertt = map_for_search_point.equal_range(schet + 1).first; itertt != map_for_search_point.equal_range(schet + 1).second; ++itertt)
                {

                    for (auto it = vec_mp[schet].begin(); it != vec_mp[schet].end(); ++it)
                    {
                        if (it->second->get_k() != 0)
                            last_map.emplace(it->second->gt_x(itertt->second->get_y()), it->second);
                    }
                    clock_t tStart2 = clock();
                    last_map.emplace(last_map.begin()->first - 1, &fict);
                    last_map.emplace(last_map.rbegin()->first + 1, &fict);
                    auto it = last_map.emplace(itertt->second->get_x(), &fict);
                    ++it;
                    auto right_it = last_map.equal_range(it->first);
                    auto rrr = right_it.second;
                    ----it;
                    auto left_it = last_map.equal_range(it->first);
                    auto lll = left_it.first;
                    --lll;
                    for (auto r = right_it.first; r != rrr; ++r)
                        for (auto l = lll; l != left_it.second; ++l)
                            if (r->second->get_num() == l->second->get_num())
                                num_point_cell.emplace(l->second->get_num(), itertt->second);

                    last_map.clear();
                    clock_t tFinish2 = clock();
                    x += (double)((tFinish2 - tStart2));
                }


            }
        }


        ++schet;
    }


    double t2 = clock();
    fout << enum_point_for_search << endl;

    for (int i = 0; i < point_for_search.size(); ++i)
        for (auto& it : num_point_cell)
            if (it.second == &point_for_search[i]) fout << it.first << endl;

    cout << "time = " << ((tFinish1 - tStart1) + x) / 1000 << endl;
}