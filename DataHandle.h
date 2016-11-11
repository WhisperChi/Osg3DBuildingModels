#ifndef DATAHANDLE_H
#define DATAHANDLE_H

#include <vector>

#include <osg/Geometry>
#include <osgEarth/MapNode>

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

typedef std::vector< std::vector<Coords> > BuildingsData;

///
/// \brief The DataHandle class
/// Create LODs and add into a switch node
///
class DataHandle
{
public:
    DataHandle(osgEarth::MapNode* mapNode,std::string url);
    ~DataHandle();

    void addDataIntoGroup(osg::Switch* group);

private:
    osgEarth::MapNode*                       _mapNode;
    std::string                                           _url;
    std::vector<osg::LOD*>                    _buildings;
};


#endif // DATAHANDLE_H
