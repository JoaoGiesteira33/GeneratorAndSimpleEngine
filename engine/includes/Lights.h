#ifndef LIGHTS_H
#define LIGHTS_H

#include "tinyxml2.h"
using namespace tinyxml2;

struct PointLight{
    int lightNumber;
    float posX, posY, posZ;
};

struct DirectionalLight{
    int lightNumber;
    float dirX, dirY, dirZ;
};

struct SpotlightLight{
    int lightNumber;
    float posX, posY, posZ;
    float dirX, dirY, dirZ;
    float cutOff;
};

PointLight loadPointLight(XMLElement * pElement, int lightNumber);
DirectionalLight loadDirectionalLight(XMLElement * pElement, int lightNumber);
SpotlightLight loadSpotlightLight(XMLElement * pElement, int lightNumber);

#endif