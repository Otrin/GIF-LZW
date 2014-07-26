#ifndef _MEDIAN_CUT_H_
#define _MEDIAN_CUT_H_

#include <vector>

const int NUM_DIMENSIONS = 3;

struct Point
{
    unsigned char x[NUM_DIMENSIONS];

	bool operator<(const Point& other) const
	{
	   if (x[0] != other.x[0])
		   return (x[0] < other.x[0]);

	   if (x[1] != other.x[1])
		   return (x[1] < other.x[1]);

	   return (x[2] < other.x[2]);
	}
};

class Block
{
    Point minCorner, maxCorner;
    Point* points;
    int pointsLength;
public:
    Block(Point* points, int pointsLength);
    Point * getPoints();
    int numPoints() const;
    int longestSideIndex() const;
    int longestSideLength() const;
    bool operator<(const Block& rhs) const;
    void shrink();
private:
    template <typename T>
    static T min(const T a, const T b)
    {
        if (a < b)
            return a;
        else
            return b;
    }

    template <typename T>
    static T max(const T a, const T b)
    {
        if (a > b)
            return a;
        else
            return b;
    }
};
template <int index>
class CoordinatePointComparator
{
public:
    bool operator()(Point left, Point right)
    {
        return left.x[index] < right.x[index];
    }
};
std::vector<Point> medianCut(Point* image, int numPoints, unsigned int desiredSize);
#endif /* #ifndef _MEDIAN_CUT_H_ */
