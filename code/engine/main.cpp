#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
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

struct Point{
    GLfloat x;
    GLfloat y;
    GLfloat z;
};

vector<Point*> modelsInfo; //Load all model .3d file's info here.
XMLInfo docInfo;

void drawTriangle(Point a, Point b, Point c){
    glBegin(GL_TRIANGLES);
	    glVertex3f(a.x,a.y,a.z);
	    glVertex3f(b.x,b.y,b.z);
	    glVertex3f(c.x,c.y,c.z);
    glEnd();
}

int loadModelInfo(const char* file_name){
    string line;
    ifstream myfile(file_name);
    
    if (!myfile) {
        cerr << "Unable to open file " << file_name;
        exit(1);
    }

    if (myfile.is_open())
    {
      while ( getline(myfile,line) )
      {
        stringstream ss(line);
        for(int i=0 ; i<3 ; i++)
        {
            struct Point* p = new Point;
            ss >> p->x;
            ss >> p->y;
            ss >> p->z;
            modelsInfo.push_back(p);
        }
      }
      myfile.close();
    }

    return 0;
}

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
    for(int i=0 ; i < modelsInfo.size() - 2 ; i+=3){
        drawTriangle(*modelsInfo[i],*modelsInfo[i+1],*modelsInfo[i+2]);
    }

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

    for(int i = 0 ; i < docInfo.models.size() ; i++){
        const char * aux = docInfo.models[i].c_str();
        cout << "Reading new File: " << aux << endl;
        loadModelInfo(aux);
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
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Enter GLUT's main cycle

    glutMainLoop();

    return 0;
}