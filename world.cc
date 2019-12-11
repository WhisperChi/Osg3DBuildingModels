/* -*-c++-*- */
/* osgEarth - Dynamic map generation toolkit for OpenSceneGraph
* Copyright 2008-2014 Pelican Mapping
* http://osgearth.org
*
* osgEarth is free software; you can redistribute it and/or modify
* it under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include <osgViewer/Viewer>
#include <osgGA/GUIEventHandler>
#include <osgDB/ReadFile>

#include <osgEarth/Notify>
#include <osgEarthUtil/EarthManipulator>
#include <osgEarthUtil/ExampleResources>

#include <osgEarthDrivers/gdal/GDALOptions>
#include <osgEarthDrivers/tms/TMSOptions>
#include <osgEarth/Map>
#include <osgEarth/MapNode>

//Xc3DCity
#include "Xc3DCity.h"

#define LC "[viewer] "



using namespace osgEarth;
using namespace osgEarth::Util;


int
usage(const char* name)
{
    OE_NOTICE 
        << "\nUsage: " << name << " file.earth" << std::endl
        << MapNodeHelper().usage() << std::endl;

    return 0;
}

int
main(int argc, char** argv)
{
    // create a viewer:
    osgViewer::Viewer viewer;

    // Tell the database pager to not modify the unref settings
    viewer.getDatabasePager()->setUnrefImageDataAfterApplyPolicy( false, false );

    // install our default manipulator (do this before calling load)
    viewer.setCameraManipulator( new EarthManipulator() );

    // load an earth file, and support all or our example command-line options
    // and earth file <external> tags    

    osg::ref_ptr<osg::Group>				root = new osg::Group();
    osg::ref_ptr<osgEarth::Map>				map = new osgEarth::Map();
    osg::ref_ptr<osgEarth::MapNode>  			mapNode = new osgEarth::MapNode(map);
    osg::ref_ptr<osgViewer::CompositeViewer>   	comViewer = new osgViewer::CompositeViewer();

    comViewer->addView(&viewer);

//    //Load Data
//    osgEarth::Drivers::TMSOptions tms;
//    tms.url()  = "http://readymap.org/readymap/tiles/1.0.0/7/";
//    ImageLayer* baseLayer = new ImageLayer("tmsLayer",tms);
//    mapNode->getMap()->addImageLayer(baseLayer);

    //gdal
    osgEarth::Drivers::GDALOptions gdal;
    gdal.url() = "./world.tif";
    //ImageLayer* baseLayer = new ImageLayer("baseLayer",gdal);
    //mapNode->getMap()->addImageLayer(baseLayer);

//    root->addChild(mapNode);

	osg::ref_ptr<osg::Switch>					group = new osg::Switch();
 //   root->addChild(group);

    Xc3DCity*       city = new Xc3DCity(comViewer,group);
    viewer.addEventHandler(new Xc3DCityHandler(group) );
    //Xc3DCityThread* cityThread = Xc3DCityThread::Instance();
    //cityThread->setCityObject(city,mapNode,"./resource/15");
    //cityThread->startThread();

    if ( root ) 
    {
	viewer.setSceneData(osgDB::readNodeFile("gdal_tiff.earth"));
        //viewer.setSceneData( root );

        viewer.getCamera()->setNearFarRatio(0.00002);
        viewer.getCamera()->setSmallFeatureCullingPixelSize(-1.0f);

        return comViewer->run();
    }
    else
    {
        return usage(argv[0]);
    }
}
