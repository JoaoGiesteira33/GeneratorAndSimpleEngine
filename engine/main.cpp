#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
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

XMLInfo docInfo;
vector<GLuint> vertices;
vector<GLuint> verticeCount;

//Camera Values
int moved_camera = 0;
float camera_alpha = 0.0f;
float camera_beta = 0.0f;
float camera_radius = 5.0f;
float camera_x = 5.0f;
float camera_y = 5.0f;
float camera_z = 5.0f;

//FPS auxiliary Values
int timebase;
float frame = 0;

void orbitalCamera(){
	camera_x = camera_radius * cosf(camera_beta) * sinf(camera_alpha);
	camera_y = camera_radius * sinf(camera_beta);
	camera_z = camera_radius * cosf(camera_beta) * cosf(camera_alpha);

	gluLookAt(camera_x, camera_y, camera_z,
			      0.0f,0.0f,0.0f,
				  0.0f,1.0f,0.0f);
}

void prepareData(int ind, const char *file_name){
    vector<float> points;
    string line;
    ifstream myfile(file_name);
    GLfloat aux;
    
    if (!myfile) {
        cerr << "Unable to open file " << file_name;
        exit(1);
    }

    if (myfile.is_open())
    {
      while ( getline(myfile,line) )
      {
        stringstream ss(line);
        for(int i=0 ; i<9 ; i++)
        {
            ss >> aux;
            points.push_back(aux);
        }
      }
      myfile.close();
    }

    verticeCount.push_back(points.size()/3);

    //Criar o VBO
    vertices.push_back(ind);
    glGenBuffers(ind+1,&vertices[ind]);
    glBindBuffer(GL_ARRAY_BUFFER, vertices[ind]);
    glBufferData(GL_ARRAY_BUFFER,
                sizeof(float) * points.size(),
                points.data(),
                GL_STATIC_DRAW);
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
    if(moved_camera == 0){
        gluLookAt(docInfo.cameraInfo[0][0],docInfo.cameraInfo[0][1],docInfo.cameraInfo[0][2],
        docInfo.cameraInfo[1][0],docInfo.cameraInfo[1][1],docInfo.cameraInfo[1][2],
        docInfo.cameraInfo[2][0],docInfo.cameraInfo[2][1],docInfo.cameraInfo[2][2]);
    }else{
        orbitalCamera();
    }

    for(int i = 0 ; i < vertices.size() ; i++){
        glBindBuffer(GL_ARRAY_BUFFER,vertices[i]);
        glVertexPointer(3,GL_FLOAT,0,0);
        glDrawArrays(GL_TRIANGLES,0,verticeCount[i]);
    }
    //FPS Calculation
    frame++;
    int time = glutGet(GLUT_ELAPSED_TIME);
    if(time - timebase > 1000){
        float fps = frame * 1000.0/(time-timebase);
        timebase = time;
        frame = 0;

        char buffer[32];
        sprintf(buffer, "FPS: %f", fps);
        glutSetWindowTitle(buffer);
    }
    //End of Frame
    glutSwapBuffers();
}

void processKeys(unsigned char key, int xx, int yy) {
    //Code to process keys
	switch (key)
	{
	case 'w':
        moved_camera = 1;
		if(camera_beta + 0.2 > 1.5)
			camera_beta = 1.5;
		else
			camera_beta += 0.2;
		break;
	case 's':
        moved_camera = 1;
		if(camera_beta - 0.2 < -1.5)
			camera_beta = -1.5;
		else
			camera_beta -= 0.2;
		break;
	case 'a':
        moved_camera = 1;
		camera_alpha -= 0.3;
		break;
	case 'd':
        moved_camera = 1;
		camera_alpha += 0.3;
		break;
    case 'q':
        camera_radius += 0.2f;
        break;
    case 'e':
        camera_radius -= 0.2f;
        break;
	case 'm':
		if(moved_camera == 1)
			moved_camera = 0;
	default:
		break;
	}
	glutPostRedisplay();
}

int main(int argc, char **argv){
    if(!argv[1] || !*argv[1]) {
        cout << "Invalid Filename";
        return 1;
    }
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
    //glutIdleFunc(renderScene);

    //Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);

    //Init GLEW
    #ifndef __APPLE__
        glewInit();
    #endif
    glEnableClientState(GL_VERTEX_ARRAY);

    for(int i = 0 ; i < docInfo.models.size() ; i++){
        const char * aux = docInfo.models[i].c_str();
        cout << "Reading new File: " << aux << endl;
        prepareData(i,aux);
    }

    //OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Enter GLUT's main cycle
    timebase = glutGet(GLUT_ELAPSED_TIME);
    glutMainLoop();

    return 0;
}