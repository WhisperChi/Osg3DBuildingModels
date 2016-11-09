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

///
/// \brief myTree
/// \param dir
/// \param files
/// \return
///
int myTree(const std::string dir,std::vector<std::string>& files)
{
    DIR              *pDir ;
    struct dirent    *ent  ;
    int               i=0  ;
    char              childpath[512];

    pDir=opendir(dir.c_str());
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


DataHandle::DataHandle()
{
    _url = "";
}

DataHandle::DataHandle(std::string url)
{
    _url    =   url;
}

DataHandle::~DataHandle()
{

}

void DataHandle::setUrl(std::string url)
{
    _url = url;
}

std::string DataHandle::url()
{
    return _url;
}

int DataHandle::data(std::vector<std::vector<Coords>>& buildings)
{
    if(_url == "")
        return 1;

    std::vector<std::string> filePaths;
    myTree(_url,filePaths);

    buildings.clear();
    for(unsigned int i = 0;i < filePaths.size();i++)
        parserJson(filePaths,buildings) ;

    return 0;
}
