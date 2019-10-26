//
// Created by dean on 19-10-26.
//

#ifndef LAB02_BARRIER_H
#define LAB02_BARRIER_H

#include "Utils.h"

struct Point2D
{
    float x;
    float z;
};

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
    float A, B, C;
    Point2D p1, p2;

    Segment(float p1_x, float p1_z, float p2_x, float p2_z)
    {
        p1.x = p1_x;
        p1.z = p1_z;
        p2.x = p2_x;
        p2.z = p2_z;

        A = p2.z - p1.z;
        B = p1.x - p2.x;
        C = p2.x * p1.z - p1.x * p2.z;
    }

    float calcDist(const Point2D &p)
    {
        float divisor = fabs(A * p.x + B * p.z + C);
        float denominator = sqrt(A * A + B * B);
        return (divisor / denominator);
    }
};


struct quadTreeNode
{
    quadTreeNode* children[4];
    std::vector<Segment>* segments;

    quadTreeNode()
    {
        for (int i = 0; i < 4; ++i)
        {
            children[i] = nullptr;
        }
        segments = nullptr;
    }
};

class Barrier
{
private:
    std::vector<Point2D> outerEdge;
    std::vector<Point2D> innerEdge;

    void loadEdge(const char* path)
    {
        std::map<Point2D, int, cmp> edge;
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
            if(line.substr(0,2) == "v ")
            {
                std::istringstream is(line.substr(2));
                Point2D p;
                float dumb;
                is >> p.x >> dumb >> p.z;
                edge[p] = 0;
            }
        }
        in.close();

        for(auto it : edge)
        {
            std::cout << it.first.x << ' ' << it.first.z << std::endl;
        }
    }

public:
    Barrier(const char* path)
    {
        loadEdge(path);
    }
};

#endif //LAB02_BARRIER_H
