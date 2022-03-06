#include <iostream>
#include <fstream>
#include <vector>
#include <string>

#include <stdio.h>

#include "tinyxml2.h"

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

using namespace tinyxml2;
using namespace std;

struct XMLInfo{
    float cameraInfo[3][3];
    float fov, near, far;
    vector<string> models;
};

int loadFileInfo(XMLNode * pRoot, XMLInfo &docInfo){
    XMLError eResult;
    //Load Camera Position Info
    XMLElement * pElement = pRoot->FirstChildElement("camera")->FirstChildElement( "position" );
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pElement->QueryFloatAttribute("x",&docInfo.cameraInfo[0][0]);
    eResult = pElement->QueryFloatAttribute("y",&docInfo.cameraInfo[0][1]);
    eResult = pElement->QueryFloatAttribute("z",&docInfo.cameraInfo[0][2]);

    //Load Camera LookAt Info
    pElement = pRoot->FirstChildElement("camera")->FirstChildElement( "lookAt" );
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pElement->QueryFloatAttribute("x",&docInfo.cameraInfo[1][0]);
    eResult = pElement->QueryFloatAttribute("y",&docInfo.cameraInfo[1][1]);
    eResult = pElement->QueryFloatAttribute("z",&docInfo.cameraInfo[1][2]);

    //Load Camera Up Info
    pElement = pRoot->FirstChildElement("camera")->FirstChildElement( "up" );
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pElement->QueryFloatAttribute("x",&docInfo.cameraInfo[2][0]);
    eResult = pElement->QueryFloatAttribute("y",&docInfo.cameraInfo[2][1]);
    eResult = pElement->QueryFloatAttribute("z",&docInfo.cameraInfo[2][2]);

    //Load Camera Projection Info
    pElement = pRoot->FirstChildElement("camera")->FirstChildElement( "projection" );
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;

    eResult = pElement->QueryFloatAttribute("fov",&docInfo.fov);
    eResult = pElement->QueryFloatAttribute("near",&docInfo.near);
    eResult = pElement->QueryFloatAttribute("far",&docInfo.far);

    //Load Model Files
    pElement = pRoot->FirstChildElement("group")->FirstChildElement( "models" );
    XMLElement * pListElement = pElement->FirstChildElement("model");
    while (pListElement != nullptr)
    {
        const char * aux = nullptr;
        aux = pListElement->Attribute("file");
        if (aux == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        string newModelFile = aux;
        docInfo.models.push_back(newModelFile);
        //Continuar a iterar
        pListElement = pListElement->NextSiblingElement("model");
    }
    return 0;
}

int main(int argc, char **argv){
    const char* file_name = argv[1];
    cout << "FileName: " << file_name << endl;

    XMLDocument doc;
    XMLPrinter printer;
    XMLInfo docInfo;

    XMLError eResult = doc.LoadFile(file_name);
    XMLCheckResult(eResult);
    doc.Print();
    
    XMLNode * pRoot = doc.FirstChild();
    if (pRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

    int ret = loadFileInfo(pRoot,docInfo);

    //Testar se dados estão corretos
    for(int i = 0 ; i < 3 ; i++){
        for(int j = 0; j < 3 ; j++)
            cout << docInfo.cameraInfo[i][j] << " | ";
            if( i == 2)
                cout << endl;
    }
    cout << "FOV: " << docInfo.fov << " | Near: " << docInfo.near << " | Far: " << docInfo.far << endl;
    for(int i=0; i < docInfo.models.size(); i++){
        cout << "Model " << i << ": " << docInfo.models[i] << endl;
    }

    return 0;
}