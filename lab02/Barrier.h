//
// Created by dean on 19-10-26.
//

#ifndef LAB02_BARRIER_H
#define LAB02_BARRIER_H

#include "Utils.h"
#include "Car.h"


struct Point2D
{
    friend std::ostream &operator<<(std::ostream &os, const Point2D &p);
    float x;
    float z;

    Point2D()
    {
        x = 0.0f;
        z = 0.0f;
    }

    Point2D(const float &x, const float &z)
    {
        this->x = x;
        this->z = z;
    }


};

std::ostream &operator<<(std::ostream &os, const Point2D &p)
{
    os << p.x << ' ' << p.z;
    return os;
}

struct cmp
{
    bool operator()(const Point2D &p1, const Point2D &p2) const
    {
        if(fabs(p1.x - p2.x) > 1.0e-9)
            return p1.x < p2.x;
        else
            return p1.z < p2.z;
    }
};

struct Segment
{
    int id;
    float A, B, C;
    Point2D p1, p2;
    glm::vec3 dir;

    Segment()
    {
        id = -1;
    }

    Segment(float p1_x, float p1_z, float p2_x, float p2_z)
    {
        id = 0;

        p1.x = p1_x;
        p1.z = p1_z;
        p2.x = p2_x;
        p2.z = p2_z;

        A = p2.z - p1.z;
        B = p1.x - p2.x;
        C = p2.x * p1.z - p1.x * p2.z;

        dir = glm::normalize(glm::vec3(p2.x-p1.x ,0.0f, p2.z-p1.z));
    }

    float calcDist(const Point2D &p)
    {
        float divisor = fabs(A * p.x + B * p.z + C);
        float denominator = sqrt(A * A + B * B);
        return (divisor / denominator);
    }

    void setID(const int &ID)
    {
        id = ID;
    }
};

class Barrier
{
    friend class Road;
private:
    std::vector<Segment> segments;

    void loadSegment(const std::string &path)
    {
        std::ifstream in;
        std::string line;
        in.open(path);
        if(!in)
        {
            std::cerr << "Failed to open " << path << std::endl;
            exit(1);
        }

        while(getline(in, line))
        {
            if(line.substr(0,2) == "o ")
            {
                if(line[2] == 'T')
                {
                    std::cout << "T here" << std::endl;
                    int lines = 0;
                    float x[2], z[2];
                    while(getline(in, line))
                    {
                        if(line.substr(0,2) == "v ")
                        {
                            if(lines < 2)
                            {
                                std::istringstream is(line.substr(2));
                                float dumb;
                                is >> x[lines] >> dumb >> z[lines];
                                lines++;
                            }
                        }
                        else
                        {
                            break;
                        }
                    }
                    segments.push_back(Segment(x[0], z[0], x[1], z[1]));
                }
                else
                {
                    float max_x, max_z, min_x, min_z;
                    max_x = max_z = -MAX;
                    min_x = min_z = MAX;
                    while(getline(in, line))
                    {
                        if(line.substr(0,2) == "v ")
                        {
                            std::istringstream is(line.substr(2));
                            float x, z;
                            float dumb;
                            is >> x >> dumb >> z;
                            max_x = std::max(max_x, x);
                            max_z = std::max(max_z, z);
                            min_x = std::min(min_x, x);
                            min_z = std::min(min_z, z);
                        }
                        else
                        {
                            break;
                        }
                    }
                    segments.push_back(Segment(max_x, max_z, min_x, min_z));
                }
            }
        }
        in.close();

        for(auto it : segments)
        {
            std::cout << it.p1 << ' ' << it.p2 << std::endl;
        }
    }

public:
    Barrier(const std::string &path)
    {
        loadSegment(path);
    }

    Barrier() = default;
};

#endif //LAB02_BARRIER_H
