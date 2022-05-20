#include "Models.h"

ModelInfo::ModelInfo()
{
    texture = "";
    diffuse[0] = 200.0f; diffuse[1] = 200.0f; diffuse[2] = 200.0f;
    ambient[0] = 50.0f; ambient[1] = 50.0f; ambient[2] = 50.0f;
    specular[0] = 0.0f; specular[1] = 0.0f; specular[2] = 0.0f;
    emissive[0] = 0.0f; emissive[1] = 0.0f; emissive[2] = 0.0f;
    shininess = 0.0f;
}