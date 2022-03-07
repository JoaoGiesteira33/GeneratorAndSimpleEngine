#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <cstdlib>

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

XMLInfo docInfo;

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

void changeSize(int w, int h){
    // Prevent a divide by zero, when window is too short
	if(h == 0)
		h = 1;
	//Compute window's aspect ratio 
	float ratio = w * 1.0 / h;
	//Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	//Load Identity Matrix
	glLoadIdentity();
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
	// Set perspective
	gluPerspective(docInfo.fov ,ratio, docInfo.near, docInfo.far);
	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void){
    //Clear Buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set The Camera
    glLoadIdentity();
    gluLookAt(docInfo.cameraInfo[0][0],docInfo.cameraInfo[0][1],docInfo.cameraInfo[0][2],
    docInfo.cameraInfo[1][0],docInfo.cameraInfo[1][1],docInfo.cameraInfo[1][2],
    docInfo.cameraInfo[2][0],docInfo.cameraInfo[2][1],docInfo.cameraInfo[2][2]);

    //Drawing instructions here
    //Axis | REMOVER LATER | ONLY FOR TEST
    glBegin(GL_LINES);
	    // X axis in red
	    glColor3f(1.0f, 0.0f, 0.0f);
	    glVertex3f(	0.0f, 0.0f, 0.0f);
	    glVertex3f( 100.0f, 0.0f, 0.0f);
	    // Y Axis in Green
	    glColor3f(0.0f, 1.0f, 0.0f);
	    glVertex3f(0.0f, 0.0f, 0.0f);
	    glVertex3f(0.0f, 100.0f, 0.0f);
	    // Z Axis in Blue
	    glColor3f(0.0f, 0.0f, 1.0f);
	    glVertex3f(0.0f, 0.0f, 0.0f);
	    glVertex3f(0.0f, 0.0f, 100.0f);
    glEnd();

    glutWireCube(5);

    glutPostRedisplay();
    //End of Frame
    glutSwapBuffers();
}

int main(int argc, char **argv){
    const char* file_name = argv[1];
    cout << "FileName: " << file_name << endl;

    XMLDocument doc;
    XMLPrinter printer;

    XMLError eResult = doc.LoadFile(file_name);
    XMLCheckResult(eResult);
    doc.Print();
    
    XMLNode * pRoot = doc.FirstChild();
    if (pRoot == nullptr) return XML_ERROR_FILE_READ_ERROR;

    if( loadFileInfo(pRoot,docInfo) != 0)
    {
        cout << "Error loading file!" << endl;
        return 1;
    }

    //Init GLUT and the Window
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow(file_name);

    //Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);

    //OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    //Enter GLUT's main cycle
    glutMainLoop();

    return 0;
}