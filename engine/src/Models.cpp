#include "Models.h"

ModelInfo::ModelInfo()
{
    texture = "n/a";
    texture_id = 0;
    diffuse[0] = 200.0f; diffuse[1] = 200.0f; diffuse[2] = 200.0f;
    ambient[0] = 50.0f; ambient[1] = 50.0f; ambient[2] = 50.0f;
    specular[0] = 0.0f; specular[1] = 0.0f; specular[2] = 0.0f;
    emissive[0] = 0.0f; emissive[1] = 0.0f; emissive[2] = 0.0f;
    shininess = 0.0f;
}

ModelInfo loadModelInfo(XMLElement * pElement){
    ModelInfo ret;

    XMLElement *textureElement = pElement->FirstChildElement("texture");
    XMLElement *colorElement = pElement->FirstChildElement("color");

    if(textureElement != nullptr ){
        const char * aux = nullptr;
        aux = textureElement->Attribute("file");
        if (aux != nullptr) ret.texture = aux;
    }
    if(colorElement != nullptr ){
        XMLElement * diffuse = colorElement->FirstChildElement("diffuse");
        if(diffuse != nullptr){
            diffuse->QueryFloatAttribute("R",&(ret.diffuse[0]));
            diffuse->QueryFloatAttribute("G",&(ret.diffuse[1]));
            diffuse->QueryFloatAttribute("B",&(ret.diffuse[2]));
        }

        XMLElement * ambient = colorElement->FirstChildElement("ambient");
        if(ambient != nullptr){
            ambient->QueryFloatAttribute("R",&(ret.ambient[0]));
            ambient->QueryFloatAttribute("G",&(ret.ambient[1]));
            ambient->QueryFloatAttribute("B",&(ret.ambient[2]));
        }

        XMLElement * specular = colorElement->FirstChildElement("specular");
        if(specular != nullptr){
            specular->QueryFloatAttribute("R",&(ret.specular[0]));
            specular->QueryFloatAttribute("G",&(ret.specular[1]));
            specular->QueryFloatAttribute("B",&(ret.specular[2]));
        }

        XMLElement * emissive = colorElement->FirstChildElement("emissive");
        if(emissive != nullptr){
            emissive->QueryFloatAttribute("R",&(ret.emissive[0]));
            emissive->QueryFloatAttribute("G",&(ret.emissive[1]));
            emissive->QueryFloatAttribute("B",&(ret.emissive[2]));
        }

        XMLElement * shininess = colorElement->FirstChildElement("shininess");
        if(shininess != nullptr){
            shininess->QueryFloatAttribute("value",&(ret.shininess));
        }
    }

    return ret;
}