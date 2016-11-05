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
				osgEarth::Viewpoint vp("",121.922f,30.8864f,0.0f,57.8275f,-16.3616,5824.4f);	
                ( (osgEarth::Util::EarthManipulator*)(viewer->getCameraManipulator()) )->setViewpoint(vp,3.0f);
			}
		}
			break;
		default:
			break;
			return true;
	}

}


