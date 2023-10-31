#include <iostream>
#include <fstream>
#include <unistd.h>
#include <vector>
#include <cmath>
#include <chrono>
#include <thread>

using namespace std;

int s_min=2, s_max=10;
int n_min=1, n_max=5;
int l_min=5, l_max=5;
int c_min=-20, c_max=20;     

int randomGen(int min, int max) {
    ifstream urandom("/dev/urandom", ios::binary);

    if (!urandom) {
        cerr << "Error: cannot open /dev/urandom\n";
        exit(EXIT_FAILURE);
    }
    int N = (max - min) + 1;
    int num = 0;
    while (true) {
        urandom.read(reinterpret_cast<char*>(&num), sizeof(num));
        num = num % N + min;
        if (num <= max && num >= min) {
            urandom.close();
            break;
        }
    }
    return num;
}


class Point {
private:
    int x_, y_;
public:
    Point(int x = 0, int y = 0) : x_(x), y_(y) {}

    int getX() const { return x_; } // get x value
    int getY() const { return y_; } // get y value

    // calculate distance to another point
    int dist(const Point& other) const
    {
        int xd = x_ - other.x_;
        int yd = y_ - other.y_;
        return std::sqrt(xd*xd + yd*yd);
    }
};

class Line {
public:
    Line(Point src, Point dst) : src_(src), dst_(dst) {}
    Point getSrc() const { return src_; }
    Point getDst() const { return dst_; }
private:
    Point src_, dst_;
};

bool equality(const Point& a, const Point& b)
{
    return (a.getX() == b.getX() && a.getY() == b.getY());
}

bool onSegment(const Point& p, const Point& q, const Point& r)
{
    if (q.getX() <= std::max(p.getX(), r.getX()) && q.getX() >= std::min(p.getX(), r.getX()) &&
        q.getY() <= std::max(p.getY(), r.getY()) && q.getY() >= std::min(p.getY(), r.getY()))
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool isIntersecting(const Point& p1, const Point& p2, const Point& q1, const Point& q2)
{
    int a = (q1.getX() - p1.getX()) * (p2.getY() - p1.getY()) - (q1.getY() - p1.getY()) * (p2.getX() - p1.getX());
    int b = (q2.getX() - p1.getX()) * (p2.getY() - p1.getY()) - (q2.getY() - p1.getY()) * (p2.getX() - p1.getX());
    int c = (p1.getX() - q1.getX()) * (q2.getY() - q1.getY()) - (p1.getY() - q1.getY()) * (q2.getX() - q1.getX());
    int d = (p2.getX() - q1.getX()) * (q2.getY() - q1.getY()) - (p2.getY() - q1.getY()) * (q2.getX() - q1.getX());

    if (a * b < 0 && c * d < 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

class street
{
private:
    int st_max;
    int st_min;
    int stCount;
    vector<string> st_names;
    int numOfLines;
    int numOfcoordinates;
    int coordinatesRange;
    string coordinates_str;
    vector<vector<Point>> cod_vector;
    vector<vector<Line>> line_vector;
    vector<string> coordinates;
public:
    street(int min,int max);
    vector<string> streetNameGen();
    vector<string> coordinatesGen();
    vector<vector<Line>> lineGen();
    int getStreetCount()
    { 
        return stCount;
    }

};

street::street(int min,int max)
{
    if (min > max) {
        throw invalid_argument("Minimum cannot be greater than maximum.");
    }
    st_min=min;
    st_max=max;
    stCount=randomGen(st_min,st_max);
}

vector<string> street::streetNameGen() {
    vector<string> st_names;
    int count = stCount;
    for (char i = 'A'; i <= 'Z' && count > 0; i++) {
        string st_name = "\"Street ";
        st_name += i;
        st_name += "\" ";
        st_names.push_back(st_name);
        count--;
    }
    return st_names;
}

vector<string> street::coordinatesGen()
{
    vector<Point> cod;
    vector<Line> line_seg;
    int maxlimitloop;
    //cout<<stCount<<endl;
    int flag_intersec=0;
    for (int i = 0; i < stCount; i++)     
{
    cod.clear();
    maxlimitloop = 0;
    coordinates_str = "";
    numOfLines = randomGen(n_min, n_max);
    numOfcoordinates = numOfLines + 1;
    for (int j = 0; j < numOfcoordinates; j++)
    {
        int c_x = randomGen(c_min, c_max);
        int c_y = randomGen(c_min, c_max);
        Point p(c_x, c_y);
        if (cod.size() < 1)
        {
            cod.push_back(p);
            continue;
        }
        else
        {
            int flag_self = 0;
            for (int ii = 0; ii < cod.size() - 1; ii++)
            {
                if (onSegment(cod[ii], cod[ii + 1], p) || isIntersecting(cod[ii], cod[ii + 1], cod[cod.size() - 1], p))
                {
                    flag_self++;
                    maxlimitloop++;
                    break;
                }
            }
            int flag_overlap = 0;
            for (int d = 0; d < line_vector.size(); d++)
            {
                vector<Line> temp_line = line_vector[d];
                for (int dd = 0; dd < temp_line.size(); dd++)
                {
                    Point new_point = p;
                    Point old_point = cod[cod.size() - 1];
                    Point l_p1 = temp_line[dd].getSrc();
                    Point l_p2 = temp_line[dd].getDst();
                    bool flag1 = equality(old_point, l_p1);
                    bool flag2 = equality(new_point, l_p2);
                    if (flag1 == true && flag2 == true)
                    {
                        flag_overlap++;
                    }
                    if (isIntersecting(l_p1, l_p2, old_point, new_point))
                    {
                        flag_intersec++;
                    }
                }
            }
            if (flag_self != 0 || flag_overlap != 0)
            {
                j--;
                if (maxlimitloop == 25)
                {
                    i--;
                    cerr << "Error: failed to generate valid input for " << to_string(maxlimitloop) << " simultaneous attempts\n";
                    exit(EXIT_FAILURE);
                }
                continue;
            }
            else
            {
                cod.push_back(p);
                continue;
            }
        }
    }

        if(cod.size()==numOfcoordinates )
        {
            for (int ii=0;ii<cod.size();ii++)
            {
                coordinates_str+="("+to_string(cod[ii].getX())+","+to_string(cod[ii].getY())+") ";
 
            }

            coordinates.push_back(coordinates_str);
            cod_vector.push_back(cod);         
        }
        vector<Line> st_line;
        st_line.clear();
        for(int k=0;k<cod.size();k++)
        {
            Line lseg(cod[k],cod[k+1]);
            st_line.push_back(lseg);
        }
        line_vector.push_back(st_line);    
    }
    if(flag_intersec > 1)
    {   
        return coordinates;
    }
    else
    {
        coordinates.clear();
        return coordinates;
    }
}

 

int parseArg(int argc, char **argv, int* s_max, int* n_max, int* l_max, int* c_max, int* c_min) {
    int opt;

    opterr = 0;

    while ((opt = getopt(argc, argv, "s:n:l:c:")) != -1) {
        switch(opt) {
            case 's':
            if (atoi(optarg) > 2) {
                *s_max = atoi(optarg);
            }
            else {
                    cerr << "Error: s must be greater than 2\n" << endl;;
                    exit(EXIT_FAILURE);
            }
                break;
            case 'n':
                if (atoi(optarg) > 1) {
                *n_max = atoi(optarg);
                    }
                else {
                        cerr << "Error: n must be greater than 1\n" << endl;;
                        exit(EXIT_FAILURE);
                     }
                break;
            case 'l':
                if (atoi(optarg) >= 5) {
                *l_max = atoi(optarg);
                  }
                else {
                        cerr << "Error: l must be greater than or equal to 5\n" << endl;;
                        exit(EXIT_FAILURE);
                    }
                break;
            case 'c':
                *c_min = -atoi(optarg);
                *c_max = atoi(optarg);
                break;
            default:
                cerr << "Error: Invalid option\n";
                return 1;
        }
    }
    
    return 0;
}

int main(int argc, char **argv)
{
    // int s_max, n_max, l_max, c_max, c_min;
    parseArg(argc, argv, &s_max, &n_max, &l_max, &c_max, &c_min);
    vector<string> street_names;
    vector<string> coo;

    while (true)
    {
        street st(s_min,s_max); 

        street_names.clear();
        street_names = st.streetNameGen();
        int l = randomGen(l_min,l_max);

        coo = st.coordinatesGen();
        if (!coo.empty())
        {
            for (int i = 0; i < coo.size(); i++)
            {
                cout << "a " << street_names[i] << coo[i] << "\n";
            }
            cout << "g" << std::endl;
        }
        else
        {
            cerr << "Error: No valid graph generated\n";
            continue;
        }

        if (!street_names.empty())
        {
            for (const auto& street_name : street_names)
            {
                cout << "r " << street_name << "\n";
            }
        }

        this_thread::sleep_for(chrono::seconds(l));
    }
    return 0;
}