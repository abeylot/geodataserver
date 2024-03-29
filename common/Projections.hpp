#ifndef _PROJECTION_HPP_
#define _PROJECTION_HPP_

class Projection {
public:
    virtual double lat2y(const double lat) const = 0;
    virtual double lon2x(const double lon) const = 0;
};


class MercatorProj: public Projection {
public:
    double lat2y(const double lat) const override;
    double lon2x(const double lon) const override;
};

class WebMercatorProj: public Projection {
public:
    double lat2y(const double lat) const override;
    double lon2x(const double lon) const override;
};
#endif
