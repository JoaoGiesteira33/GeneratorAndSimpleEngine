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
#include <iterator>
#include <string>
#include <cstdlib>
#include <algorithm>

#include <math.h>
#include <stdio.h>

#include "Matrices.h"
#include "Vectors.h"
#include "tinyxml2.h"

#ifndef XMLCheckResult
	#define XMLCheckResult(a_eResult) if (a_eResult != XML_SUCCESS) { printf("Error: %i\n", a_eResult); return a_eResult; }
#endif

using namespace tinyxml2;
using std::endl;
using std::cout;
using std::string;
using std::ifstream;
using std::vector;
using std::stringstream;
using std::cerr;

struct XMLInfo{
    float cameraInfo[3][3];
    float fov, near, far;
    vector<string> models; //Models to be loaded
};

struct Group{
    vector<Matrix4> transformationMatrix;
    vector<int> models_indices;
    vector<Group*> groups;
};

XMLInfo docInfo;
Group* rootGroup;
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

//Obtem indíce de uma string dentro de um vetor
int getIndex(vector<string> values, string value)
{
    int index = 0;

    for (string s : values) { 
        if (s.compare(value) == 0)
            return index;
        index++;
    }

    return -1;
}

void orbitalCamera(){
	camera_x = camera_radius * cosf(camera_beta) * sinf(camera_alpha);
	camera_y = camera_radius * sinf(camera_beta);
	camera_z = camera_radius * cosf(camera_beta) * cosf(camera_alpha);

	gluLookAt(camera_x, camera_y, camera_z,
			      0.0f,0.0f,0.0f,
				  0.0f,1.0f,0.0f);
}

//Carrega informação sobre vértices num ficheiro para o programa
void prepareData(const int ind, const char *file_name){
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
    cout << "Indice in here is: " << ind << endl;
    GLint value = ind + 1;
    vertices.push_back(value);

    //glGenBuffers(value,&vertices.at(ind));
    cout << "Value: " << value << endl; 
    glBindBuffer(GL_ARRAY_BUFFER, vertices[ind]);
    glBufferData(GL_ARRAY_BUFFER,
                sizeof(float) * points.size(),
                points.data(),
                GL_STATIC_DRAW);
    cout << "Adding => Vertices: " << vertices[ind] << " | VerticesCount: " << verticeCount[ind] << endl;
}

int load_models(Group * group, XMLElement * pList){
    XMLElement* pListElement = pList->FirstChildElement("model");

    while (pListElement != nullptr)
    {
        // Obter nome do ficheiro
        const char * aux = nullptr;
        aux = pListElement->Attribute("file");
        if (aux == nullptr) return XML_ERROR_PARSING_ATTRIBUTE;
        string newModelFile = aux;

        // Confirmar se ficheiro já foi inserido
        int index = getIndex(docInfo.models,newModelFile);

        if (index != -1){ //Ficheiro já existente
            cout << "Already Exists " << newModelFile << " in : " << index << endl;
            (group->models_indices).push_back(index);
        }else{ //Novo ficheiro
            cout << "New Model: " << newModelFile << " in : " << docInfo.models.size() << endl;
            (group->models_indices).push_back(docInfo.models.size());
            docInfo.models.push_back(newModelFile);
        }

        //Continuar a iterar
        pListElement = pListElement->NextSiblingElement("model");
    }

    return 0;
}

vector<Matrix4> load_matrix(XMLElement * transforms){
    vector<Matrix4> ret;
    XMLError eResult;
    float x,y,z,angle;

    XMLElement * pElement = transforms->FirstChildElement();
    while(pElement != nullptr){
        if(strcmp(pElement->Value(),"translate") == 0){
            eResult = pElement->QueryFloatAttribute("x",&x);
            eResult = pElement->QueryFloatAttribute("y",&y);
            eResult = pElement->QueryFloatAttribute("z",&z);
            Matrix4 res;
            res.translate(x,y,z);
            ret.push_back(res);
        }else if(strcmp(pElement->Value(),"rotate") == 0){
            eResult = pElement->QueryFloatAttribute("angle",&angle);
            eResult = pElement->QueryFloatAttribute("x",&x);
            eResult = pElement->QueryFloatAttribute("y",&y);
            eResult = pElement->QueryFloatAttribute("z",&z);
            Matrix4 res;
            res.rotate(angle,x,y,z);
            ret.push_back(res);
        }else if(strcmp(pElement->Value(),"scale") == 0){
            eResult = pElement->QueryFloatAttribute("x",&x);
            eResult = pElement->QueryFloatAttribute("y",&y);
            eResult = pElement->QueryFloatAttribute("z",&z);
            Matrix4 res;
            res.scale(x,y,z);
            ret.push_back(res);
        }
        pElement = pElement->NextSiblingElement();
    }

    return ret;
}

Group* load_group(XMLElement * pList){
    Group * retGroup = new Group;

    //Transformations
    XMLElement * transforms = pList->FirstChildElement("transform");
    if(transforms != nullptr){
        retGroup->transformationMatrix = load_matrix(transforms);
    }

    //Models
    XMLElement * models = pList->FirstChildElement("models");
    if(models != nullptr){
        load_models(retGroup, models);
    }

    //Groups
    XMLElement * group = pList->FirstChildElement("group");
    if( group != nullptr){
        for(; group != nullptr ; group = group->NextSiblingElement("group")){
            Group* newGroup = load_group(group);
            (retGroup->groups).push_back(newGroup);
        }
    }

    return retGroup;
}

int loadFileInfo(XMLNode * pRoot){
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

    //Load Groups
    pElement = pRoot->FirstChildElement("group");
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;
    rootGroup = load_group(pElement);

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

void renderGroup(Group * g){
    //Save current matrix
    glPushMatrix();
    
    for(int i = 0 ; i < (g->transformationMatrix).size() ; i++){
        glMultMatrixf((g->transformationMatrix)[i].get());
    }

    //Render models
    for(int i = 0 ; i < (g->models_indices).size() ; i++){
        int ind = (g->models_indices)[i];
        glBindBuffer(GL_ARRAY_BUFFER,vertices[ind]);
        glVertexPointer(3,GL_FLOAT,0,0);
        glDrawArrays(GL_TRIANGLES,0,verticeCount[ind]);
    }

    //Render sub-groups
    for(int i = 0 ; i < (g->groups).size() ; i++){
        renderGroup((g->groups)[i]);
    }

    //Obtain matrix before transformations from this group
    glPopMatrix();
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

    //Rendering
    renderGroup(rootGroup);

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

    if( loadFileInfo(pRoot) != 0)
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

    //Load 3d files to vertices arrays
    for (int i = 0 ; i < docInfo.models.size() ; i++)
    {
        const char * aux = docInfo.models[i].c_str();
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