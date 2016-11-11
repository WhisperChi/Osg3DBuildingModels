#include "DataHandle.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>

//Json
#include "json/json.h"
#include <fstream>

#include <osgEarth/MapNode>
#include <osgEarth/Decluttering>
#include <osgEarth/ECEF>

#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/AnnotationEvents>
#include <osgEarthUtil/AutoClipPlaneHandler>
#include <osgEarthUtil/ExampleResources>

#include <osgEarthAnnotation/AnnotationEditing>
#include <osgEarthAnnotation/AnnotationRegistry>
#include <osgEarthAnnotation/ImageOverlay>
#include <osgEarthAnnotation/ImageOverlayEditor>
#include <osgEarthAnnotation/CircleNode>
#include <osgEarthAnnotation/RectangleNode>
#include <osgEarthAnnotation/EllipseNode>
#include <osgEarthAnnotation/PlaceNode>
#include <osgEarthAnnotation/LabelNode>
#include <osgEarthAnnotation/LocalGeometryNode>
#include <osgEarthAnnotation/FeatureNode>
#include <osgEarthAnnotation/HighlightDecoration>
#include <osgEarthAnnotation/ScaleDecoration>

#include <osgEarthSymbology/GeometryFactory>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>
#include <osgDB/WriteFile>

#include <osg/OperationThread>

#include <osgEarth/Pickers>

///
/// \brief myTree
/// \param path
/// \param files
/// \return
///
int myTree(char* path ,std::vector<std::string>& files)
{
    DIR              *pDir ;
    struct dirent    *ent  ;
    int               i=0  ;
    char              childpath[512];

    pDir=opendir(path);
    memset(childpath,0,sizeof(childpath));

    while((ent=readdir(pDir))!=NULL)
    {
        if(ent->d_type & DT_DIR)
        {
            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
                continue;

            sprintf(childpath,"%s/%s",path,ent->d_name);
            printf("path:%s/n",childpath);

            listDir(childpath)  ;

         }
         else
         {
                std::cout<<ent->d_name<<std::endl;
                std::string tmp(ent->d_name);
                files.push_back(tmp);
         }
     }

    return 0;
}

///
/// \brief parserJson
/// \param fileName
/// \return
///
int parserJson(std::string fileName,BuildingsData& buildings)
{
    std::ifstream in(fileName.c_str(),std::ios::binary);
    if( !in.is_open() )
        return 1;

    //Json Parser
    {
        Json::Reader reader;
        Json::Value    root;

        if( reader.parse(in,root) )
        {
            Json::Value arr = root["features"];
            for(unsigned int i = 0; i < arr.size();i++)
            {
                Json::Value coordsArr = arr[i]["geometry"]["coordinates"][0];
                std::vector<Coords> building;
                building.clear();
                for(unsigned int j = 0; j < coordsArr.size();j++)
                {
                    double lon,lat;
                    Coords tmp;
                    lon = coordsArr[j][0].asDouble();
                    lat  = coordsArr[j][1].asDouble();
                    tmp.lon = lon;
                    tmp.lat = lat;
                    building.push_back(tmp);
                }
                buildings.push_back(building);
            }
        }
    }//End Parser

    return 0;
}

///
/// \brief createBuilding
///     Create a building LOD
/// \param coordsData
/// \return
///
osg::LOD* createBuilding(std::vector<Coords> coordsData,osgEarth::MapNode* mapNode)
{
    if(coordsData.size() == 0)
        return NULL;

    osgEarth::Symbology::Geometry* geom = new osgEarth::Symbology::Polygon();
    osgEarth::Symbology::Style            geomStyle ;
    const osgEarth::SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();

    //Rand height
    int h = rand()%50 + 30; //0 considered
    double alt = osg::PI * h;

    style.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color::White;
    style.getOrCreate<LineSymbol>()->stroke()->width() = 3.0f;
    style.getOrCreate<ExtrusionSymbol>()->height() = alt;
    style.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::White,0.7);

    osg::LOD* building = new osg::LOD();
    for(unsigned int i = 0 ; i < coordsData.size(); i++)
    {
        geom->push_back(coordsData[i].lon,coordsData[i].lat,coordsData[i].alt);
    }
    osgEarth::Annotation::FeatureNode* node = new osgEarth::Annotation::FeatureNode(
                mapNode,
                new osgEarth::Features::Feature(geom,geoSRS,geomStyle)
                );
    building->addChild(node,0.0f,2000);

    return building;
}

DataHandle::DataHandle(osgEarth::MapNode *mapNode, std::string url)
    :   _mapNode(mapNode),
        _url(url)
{

}

DataHandle::~DataHandle()
{

}

void DataHandle::addDataIntoGroup(osg::Switch *group)
{
    //1. search files from url
    std::vector<std::string> files;
    myTree(_url,files);

    //2.create geomtrys ,add into a LOD vector
    for(unsigned int i = 0; i < files.size(); i++)
    {
        BuildingsData tmpData ;
        parserJson(files[i],tmpData);
        for(unsigned int j = 0; j < tmpData.size();j++)
        {
            _buildings->push_back(createBuilding(tmpData[i], _mapNode) );
        }
    }

    //3.Add LODs to a group
    group->addChild(_buildings);
}

