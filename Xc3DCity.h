#ifndef __3D_CITY__
#define __3D_CITY__

#include <stdio.h>
#include <string>

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
#include <osgViewer/CompositeViewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/StateSetManipulator>
#include <osgGA/EventVisitor>
#include <osgDB/WriteFile>

#include <osgEarth/Pickers>

#include <osgGA/GUIEventAdapter>
#include <osgGA/GUIEventHandler>
#include <osgGA/GUIActionAdapter>

#include <osg/OperationThread>

using namespace osgEarth;
using namespace osgEarth::Annotation;
using namespace osgEarth::Features;
using namespace osgEarth::Util;
using namespace osgEarth::Util::Controls;

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

struct SafeXc3DCityOperation;

/*
 * Function: 安全增删队列里面的内容
 * */
class Xc3DCity 
{

public:
	Xc3DCity(osgViewer::CompositeViewer* viewer,osg::Switch* group);	
	~Xc3DCity();

    
protected:

     void addNode(osg::Switch* area);
	 void removeNode(osg::Switch *area); 

private:
	osgViewer::CompositeViewer*				_viewer;
	osg::Switch*							_group;

	std::vector<osg::Switch*>				_safeAddNodes;
	std::vector<osg::Switch*>				_safeRemoveNodes;
	
    OpenThreads::Mutex              		_xc3DCityMutex;
	friend	struct SafeXc3DCityOperation;
	SafeXc3DCityOperation*					_cityOperation;
};

class Xc3DCityHandler:public osgGA::GUIEventHandler
{

public:
	Xc3DCityHandler(osg::Switch* group);	
	~Xc3DCityHandler();

	virtual bool handle( const osgGA::GUIEventAdapter& ea,
							   osgGA::GUIActionAdapter& aa,
							   osg::Object*	ob,
							   osg::NodeVisitor* nv);

private:
	osg::Group* _group;

};

















#endif
