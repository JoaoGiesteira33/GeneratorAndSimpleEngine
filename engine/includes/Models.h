#ifndef MODELS_H
#define MODELS_H

#include <string>

#include "tinyxml2.h"

using namespace tinyxml2;
using std::string;

struct ModelInfo{
    string texture;
    float diffuse[3];
    float ambient[3];
    float specular[3];
    float emissive[3];
    float shininess;

    ModelInfo();
};

ModelInfo loadModelInfo(XMLElement * pElement);

#endif