#include "Xc3DCity.h"

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



using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>
#include <vector>

#include "json/json.h"


///
/// \brief parserJson
/// \param fileName
/// \param buildings
/// \return
///
int parserJson(std::string fileName,std::vector<std::vector<Coords>>& buildings)
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
                    lon = coordsArr[j][0].asDouble();
                    lat  = coordsArr[j][1].asDouble();
                    Coords tmp(lon,lat,0.0);
                    building.push_back(tmp);
                }
                buildings.push_back(building);
            }
        }
        else
            return 2;
    }//End Parser

    in.close();

    return 0;
}

///
/// \brief listDir
/// \param path
/// \param a
///
void listDir(std::string path,std::vector<std::string>& files)
{
    DIR              *pDir ;
    struct dirent    *ent  ;
    int               i=0  ;

    pDir=opendir(path.c_str());


    while((ent=readdir(pDir))!=NULL)
    {

        if(ent->d_type & DT_DIR)
        {

            if(strcmp(ent->d_name,".")==0 || strcmp(ent->d_name,"..")==0)
                continue;

            std::string tmp(ent->d_name);
            std::string childpath = path + "/";
            childpath += tmp;

            listDir(childpath,files);

        }
        else
        {
            std::string a(path);
            std::string b(ent->d_name);
            std::string c = a + "/";
            c+=b;
            files.push_back(c);
        }
    }

}



/*
 * Function: 安全更新 Xc3DCity 类
 * */
struct SafeXc3DCityOperation : public osg::Operation
{
    SafeXc3DCityOperation(Xc3DCity* city)
            :	Operation("",true),
                _city(city)
    {
    }


    virtual void operator()(osg::Object*)override
	{
		//Add Nodes		
		for(auto iter = _city->_safeAddNodes.begin();iter != _city->_safeAddNodes.end();iter++)
		{
            std::cout << "Can You See Me?\n" ;
			_city->_group->addChild(*iter);
		}	
		_city->_safeAddNodes.clear();

		//Remove
		//Todo		
	}

	Xc3DCity*	_city;
};

/*
 * Function: 安全增删队列里面的内容
 * */
Xc3DCity::Xc3DCity(osgViewer::CompositeViewer* viewer,osg::Switch* group)
	:	_viewer(viewer),
		_group(group)	
{
    _cityOperation = new SafeXc3DCityOperation(this);
	_viewer->addUpdateOperation(_cityOperation);
}

Xc3DCity::~Xc3DCity()
{
    _viewer->removeUpdateOperation(_cityOperation);
}

void Xc3DCity::addNode(osg::Switch* area)
{
	if( !area )
		return ;

	_safeAddNodes.push_back(area);
}

void Xc3DCity::removeNode(osg::Switch* area)
{
	if( !area )	
		return ;

	_safeRemoveNodes.push_back(area);
}


/* Xc3DCityHandler
 * keyboard event and mouse event
 *
 * */
Xc3DCityHandler::Xc3DCityHandler(osg::Switch* group)
		:	_group(group)
{
    _flag = 0;
    _visiable = true;
}

Xc3DCityHandler::~Xc3DCityHandler()
{

}

bool Xc3DCityHandler::handle( const osgGA::GUIEventAdapter& ea,
									osgGA::GUIActionAdapter& aa,
									osg::Object* ob,
									osg::NodeVisitor* nv)
{
    osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
    if(!viewer)
        return false;
	
    switch( ea.getEventType() )
    {
        case osgGA::GUIEventAdapter::KEYDOWN:
        {
            if(ea.getKey() == osgGA::GUIEventAdapter::KEY_C)
            {
                switch(_flag)
                {
                case 0:
                {
                    osgEarth::Viewpoint vp("",120.27827f,31.52945f,0.0f,57.8275f,-16.3616,3824.4f);
                    ( (osgEarth::Util::EarthManipulator*)(viewer->getCameraManipulator()) )->setViewpoint(vp,3.0f);
                    _flag++;
                }
                    break;
               case 1:
                {
                    osgEarth::Viewpoint vp("",141.922f,30.8864f,0.0f,57.8275f,-16.3616,5824.4f);
                    ( (osgEarth::Util::EarthManipulator*)(viewer->getCameraManipulator()) )->setViewpoint(vp,3.0f);
                    _flag++;
                }
                    break;
                case 2:
                {
                    osgEarth::Viewpoint vp("",111.922f,30.8864f,0.0f,57.8275f,-16.3616,5824.4f);
                    ( (osgEarth::Util::EarthManipulator*)(viewer->getCameraManipulator()) )->setViewpoint(vp,3.0f);
                    _flag++;
                }
                    break;
                default:
                {
                    osgEarth::Viewpoint vp("",101.922f,30.8864f,0.0f,57.8275f,-16.3616,5824.4f);
                    ( (osgEarth::Util::EarthManipulator*)(viewer->getCameraManipulator()) )->setViewpoint(vp,3.0f);
                    _flag = 0;
                }
                    break;
                }//end Switch
            }//end key_c
            else if(ea.getKey() == osgGA::GUIEventAdapter::KEY_X)
            {
                if(_visiable == false)
                   {
                        _group->setNodeMask(1);
                        _visiable = true;
                    }
                else
                {
                    _group->setNodeMask(0);
                    _visiable = false;
                }
            }//end key_x
        }
            break;
        default:
            break;
            return true;
    }

}

/*
 *  Xc3DCityThread
*/

void Xc3DCityThread::run()
{
    _done = false;
    _dirty = true;
    do
    {
        update();

    }while(!_done);
}

int Xc3DCityThread::cancel()
{
    _done = true;
    while( isRunning() )
        YieldCurrentThread();

    return 0;
}

void Xc3DCityThread::setCityObject(Xc3DCity *city,osgEarth::MapNode* mapNode,std::string url)
{
    _city = city;
    _mapNode = mapNode;
    _url = url;
}

osg::LOD *Xc3DCityThread::createGeometry(osgEarth::MapNode* mapNode,const std::vector<Coords>& buildings)
{
    if(buildings.size() == 0)
        return NULL;

    osg::LOD*     lodNode = new osg::LOD();

    osgEarth::Symbology::Geometry*   geom = new osgEarth::Symbology::Polygon();
    osgEarth::Symbology::Style      geomStyle;
    const SpatialReference* geoSRS = mapNode->getMapSRS()->getGeographicSRS();

    //rand Height
    int h = rand()%100 + 20;
    double alt = osg::PI * h;

    for(unsigned int i = 0;i < buildings.size();i++)
        geom->push_back(buildings[i].lon,buildings[i].lat,0.0);

    geomStyle.getOrCreate<LineSymbol>()->stroke()->color() = osgEarth::Symbology::Color::White;
    geomStyle.getOrCreate<LineSymbol>()->stroke()->width() = 2.0f;
    geomStyle.getOrCreate<ExtrusionSymbol>()->height() = alt;
    geomStyle.getOrCreate<PolygonSymbol>()->fill()->color() = Color(Color::White,0.6);

    osgEarth::Annotation::FeatureNode* node =  new osgEarth::Annotation::FeatureNode(
                        mapNode,
                        new osgEarth::Features::Feature(geom,geoSRS,geomStyle)
                        );

    lodNode->addChild(node,0.0f,3000.0f);
    return lodNode;
}

void Xc3DCityThread::update()
{
    OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_threadMutex);

    //1.files
    std::vector<std::string > files;
    listDir(_url,files);

    //2.data acquire
    std::vector<std::vector<Coords>> buildings;
    for(unsigned int i = 0;i < files.size();i++)
    {
        parserJson(files[i],buildings);
    }

    //3.create LODs
    //   add into a Switch node
    osg::Switch* group = new osg::Switch();
    for(unsigned int i = 0;i < buildings.size();i++)
    {
        osg::LOD*   lod = createGeometry(_mapNode,buildings[i]);
        group->addChild( lod);
    }

    std::cout << "buildings num :" << group->getNumChildren() << std::endl;

    std::cout << "Before add" << std::endl;
    _city->addNode(group);
    std::cout << "After add" << std::endl;

    _done = true;
}
