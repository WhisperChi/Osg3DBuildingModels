#ifndef DATAHANDLE_H
#define DATAHANDLE_H

#include <vector>

#include <osg/Geometry>

typedef struct _coords
{
    double lon;
    double lat;
    double alt;
    _coords(double x,double y,double z)
        :	lon(x),
            lat(y),
            alt(z)
    {

    }

}Coords;

class DataHandle
{
public:
    DataHandle();
    DataHandle(std::string url);
    ~DataHandle();

    void setUrl(std::string url);
    std::string url();

    int data(std::vector<std::vector<Coords> >& buildingdas);

private:
    std::string                                         _url;

};


#endif // DATAHANDLE_H
