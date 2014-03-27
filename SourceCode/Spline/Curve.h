#ifndef __AACURVE_H__
#define __AACURVE_H__

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <xutility>

// spline data
struct Spline{
    struct Point{
        float t, y;

        Point(void) : t(0), y(0){}
        Point(float at, float ay) : t(at), y(ay){}
    };

    enum{
        SPLINE_CUBIC = 0,
        //     SPLINE_QUADRATIC,
        //     SPLINE_LINEAR,
        SPLINE_COUNT,
    };

    static std::string spline_name[SPLINE_COUNT];

    int type;

    typedef std::vector<Point> KnotsList;
    std::string name;
    //   std::map<float, aaPoint> knots;
    KnotsList knots;

    bool bLimited;
    float limit_left, limit_top, limit_right, limit_bottom;

    Spline(void)
        : type(SPLINE_CUBIC), bLimited(false), limit_left(0.0f),
        limit_top(0.0f), limit_right(0.0f), limit_bottom(0.0f){

    }

    Spline(const std::string &spline_name)
        : type(SPLINE_CUBIC), name(spline_name), bLimited(false), limit_left(0.0f),
        limit_top(0.0f), limit_right(0.0f), limit_bottom(0.0f){

    }

    void setKnots(const std::vector<std::pair<float, float> > &nots){
        knots.clear();
        for(size_t i=0; i<nots.size(); ++i){
            //knots.insert(std::make_pair(nots[i].first, aaPoint(nots[i].first, nots[i].second)));
            knots.push_back(Point(nots[i].first, nots[i].second));
        }
    }

    void addKnots(std::pair<float, float> knot){
        //knots.insert(std::make_pair(not.first, aaPoint(not.first, not.second)));
        KnotsList::iterator it = knots.begin();
        for(; it!=knots.end(); ++it){
            if(knot.first < (*it).t)
                break;
            else if(knot.first == (*it).t){
                return ;
            }
        }

        knots.insert(it, Point(knot.first, knot.second));
    }

    int addKnots(Point knot){
        //knots.insert(std::make_pair(not.t, not));
        int count = 0;
        KnotsList::iterator it = knots.begin();
        for(; it!=knots.end(); ++it){
            if(knot.t < (*it).t)
                break;
            else if(knot.t == (*it).t){
                return -1;
            }
            ++count;
        }

        knots.insert(it, knot);
        return count;
    }

    void removeKnot(size_t index){
        if(index >= 0 && index < knots.size())    
        {
            auto itr = knots.begin();
            std::advance(itr, index);
            knots.erase(itr);
        }
    }

    void setKnot(size_t index, Point pt) {
        if(index >= 0 && index < knots.size())    
        {
            knots[index] = pt;
        }
    }

    bool findKont(Point pt, size_t &index) {
        for(size_t i = 0; i < knots.size(); ++i)
        {
            if(abs(pt.t - knots[i].t) < 4.f && abs(pt.y - knots[i].y) < 4.f)
            {
                index = i;
                return true;
            }
        }
        return false;
    }

    void setLimit(float left, float top, float right, float bottom){
        bLimited = true;
        limit_left = left;
        limit_top = top;
        limit_right = right;
        limit_bottom = bottom;
    }

    void removeLimit(void){
        bLimited = false;
    }

    void setName(const std::string &name){
        this->name = name;
    }

    size_t size(void) const{ return knots.size(); }
};

// spline function

class Curve{
protected:
    std::string m_name;
    int m_knum;

    double *m_t;
    double *m_knot;

public:
    //       aaCurve(const std::string &name, int knum, double *t, double *knot);
    Curve(const Spline &spline_data);
    virtual ~Curve(void);

    std::string name(void) const{ return m_name; }

    virtual bool getValue(double t, double &value) = 0;

};

class CubicSpline : public Curve{
private:
    double *m_ddp;

public:
    CubicSpline(const Spline &spline_data);
    virtual ~CubicSpline(void);

    virtual bool getValue(double t, double &value);
};

class BSpline : public Curve{
public:
    BSpline(const Spline &spline_data);
    virtual ~BSpline(void);

    virtual bool getValue(double t, double &value);
};


class QuadraticSpline : public Curve{

public:
    QuadraticSpline(const Spline &spline_data);
    virtual ~QuadraticSpline(void);

    virtual bool getValue(double t, double &value);
};

class LinearSpline : public Curve{

public:
    LinearSpline(const Spline &spline_data);
    virtual ~LinearSpline(void);

    virtual bool getValue(double t, double &value);
};
//////////////////////////////////////////////////////////////////////////

class CurveFactory{
private:
    CurveFactory(void);
    CurveFactory(const CurveFactory&);
public:
    static Curve *createCurve(const Spline &spline_data){
        switch(spline_data.type){
        case Spline::SPLINE_CUBIC:
            return new CubicSpline(spline_data);
            /*case aaSpline::SPLINE_QUADRATIC:
            return aaCurvePtr(new aaQuadraticSpline(spline_data));
            case aaSpline::SPLINE_LINEAR:
            return aaCurvePtr(new aaLinearSpline(spline_data));*/
        default:
            return nullptr;
        }
    }
};

#endif // __AASPLINE_H__

