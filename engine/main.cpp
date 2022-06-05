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
#include <map>

#include <math.h>
#include <stdio.h>
#include <IL/il.h>

#include "Matrices.h"
#include "Vectors.h"
#include "tinyxml2.h"
#include "TimeBasedTransformations.h"
#include "Lights.h"
#include "Models.h"

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
using std::map;

struct XMLInfo{
    //Camera
    float cameraInfo[3][3];
    float fov, near, far;
    //Models
    vector<string> models;
    map<string, int> textures;
    //Lights
    vector<PointLight> vpntLight;
    vector<DirectionalLight> vdirLight;
    vector<SpotlightLight> vsptlLight;
};

struct TimeDependentTranslate{
    int matrix_index; //Matrix in the group saving the transformation values
    int time, align; //Align 1 is True, Align 0 is False
    vector<float> coordinates;
};
struct TimeDependentRotate{
    int matrix_index; //Matrix in the group saving the transformation values
    int time;
    vector<float> coordinates;
};

struct Group{
    vector<Matrix4> transformationMatrix;
    vector<TimeDependentTranslate*> timeDependentTranslates;
    vector <TimeDependentRotate*> timeDependentRotates;
    vector<int> models_indices;
    vector<ModelInfo> models_info;
    vector<Group*> groups;
};

XMLInfo docInfo;
Group* rootGroup;
vector<GLuint> vertices;
vector<GLuint> verticeCount;
vector<GLuint> normals;
vector<GLuint> textures;

//Camera Values
int camera_mode = 0;

float camera_alpha = 0.0f;
float camera_beta = 0.0f;
float camera_radius = 250.0f;

float camera_x;
float camera_y;
float camera_z;

float camera_dx;
float camera_dy;
float camera_dz;

//FPS auxiliary Values
int timebase;
float frame = 0;

//Mouse values
GLfloat mousePosX, mousePosY;
int tracking = 0;

int loadTexture(string s) {
	unsigned int t,tw,th;
	unsigned char *texData;
	unsigned int texID;

	//Iniciar o DevIL
	ilInit();
    
	//Colocar origem da textura no canto inferior esquerdo
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_LOWER_LEFT);

	//Carregar a textura para memória
	ilGenImages(1,&t);
	ilBindImage(t);
	ilLoadImage((ILstring)s.c_str());
	tw = ilGetInteger(IL_IMAGE_WIDTH);
	th = ilGetInteger(IL_IMAGE_HEIGHT);

	//Assegurar que a textura se encontra em RGBA
	//(Red, Green, Blue, Alpha)
	//com um byte (0 - 255) por componente
	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
	texData = ilGetData();

	//Gerar a textura para a placa gráfica
	glGenTextures(1,&texID);
	
	glBindTexture(GL_TEXTURE_2D,texID);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		
	//Upload dos dados de imagem
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);

	return texID;
}

void renderLight(){
    for(int i = 0 ; i < docInfo.vpntLight.size() ; i++){
        PointLight aux = docInfo.vpntLight[i];
        GLfloat position[] = { aux.posX, aux.posY, aux.posZ, 1.0 }; //W=1 -> Point Light
        glEnable(GL_LIGHT0 + aux.lightNumber);
        glLightfv(GL_LIGHT0 + aux.lightNumber, GL_POSITION, position);
    }
    for(int i = 0 ; i < docInfo.vdirLight.size() ; i++){
        DirectionalLight aux = docInfo.vdirLight[i];
        GLfloat position[] = { aux.dirX, aux.dirY, aux.dirZ, 0.0 }; //W=0 -> Directional Light
        glEnable(GL_LIGHT0 + aux.lightNumber);
        glLightfv(GL_LIGHT0 + aux.lightNumber, GL_POSITION, position);
    }
    for(int i = 0 ; i < docInfo.vsptlLight.size() ; i++){
        SpotlightLight aux = docInfo.vsptlLight[i];
        GLfloat position[] = { aux.posX, aux.posY, aux.posZ, 1.0 }; //W=1 -> Point Light acting as Spotlight
        GLfloat spot_direction[] = { aux.dirX, aux.dirY, aux.dirZ};
        GLfloat spot_cutoff[] = {aux.cutOff};
        glEnable(GL_LIGHT0 + aux.lightNumber);
        glLightfv(GL_LIGHT0 + aux.lightNumber, GL_POSITION, position);
        glLightfv(GL_LIGHT0 + aux.lightNumber, GL_SPOT_CUTOFF, spot_cutoff);
        glLightfv(GL_LIGHT0 + aux.lightNumber, GL_SPOT_DIRECTION, spot_direction);
    }
}

void renderCatmullRomCurve(float **points, int point_count) {
    glDisable(GL_LIGHTING);
	float point[3];
	float dir[3];
	glBegin(GL_LINE_LOOP);
	for(float t = 0 ; t < 1 ; t += 0.01f){
		getGlobalCatmullRomPoint(t,point,dir,points,point_count);
		glVertex3f(point[0],point[1],point[2]);
	}
	glEnd();
    glEnable(GL_LIGHTING);
}

void updateRotateMatrix(Group * g, TimeDependentRotate * tdr){
    Matrix4 rotationMatrix;

    float time_elapsed = glutGet(GLUT_ELAPSED_TIME);
    time_elapsed = time_elapsed / 1000;
    time_elapsed = time_elapsed / (tdr->time);
    float angle = 360 * time_elapsed;

    rotationMatrix.rotate(angle,tdr->coordinates[0],tdr->coordinates[1],tdr->coordinates[2]);

    g->transformationMatrix[tdr->matrix_index] = rotationMatrix;
}

void updateTranslateMatrix(Group * g, TimeDependentTranslate * tdt){
    Matrix4 tmatrix;
    int point_count = (tdt->coordinates).size() / 3;
    //Create matrix for point
    float** points = new float*[point_count];
    for(int i = 0; i < point_count; ++i){
        points[i] = new float[3];
        points[i][0] = tdt->coordinates[3*i];
        points[i][1] = tdt->coordinates[3*i+1];
        points[i][2] = tdt->coordinates[3*i+2];
    }
    //Elapsed Time
    float time_elapsed = glutGet(GLUT_ELAPSED_TIME);
    time_elapsed = time_elapsed / 1000;
    time_elapsed = time_elapsed / (tdt->time);

    //Calculate Matrix
	float point[3];
	float dir[3];
	getGlobalCatmullRomPoint(time_elapsed,point,dir,points,point_count);

    tmatrix.translate(point[0],point[1],point[2]);
    
    if(tdt->align == 1){
	    float rotMatrix[4][4];
	    float x[3]; x[0] = dir[0]; x[1] = dir[1]; x[2] = dir[2];
	    static float y[3] = {0,1,0};
	    float z[3]; cross(x,y,z);
	    cross(z,x,y);

	    //Calculate Rotation Matrix
	    normalize(x);normalize(y);normalize(z);
	    buildRotMatrix(x,y,z,(float *)rotMatrix);
        tmatrix = tmatrix * Matrix4((float*)rotMatrix);
    }
    //Update matrix
    g->transformationMatrix[tdt->matrix_index] = tmatrix;
    renderCatmullRomCurve(points,point_count);
}

//Obtem indíce de uma string dentro de um vetor
int getIndex(const vector<string>& values, const string& value){
    int index = 0;

    for (string s : values) {
        if (s == value)
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
              0.0f, 0.0f, 0.0f,
              0.0f,1.0f,0.0f);
}

 void fpsCamera(){
     camera_dx = cosf(camera_beta) * sinf(camera_alpha);
     camera_dy = sinf(camera_beta);
     camera_dz = cosf(camera_beta) * cosf(camera_alpha);
 
     gluLookAt(camera_x, camera_y, camera_z,
                   camera_x - camera_dx,camera_y - camera_dy, camera_z - camera_dz,
               0.0f,1.0f,0.0f);
 }

//Carrega informação sobre vértices num ficheiro para o programa
void prepareData(const int ind, const char *file_name){
    vector<float> points;
    vector<float> v_normals;
    vector<float> v_textures;
    string line;
    ifstream myfile(file_name);
    GLfloat aux;
    
    if (!myfile) {
        cerr << "Unable to open file " << file_name;
        exit(1);
    }

    if (myfile.is_open()){
        while ( getline(myfile,line) )
        {
            stringstream ss(line);
            //Ler ponto
            for(int i=0 ; i<3 ; i++)
            {
                ss >> aux;
                points.push_back(aux);
            }
            //Ler normal
            for(int i=0 ; i<3 ; i++){
                ss >> aux;
                v_normals.push_back(aux);
            }
            //Ler coordenada da textura
            for(int i = 0 ; i < 2 ; i++){
                ss >> aux;
                v_textures.push_back(aux);
            }
        }
        myfile.close();
    }

    verticeCount.push_back(points.size()/3);

    cout << "Indice in here is: " << ind << endl;
    GLint value = ind + 1;
    //Get space
    vertices.push_back(value);
    normals.push_back(value);
    textures.push_back(value);
    //Create VBOs
    glGenBuffers(1,&vertices.at(ind));
    cout << "Value: " << value << endl; 
    glBindBuffer(GL_ARRAY_BUFFER, vertices[ind]);
    glBufferData(GL_ARRAY_BUFFER,
                sizeof(float) * points.size(),
                points.data(),
                GL_STATIC_DRAW);
    cout << "Added => Vertices: " << vertices[ind] << " | VerticesCount: " << verticeCount[ind] << endl;
    
    glGenBuffers(1, &normals.at(ind));
    glBindBuffer(GL_ARRAY_BUFFER, normals[ind]);
	glBufferData(GL_ARRAY_BUFFER,
                sizeof(float) * v_normals.size(),
                v_normals.data(),GL_STATIC_DRAW);

    cout << "Added => Normals: " << normals[ind] << " | VerticesCount: " << verticeCount[ind] << endl;

    glGenBuffers(1, &textures.at(ind));
	glBindBuffer(GL_ARRAY_BUFFER, textures[ind]);
	glBufferData(GL_ARRAY_BUFFER,
                sizeof(float) * v_textures.size(),
                v_textures.data(), GL_STATIC_DRAW);

    cout << "Added => Textures: " << textures[ind] << endl;

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
        ModelInfo newModelInfo = loadModelInfo(pListElement);
        //Check if group has texture
        if(newModelInfo.texture.compare("n/a") != 0){
            cout << "Texture in this model: " << newModelInfo.texture << endl;
            //Check if texture is loaded/found
            if(docInfo.textures.count(newModelInfo.texture)){ //Texture is already loaded
                cout << "Texture in this model already loaded: " << newModelInfo.texture << " : " << docInfo.textures[newModelInfo.texture] << endl;
                newModelInfo.texture_id = docInfo.textures[newModelInfo.texture];
            }else{ //Texture is not loaded/found
                cout << "First time seeing this texture" << endl;
                //Load Texture
                newModelInfo.texture_id = loadTexture(newModelInfo.texture);
                cout << "Safely loaded texture with id: " << newModelInfo.texture_id << endl;
                //Save new texture
                docInfo.textures.insert( {newModelInfo.texture,newModelInfo.texture_id} );
            }
        }

        group->models_info.push_back(newModelInfo);

        //Continuar a iterar
        pListElement = pListElement->NextSiblingElement("model");
    }

    return 0;
}

void load_matrix(Group * group, XMLElement * transforms){
    vector<Matrix4> ret;
    XMLError eResult;
    float x,y,z,angle;
    int transformation_index = 0; // Order transformations to save timed ones

    XMLElement * pElement = transforms->FirstChildElement();
    while(pElement != nullptr){
        cout << "Transformation Index: " << transformation_index << endl;
        if(strcmp(pElement->Value(),"translate") == 0){
            Matrix4 res;
            if(pElement->Attribute("time") == NULL){ //Static Translate
                cout << "Static Translate" << endl;
                eResult = pElement->QueryFloatAttribute("x",&x);
                eResult = pElement->QueryFloatAttribute("y",&y);
                eResult = pElement->QueryFloatAttribute("z",&z);

                res.translate(x,y,z);
            }else{ //Time Dependent Translate
                cout << "TimeBased Translate" << endl;
                TimeDependentTranslate *tdt = new TimeDependentTranslate;
                //Save Matrix Index
                tdt->matrix_index = transformation_index;
                //Save Time
                pElement->QueryIntAttribute("time",&(tdt->time));
                //Save Alignment
                if(pElement->Attribute("align","True")) tdt->align = 1;
                else tdt->align = 0;
                //Save Points
                XMLElement * pointLine = pElement->FirstChildElement("point");
                while(pointLine != nullptr){
                    eResult = pointLine->QueryFloatAttribute("x",&x); tdt->coordinates.push_back(x);
                    eResult = pointLine->QueryFloatAttribute("y",&y); tdt->coordinates.push_back(y);
                    eResult = pointLine->QueryFloatAttribute("z",&z); tdt->coordinates.push_back(z);

                    pointLine = pointLine->NextSiblingElement("point");
                }
                //Add to vector of time dependent translations
                group->timeDependentTranslates.push_back(tdt);
            }
            //Add to vector of transformations
            ret.push_back(res);
        }else if(strcmp(pElement->Value(),"rotate") == 0){
            eResult = pElement->QueryFloatAttribute("x",&x);
            eResult = pElement->QueryFloatAttribute("y",&y);
            eResult = pElement->QueryFloatAttribute("z",&z);
            Matrix4 res;
            
            if(pElement->Attribute("time") == NULL){ //Static Rotate
                cout << "Static Rotate" << endl;
                eResult = pElement->QueryFloatAttribute("angle",&angle);
                cout << "Static Rotate Angle: " << angle << endl;
                res.rotate(angle,x,y,z);
            }
            else{ //Time Dependent Rotate
                cout << "TimeBased Rotate" << endl;
                TimeDependentRotate * tdr = new TimeDependentRotate;
                //Save Matrix Index
                tdr->matrix_index = transformation_index;
                //Save Time
                pElement->QueryIntAttribute("time",&(tdr->time));
                //Save Axis
                tdr->coordinates.push_back(x); tdr->coordinates.push_back(y); tdr->coordinates.push_back(z);

                group->timeDependentRotates.push_back(tdr);
                //Load matrix transformation for Time 0
                res.rotate(0,x,y,z);
            }
            ret.push_back(res);
        }else if(strcmp(pElement->Value(),"scale") == 0){
            cout << "Scale" << endl;
            eResult = pElement->QueryFloatAttribute("x",&x);
            eResult = pElement->QueryFloatAttribute("y",&y);
            eResult = pElement->QueryFloatAttribute("z",&z);
            Matrix4 res;
            res.scale(x,y,z);
            ret.push_back(res);
        }
        transformation_index++;
        pElement = pElement->NextSiblingElement();
    }

    group->transformationMatrix = ret;
}

Group* load_group(XMLElement * pList){
    Group * retGroup = new Group;

    //Transformations
    XMLElement * transforms = pList->FirstChildElement("transform");
    if(transforms != nullptr){
        load_matrix(retGroup,transforms);
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

int loadLights(XMLElement * pElement){
    XMLElement * light = pElement->FirstChildElement();
    int light_number = 0;
    float amb[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

    while(light != nullptr && light_number < 8){
        if ( light->Attribute( "type", "point" ) ){
            PointLight newLight = loadPointLight(light,light_number);
            docInfo.vpntLight.push_back(newLight);
        } else if ( light->Attribute( "type", "directional" ) ){
            DirectionalLight newLight = loadDirectionalLight(light,light_number);
            docInfo.vdirLight.push_back(newLight);
        }
        else if ( light->Attribute( "type", "spotlight" ) ){
            SpotlightLight newLight = loadSpotlightLight(light,light_number);
            docInfo.vsptlLight.push_back(newLight);
        }

        light = light->NextSiblingElement();
        light_number++;
    }

    return 0;
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

    //Load Lights
    pElement = pRoot->FirstChildElement("lights");
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;
    loadLights(pElement);

    //Load Groups
    pElement = pRoot->FirstChildElement("group");
    if (pElement == nullptr) return XML_ERROR_PARSING_ELEMENT;
    rootGroup = load_group(pElement);

    camera_x = docInfo.cameraInfo[0][0];
    camera_y = docInfo.cameraInfo[0][1];
    camera_z = docInfo.cameraInfo[0][2];

    camera_dx = docInfo.cameraInfo[1][0];
    camera_dy = docInfo.cameraInfo[1][1];
    camera_dz = docInfo.cameraInfo[1][2];

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

    //Update Time Dependent Transformations
    for(int i = 0 ; i < (g->timeDependentTranslates).size() ; i++){
        TimeDependentTranslate * aux = (g->timeDependentTranslates)[i];
        updateTranslateMatrix(g,aux);
    }
    for(int i = 0 ; i < (g->timeDependentRotates).size() ; i++){
        TimeDependentRotate * aux = (g->timeDependentRotates)[i];
        updateRotateMatrix(g,aux);
    }
    
    //Apply transformation
    for(int i = 0 ; i < (g->transformationMatrix).size() ; i++){
        glMultMatrixf((g->transformationMatrix)[i].get());
    }

    //Render models
    for(int i = 0 ; i < (g->models_indices).size() ; i++){
        int ind = (g->models_indices)[i];
        ModelInfo mi = g->models_info[i];
        
        cout << "Diffuse: " << mi.diffuse[0] << " | " << mi.diffuse[1] << " | " << mi.diffuse[2] << endl;
        cout << "Ambient: " << mi.ambient[0] << " | " << mi.ambient[1] << " | " << mi.ambient[2] << endl;
        cout << "Specular: " << mi.specular[0] << " | " << mi.specular[1] << " | " << mi.specular[2] << endl;
        cout << "Emissive: " << mi.emissive[0] << " | " << mi.emissive[1] << " | " << mi.emissive[2] << endl;
        cout << "Shininess: " << mi.shininess << endl;

        GLfloat materialDiffuse[] = {mi.diffuse[0],mi.diffuse[1],mi.diffuse[2],1.0};
	    GLfloat materialAmbient[] = {mi.ambient[0],mi.ambient[1],mi.ambient[2],1.0};
	    GLfloat materialSpecular[] = {mi.specular[0],mi.specular[1],mi.specular[2],1.0};
	    GLfloat materialEmissive[] = {mi.emissive[0],mi.emissive[1],mi.emissive[2],1.0};
	    GLfloat materialShininess[] = {mi.shininess};

        glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);
	    glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);
	    glMaterialfv(GL_FRONT, GL_EMISSION, materialEmissive);
        glMaterialfv(GL_FRONT, GL_SHININESS, materialShininess);

        cout << "Vertices indice is: " << vertices[ind] << endl;
        glBindBuffer(GL_ARRAY_BUFFER,vertices[ind]);
        glVertexPointer(3,GL_FLOAT,0,0);

        cout << "Normals indice is: " << normals[ind] << endl;
        glBindBuffer(GL_ARRAY_BUFFER,normals[ind]);
	    glNormalPointer(GL_FLOAT,0,0);

        if(mi.texture.compare("n/a") != 0){ //Draw with texture if model has it
            glBindTexture(GL_TEXTURE_2D,mi.texture_id);
        }else{
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        glBindBuffer(GL_ARRAY_BUFFER, textures[ind]);
	    glTexCoordPointer(2,GL_FLOAT,0,0);

        glDrawArrays(GL_TRIANGLES,0,verticeCount[ind]);
        glBindTexture(GL_TEXTURE_2D, 0); //Reset
    }
    
    //Render sub-groups
    for(int i = 0 ; i < (g->groups).size() ; i++){
        renderGroup((g->groups)[i]);
    }

    //Obtain matrix before transformations from this group
    glPopMatrix();
}

void renderScene(){
    //Clear Buffers
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //Set The Camera
    glLoadIdentity();
    if(camera_mode == 1){
        orbitalCamera();
    }else if(camera_mode == 2){
        fpsCamera();
    }else{
        gluLookAt(docInfo.cameraInfo[0][0],docInfo.cameraInfo[0][1],docInfo.cameraInfo[0][2],
        docInfo.cameraInfo[1][0],docInfo.cameraInfo[1][1],docInfo.cameraInfo[1][2],
        docInfo.cameraInfo[2][0],docInfo.cameraInfo[2][1],docInfo.cameraInfo[2][2]);
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
    //Lights
    renderLight();

    //Rendering
    renderGroup(rootGroup);

    //End of Frame
    glutSwapBuffers();
}

void walkFront(){
    camera_x -= 7.77f * camera_dx;
    camera_y -= 7.77f * camera_dy;
    camera_z -= 7.77f * camera_dz;
}

void walkBack(){
    camera_x += 7.77f * camera_dx;
    camera_y += 7.77f * camera_dy;
    camera_z += 7.77f * camera_dz;
}

void walkRight(){
    float aux_camera_dx = cosf(camera_beta) * sinf(camera_alpha+M_PI_2);
    float aux_camera_dz = cosf(camera_beta) * cosf(camera_alpha+M_PI_2);

    camera_x += 7.77f * aux_camera_dx;
    camera_z += 7.77f * aux_camera_dz;
}

void walkLeft(){
    float aux_camera_dx = cosf(camera_beta) * sinf(camera_alpha+M_PI_2);
    float aux_camera_dz = cosf(camera_beta) * cosf(camera_alpha+M_PI_2);

    camera_x -= 7.77f * aux_camera_dx;
    camera_z -= 7.77f * aux_camera_dz;
}

void processKeys(unsigned char key, int xx, int yy) {
    //Code to process keys
	switch (key){
        case 'w':
            camera_mode=2;
            walkFront();
            break;
        case 's':
            camera_mode=2;
            walkBack();
            break;
        case 'a':
            camera_mode=2;
            walkLeft();
            break;
        case 'd':
            camera_mode=2;
            walkRight();
            break;
        case 'i': //reset inicial values
            camera_mode = 0;

            camera_alpha = 0.0f;
            camera_beta = 0.0f;
            camera_radius = 250.0f;

            camera_x = docInfo.cameraInfo[0][0];
            camera_y = docInfo.cameraInfo[0][1];
            camera_z = docInfo.cameraInfo[0][2];

            camera_dx = docInfo.cameraInfo[1][0];
            camera_dy = docInfo.cameraInfo[1][1];
            camera_dz = docInfo.cameraInfo[1][2];
            break;
        case 'm':
            if(camera_mode==2 || camera_mode==0) {
                camera_mode = 1;
                camera_alpha = 0.0f;
                camera_beta = 0.0f;
            }else{
                camera_mode = 2;
            }
            break;
        default:
            break;
	}
	glutPostRedisplay();
}

void processMouseClick(int button, int state, int x, int y){
    if(camera_mode==0) camera_mode = 1;

    switch (button) {
        case(GLUT_LEFT_BUTTON):
            if(state==GLUT_DOWN){
                tracking=1;
                mousePosX=(float)x;
                mousePosY=(float)y;
            }
            else {
                tracking = 0;
            }
            break;
        case (3):
            camera_radius += 1.5f;
            break;
        case(4):
            camera_radius -= 1.5f;
            if (camera_radius < 1.0f)
                camera_radius = 1.0f;
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

void processMouseMotion(int x, int y){
    if(!tracking)
        return;

    float deltaX = (float)x-mousePosX;
    float deltaY = (float)y-mousePosY;
    mousePosX=(float)x;
    mousePosY=(float)y;
    camera_alpha -= deltaX/400;
    camera_beta += deltaY/400;
    if (camera_beta > 1.5f)
        camera_beta = 1.5f;
    if (camera_beta < -1.5f)
        camera_beta = -1.5f;
    glutPostRedisplay();
}

void printInfo() {
    printf("\n\nVendor: %s", glGetString(GL_VENDOR));
    printf("\nRenderer: %s", glGetString(GL_RENDERER));
    printf("\nVersion: %s", glGetString(GL_VERSION));

    printf("\n\nUse the mouse and 'W A S D' to move the camera, scroll is used to zoom in and zoom out");
    printf("\nTo change camera mode use 'M' and to reset camera to xml settings use 'I'\n");
    fflush(stdout);
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

    //Init GLUT and the Window
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DEPTH|GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowPosition(100,100);
    glutInitWindowSize(800,800);
    glutCreateWindow(file_name);

    //Required callback registry
    glutDisplayFunc(renderScene);
    glutReshapeFunc(changeSize);
    glutIdleFunc(renderScene);

    //Callback registration for keyboard processing
	glutKeyboardFunc(processKeys);
    glutMouseFunc(processMouseClick);
    glutMotionFunc(processMouseMotion);
    glutPassiveMotionFunc(processMousePassiveMotion);

    //Init GLEW
    #ifndef __APPLE__
        glewInit();
    #endif

    //OpenGL Settings
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_RESCALE_NORMAL);

    if( loadFileInfo(pRoot) != 0)
    {
        cout << "Error loading file!" << endl;
        return 1;
    }

    //Load 3d files to vertices arrays
    cout << "Starting to prepare data\n";
    for (int i = 0 ; i < docInfo.models.size() ; i++)
    {
        const char * aux = docInfo.models[i].c_str();
        prepareData(i,aux);
    }

    //Enter GLUT's main cycle
    timebase = glutGet(GLUT_ELAPSED_TIME);
    printInfo();
    glutMainLoop();

    return 0;
}