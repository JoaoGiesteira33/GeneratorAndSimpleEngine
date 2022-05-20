#include "Lights.h"

PointLight loadPointLight(XMLElement * pElement, int lightNumber){
    PointLight ret;
    ret.lightNumber = lightNumber;

    pElement->QueryFloatAttribute("posX",&ret.posX);
    pElement->QueryFloatAttribute("posY",&ret.posY);
    pElement->QueryFloatAttribute("posZ",&ret.posZ);

    return ret;
}

DirectionalLight loadDirectionalLight(XMLElement * pElement, int lightNumber){
    DirectionalLight ret;
    ret.lightNumber = lightNumber;

    pElement->QueryFloatAttribute("dirX",&ret.dirX);
    pElement->QueryFloatAttribute("dirY",&ret.dirY);
    pElement->QueryFloatAttribute("dirZ",&ret.dirZ);

    return ret;
}

SpotlightLight loadSpotlightLight(XMLElement * pElement, int lightNumber){
    SpotlightLight ret;
    ret.lightNumber = lightNumber;

    pElement->QueryFloatAttribute("posX",&ret.posX);
    pElement->QueryFloatAttribute("posY",&ret.posY);
    pElement->QueryFloatAttribute("posZ",&ret.posZ);

    pElement->QueryFloatAttribute("dirX",&ret.dirX);
    pElement->QueryFloatAttribute("dirY",&ret.dirY);
    pElement->QueryFloatAttribute("dirZ",&ret.dirZ);

    pElement->QueryFloatAttribute("cutoff",&ret.cutOff);

    return ret;
}