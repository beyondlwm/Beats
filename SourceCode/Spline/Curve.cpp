#include "stdafx.h"
#include "Curve.h"
#include "Spline.h"


std::string Spline::spline_name[Spline::SPLINE_COUNT] = {"Cubic Spline"};

/*   aaCurve::aaCurve(const std::string &name, int knum, double *t, double *knot)
: m_name(name), m_knum(knum), m_t(0), m_knot(0){

if(m_knum <= 0)
return ;

m_t = new double[m_knum];
m_knot = new double[m_knum];

for(int i=0; i<m_knum; ++i){
m_t[i]  = t[i];
m_knot[i] = knot[i];
}

}
*/
Curve::Curve(const Spline &spline_data)
    : m_name(spline_data.name), m_knum(spline_data.size()), m_t(0), m_knot(0){
        if(m_knum <= 0)
            return ;

        m_t = new double[m_knum];
        m_knot = new double[m_knum];

        for(int i=0; i<m_knum; ++i){
            m_t[i]  = spline_data.knots[i].t;
            m_knot[i] = spline_data.knots[i].y;
        }

}

Curve::~Curve(void){
    if(m_t)
        delete []m_t;
    if(m_knot)
        delete []m_knot;
}

bool Curve::getValue(double t, double &value){
    if(!m_t)
        return false;

    return true;
}


//////////////////////////////////////////////////////////////////////////

CubicSpline::CubicSpline(const Spline &spline_data)
    : Curve(spline_data), m_ddp(0){

        if(m_knum > 3)
            m_ddp = spline_cubic_set(m_knum, m_t, m_knot, 2, 0, 2, 0);
}

CubicSpline::~CubicSpline(void){
    if(m_ddp)
        delete []m_ddp;
}

bool CubicSpline::getValue(double t, double &value){
    if(!m_t)
        return false;

    if(m_knum > 3){
        if(!m_ddp)
            return false;

        double dp, ddp;
        value = spline_cubic_val(m_knum, m_t, t, m_knot, m_ddp, &dp, &ddp);

        return true;
    }else if(m_knum > 2){
        double dp;
        spline_quadratic_val(m_knum, m_t, m_knot, t, &value, &dp);

        return true;
    }else if(m_knum > 1){
        double dp;
        spline_linear_val(m_knum, m_t, m_knot, t, &value, &dp);

        return true;
    }else{
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////
QuadraticSpline::QuadraticSpline(const Spline &spline_data)
    : Curve(spline_data){

}

QuadraticSpline::~QuadraticSpline(void){

}

bool QuadraticSpline::getValue(double t, double &value){
    if(!m_t)
        return false;

    if(m_knum > 2){
        double dp;
        spline_quadratic_val(m_knum, m_t, m_knot, t, &value, &dp);

        return true;
    }else if(m_knum > 1){
        double dp;
        spline_linear_val(m_knum, m_t, m_knot, t, &value, &dp);

        return true;
    }else{
        return false;
    }
}


//////////////////////////////////////////////////////////////////////////
LinearSpline::LinearSpline(const Spline &spline_data)
    : Curve(spline_data){

}

LinearSpline::~LinearSpline(void){

}

bool LinearSpline::getValue(double t, double &value){
    if(!m_t || m_knum < 2)
        return false;

    double rate = (t - m_t[0]) / (m_t[1] - m_t[0]);
    if(rate < 0)
        value = m_knot[0];
    if(rate > 1)
        value = m_knot[1];
    value = (1 - rate) * m_knot[0] + rate * m_knot[1];

    return true;
}

