#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string.h>
#include <algorithm>

#define _USE_MATH_DEFINES
#include <math.h>



typedef struct point{
    float x;
    float y;
    float z;
    float nx;
    float ny;
    float nz;
    float tx;
    float ty;
}*Point;

typedef struct simplePoint{
    float x;
    float y;
    float z;
}*SimplePoint;

Point new_point(float x, float y, float z, float nx, float ny, float nz, float tx, float ty) {
    Point p = (Point) (malloc(sizeof(struct point)));
    p->x = x; p->y = y; p->z = z;
    p->nx = nx; p->ny = ny; p->nz = nz;
    p->tx = tx; p->ty = ty;
    return p;
}

SimplePoint new_simplePoint(float x, float y, float z) {
    SimplePoint p = (SimplePoint) (malloc(sizeof(struct simplePoint)));
    p->x = x; p->y = y; p->z = z;
    return p;
}

void write_point(float x, float y, float z, float nx, float ny, float nz, float tx, float ty, std::ofstream& file){
    file << std::to_string(x) << " "
         << std::to_string(y) << " "
         << std::to_string(z) << " "
         << std::to_string(nx) << " "
         << std::to_string(ny) << " "
         << std::to_string(nz) << " "
         << std::to_string(tx) << " "
         << std::to_string(ty) << " ";
}

SimplePoint getVector(Point v1, Point v2){
    return new_simplePoint(v2->x - v1->x,
                     v2->y - v1->y,
                     v2->z - v1->z);
}
SimplePoint getVector(SimplePoint v1, SimplePoint v2){
    return new_simplePoint(v2->x - v1->x,
                     v2->y - v1->y,
                     v2->z - v1->z);
}

Point joinPointVector(SimplePoint point, SimplePoint vector, float tx, float ty){
    return new_point(point->x, point->y, point->z, vector->x, vector->y, vector->z, tx, ty);
}

void normalizeVector_Point(Point& vec){
    float length = sqrt( vec->nx * vec->nx +
                         vec->ny * vec->ny +
                         vec->nz * vec->nz);
    if (length == 0.0f)
        length = 1.0f;
    vec->nx /= length;
    vec->ny /= length;
    vec->nz /= length;
}
void normalizeVector(SimplePoint& vec){
    float length = sqrt( vec->x * vec->x +
                         vec->y * vec->y +
                         vec->z * vec->z);
    if (length == 0.0f)
        length = 1.0f;
    vec->x /= length;
    vec->y /= length;
    vec->z /= length;
}

SimplePoint normal_at_point_torus(float x, float y, float z, float radius){
    SimplePoint p0 = new_simplePoint(x,0,z); //ponto projetado com y=0 que é a altura do meio do torus

    normalizeSimpleVector(p0); //norma 1

    p0->x*=radius;// multiplicamos pelo raio para ficar na circunferencia central
    p0->y*=radius;
    p0->z*=radius;

    //std::cout<<"Raio: "<<p0->z<<", "<<p0->y<<", "<<p0->z<<"\n";
    return p0;
}


void write_point(Point p, std::ofstream& file) {
    file << std::to_string(p->x) << " "
         << std::to_string(p->y) << " "
         << std::to_string(p->z) << " "
         << std::to_string(p->nx) << " "
         << std::to_string(p->ny) << " "
         << std::to_string(p->nz) << " "
         << std::to_string(p->tx) << " "
         << std::to_string(p->ty) << " ";
}

long factorial(int n) {
    long factorial = 1;
    for (int i = 2; i <= n; i++)
        factorial = factorial * i;
    //printf("FACTORIAL %d: %d\n",n,factorial);
    return factorial;
    
}
 
long nCr(int n, int r) {
    return factorial(n) / (factorial(r) * factorial(n - r)); 
}

SimplePoint bernsteins_polinomials(float t, Point p0, Point p1, Point p2, Point p3){//mudar para simple point
    Point p =(Point)malloc(sizeof(float)*3);
    //B(t) = t³*P3 + 3t²*(1-t)*P2 + 3t*(1-t)²*P1 + (1-t)³*P0
    p->x = pow(t,3) * p3->x + 3*pow(t,2) * (1-t)* p2->x + 3*t* pow(1-t,2) * p1->x + pow(1-t,3) * p0->x;
    p->y = pow(t,3) * p3->y + 3*pow(t,2) * (1-t)* p2->y + 3*t* pow(1-t,2) * p1->y + pow(1-t,3) * p0->y;
    p->z = pow(t,3) * p3->z + 3*pow(t,2) * (1-t)* p2->z + 3*t* pow(1-t,2) * p1->z + pow(1-t,3) * p0->z;

    return p;
}

Point *get_patch_points(Point points[], int patch[], int N){
    Point *pts =(Point*)malloc(sizeof(Point)*N);
    for(int i=0; i<N; i++){
        pts[i]=(Point)malloc(sizeof(float)*3);
        pts[i]->x = points[patch[i]]->x;
        pts[i]->y = points[patch[i]]->y;
        pts[i]->z = points[patch[i]]->z;
    }
    return pts;
}

void bezier_patch(std::ifstream &infile, std::ofstream &file, int tecel){

    Point *ctrl_points; 

    int line_counter=0,     //contar a linha do ficheiro conforme lẽ
        patch_counter=0,    //contar o número do patch conforme constrói a estrutura de dados index_patch
        point_counter=0,    //contar o número de pontos conforme constrói a estrutura de dados ctrl_points
        n_patches=0,        //guardar número de patches
        nr_points=16,        //guardar número de pontos por patch
        **index_patch;      //matriz em que cada linha corresponde a um patch. Cada elemento da linha corresoinde ao ponto nessa posição do patch

    
    std::string line;
    while (std::getline(infile, line)){
        std::istringstream iss(line);

        //PRIMEIRA LINHA: guardar nr_patches e alocar espaço para a estrutura dos pacthes
        if(line_counter == 0){ 
            n_patches = stoi(line);
            index_patch = (int**)malloc(sizeof(int*)*n_patches);
        }

        //SEGUNDA ATÉ N_PATCHES: 
        else if(line_counter <= n_patches){
            char* line_c = strcpy(new char[line.length() + 1], line.c_str()); //converte de string para char* pra usar strtok

            index_patch[line_counter-1] =(int*)malloc(sizeof(int)*nr_points); //aloca espaço para os pontos do patch

            char *ptr = strtok(line_c, ",");   
            int i=0; 
            while (ptr != NULL){  
                index_patch[line_counter-1][i++] = atoi(ptr); //guarda os pontos do patch na linha do patch atual                  //std::cout<<"index_patch["<<line_counter-1 << "][" << i-1 << "] = "<< index_patch[line_counter-1][i-1]<<"\n";
                ptr = strtok (NULL, " , ");  
            }  
        }
        //N_PATCHES + 1: Aloca espaço para x pontos (lê o x)
        else if(line_counter == n_patches +1)
            ctrl_points =(Point*)malloc(sizeof(Point)*stoi(line));

        //PONTOS: guarda os pontos na estrutura ctrl_points pelo indice correspondente à ordem que aparecem
        else{
            ctrl_points[point_counter] =(Point)malloc(sizeof(float)*3); 
            char* line_c = strcpy(new char[line.length() + 1], line.c_str());
            char *ptr; // declare a ptr pointer  

            ptr = strtok(line_c, ","); ctrl_points[point_counter]->x = atof(ptr); 
            ptr = strtok (NULL, ","); ctrl_points[point_counter]->y = atof(ptr);
            ptr = strtok (NULL, " , "); ctrl_points[point_counter]->z = atof(ptr);                                                  //std::cout<<"Ponto "<<point_counter<<": ( "<<ctrl_points[point_counter]->x<<", " <<ctrl_points[point_counter]->y <<" , "<< ctrl_points[point_counter]->z<<" )\n";
            point_counter++;
        }
        line_counter++;
    }

    float t =(float) 1/tecel; //fração de tecelagem

    Point final_pts[n_patches][tecel+1][tecel+1]; //guarda os pontos de cada patch gerado por tecelagem
    
    for(int i=0; i<n_patches; i++){
        Point *pts = get_patch_points(ctrl_points,index_patch[i], nr_points); //função para get os 16 pts do patch

        for(int k=0; k<=tecel; k++){
            Point p0 = bernsteins_polinomials(k*t,pts[0], pts[1], pts[2], pts[3]); //pontos de controlo por k
            Point p1 = bernsteins_polinomials(k*t,pts[4], pts[5], pts[6], pts[7]);
            Point p2 = bernsteins_polinomials(k*t,pts[8], pts[9], pts[10], pts[11]);
            Point p3 = bernsteins_polinomials(k*t,pts[12], pts[13], pts[14], pts[15]);

            for(int v=0; v<=tecel ; v++){
                final_pts[i][k][v]=bernsteins_polinomials(v*t, p0,p1,p2,p3); //ponto de controlo de v
            }
        }
    }
    //escrever os pontos na ordem certa de forma a fazer triângulos
    for(int i=0; i<n_patches; i++){
        for(int k=0; k<tecel; k++){
            for(int v=0; v<tecel; v++){
                write_point(final_pts[i][k][v], file);
                write_point(final_pts[i][k+1][v], file);
                write_point(final_pts[i][k][v+1], file);  
                file<<std::endl;

                write_point(final_pts[i][k][v+1], file);
                write_point(final_pts[i][k+1][v], file);
                write_point(final_pts[i][k+1][v+1], file);
                file<<std::endl;
            }
        }
    }
}


void gen_bezier(char** args){
    
    std::ifstream infile;
    infile.open(args[2]);

    int tecelagem = atoi(args[3]);
    std::ofstream file;
    file.open(args[4]);

    bezier_patch(infile,file, tecelagem);
}

void drawTorusRing(int mainSegments, int tubeSegments, float mainRadius, float tubeRadius, std::ofstream& file){
    std::cout<<"Main radius: "<<mainRadius<<"\n";
	float phiAngleStep = (2 * (float)M_PI) / float(mainSegments);
	float thetaAngleStep = (2 * (float)M_PI) / float(tubeSegments);
	float currentPhi = 0.0f;

	for(int i = 0 ; i < mainSegments ; i++){
		float sinPhi = sinf(currentPhi);
		float cosPhi = cosf(currentPhi);
		float nextSinPhi = sinf(currentPhi + phiAngleStep);
		float nextCosPhi = cosf(currentPhi + phiAngleStep);
		float currentTheta = 0.0f;
		
		for(int j = 0 ; j < tubeSegments ; j++){
			float sinTheta = sinf(currentTheta);
			float cosTheta = cosf(currentTheta);
			float nextSinTheta = sinf(currentTheta + thetaAngleStep);
			float nextCosTheta = cosf(currentTheta + thetaAngleStep);

			//Point 1 Coords
			float x_1 = (mainRadius + tubeRadius * cosTheta) * cosPhi;
			float y_1 = (mainRadius + tubeRadius * cosTheta) * sinPhi;
			float z_1 = tubeRadius * sinTheta;
            SimplePoint center1 = normal_at_point_torus(x_1,y_1,z_1,mainRadius); //calcula o ponto no raio interno do torus que intercecionava a projeção do ponto em y=0 e essa circunferencia 
            SimplePoint aux1 = new_simplePoint(x_1,y_1,z_1); 
            SimplePoint n1 = getVector(center1,aux1); //vetor    
            normalizeVector(n1);
            Point p1 = joinPointVector(aux1, n1);
            free(center1);free(aux1);free(n1);
            
			//Point 2 Coords
			float x_2 = (mainRadius + tubeRadius * nextCosTheta) * nextCosPhi;
			float y_2 = (mainRadius + tubeRadius * nextCosTheta) * nextSinPhi;
			float z_2 = tubeRadius * nextSinTheta;
            SimplePoint center2 = normal_at_point_torus(x_2,y_2,z_2,mainRadius);
            SimplePoint aux2 = new_simplePoint(x_2,y_2,z_2);
            SimplePoint n2 = getVector(center2,aux2);
            normalizeVector(n2);
            Point p2 = joinPointVector(aux2, n2);
            free(center2);free(aux2);free(n2);

			//Point 3 Coords
			float x_3 = (mainRadius + tubeRadius * nextCosTheta) * cosPhi;
			float y_3 = (mainRadius + tubeRadius * nextCosTheta) * sinPhi;
			float z_3 = tubeRadius * nextSinTheta;
            SimplePoint center3 = normal_at_point_torus(x_3,y_3,z_3,mainRadius);
            SimplePoint aux3 = new_simplePoint(x_3,y_3,z_3);
            SimplePoint n3 = getVector(center3,aux3);
            normalizeVector(n3);
            Point p3 = joinPointVector(aux3, n3);
            free(center3);free(aux3);free(n3);

			//Point 4 Coords
			float x_4 = (mainRadius + tubeRadius * cosTheta) * nextCosPhi;
			float y_4 = (mainRadius + tubeRadius * cosTheta) * nextSinPhi;
			float z_4 = tubeRadius * sinTheta;
            SimplePoint center4 = normal_at_point_torus(x_4,y_4,z_4,mainRadius);
            SimplePoint aux4 = new_simplePoint(x_4,y_4,z_4);
            SimplePoint n4 = getVector(center4,aux4);
            normalizeVector(n4);
            Point p4 = joinPointVector(aux4, n4);
            free(center4);free(aux4);free(n4);

			write_point(p1,file);
            file<<std::endl;
			write_point(p2,file);
            file<<std::endl;
			write_point(p3,file);
            file<<std::endl;

			write_point(p1,file);
            file<<std::endl;
			write_point(p4,file);
            file<<std::endl;
			write_point(p2,file);
            file<<std::endl;

            free(p1);free(p2);free(p3);free(p4);
            
            currentTheta += thetaAngleStep;
		}
		currentPhi += phiAngleStep;
	}
}
int gen_torus(char** args){
    int mainSegments = std::atoi(args[2]);
    int tubeSegments = std::atoi(args[3]);
    float mainRadius = (float) std::atoi(args[4]);
    float tubeRadius = (float) std::atoi(args[5]);

    std::ofstream file;
    file.open(args[6]);

    drawTorusRing(mainSegments,tubeSegments,mainRadius,tubeRadius,file);

    file.close();
    return 0;
}

int gen_sphere(char** args){
    float radius = (float) std::atoi(args[2]);
    int slices = std::atoi(args[3]);
    int stacks = std::atoi(args[4]);

    std::ofstream file;
    file.open(args[5]);
    float ang_stack = M_PI / stacks;
	float ang_slice = (2 * (float)M_PI) / (float)slices;

    SimplePoint center = new_simplePoint(0.0f, 0.0f, 0.0f); // ?????

	for(int i = 0 ; i < slices ; i++){ //Iterate Slices
		float alpha = (float)i * ang_slice;
		float next_alpha = (float)(i+1) * ang_slice;
    	float cosAlpha = cosf(alpha);
		float nextCosAlpha = cosf(next_alpha);
		float sinAlpha = sinf(alpha);
		float nextSinAlpha = sinf(next_alpha);

		for(int j = 0 ; j < stacks ; j++){ //Iterate Stacks
			float beta = (float)j*ang_stack - (float)M_PI/2;
			float next_beta = (float)(j+1)*ang_stack - (float)M_PI/2;
			float cosBeta = cosf(beta);
			float nextCosBeta = cosf(next_beta);
			float sinBeta = sinf(beta);
			float nextSinBeta = sinf(next_beta);

			//Point 1 Coords
			float x_1 = radius * cosBeta * sinAlpha;
			float y_1 = radius * sinBeta;
			float z_1 = radius * cosBeta * cosAlpha;
            SimplePoint aux1 = new_simplePoint(x_1, y_1, z_1);
            SimplePoint n1 = getVector(center, aux1);
            normalizeVector(n1);
            Point p1 = joinPointVector(aux1, n1);
            free(aux1);free(n1);

			//Point 2 Coords
			float x_2 = radius * nextCosBeta * nextSinAlpha;
			float y_2 = radius * nextSinBeta;
			float z_2 = radius * nextCosBeta * nextCosAlpha;
            SimplePoint aux2 = new_simplePoint(x_2, y_2, z_2);
            SimplePoint n2 = getVector(center, aux2);
            normalizeVector(n2);
            Point p2 = joinPointVector(aux2, n2);
            free(aux2);free(n2);

			//Point 3 Coords
			float x_3 = radius * nextCosBeta * sinAlpha;
			float y_3 = radius * nextSinBeta;
			float z_3 = radius * nextCosBeta * cosAlpha;
            SimplePoint aux3 = new_simplePoint(x_3, y_3, z_3);
            SimplePoint n3 = getVector(center, aux3);
            normalizeVector(n3);
            Point p3 = joinPointVector(aux3, n3);
            free(aux3);free(n3);

			//Point 4 Coords
			float x_4 = radius * cosBeta * nextSinAlpha;
			float y_4 = radius * sinBeta;
			float z_4 = radius * cosBeta * nextCosAlpha;
            SimplePoint aux4 = new_simplePoint(x_4, y_4, z_4);
            SimplePoint n4 = getVector(center, aux4);
            normalizeVector(n4);
            Point p4 = joinPointVector(aux4, n4);
            free(aux4);free(n4);

			write_point(p1,file);
            file<<std::endl;
			write_point(p2,file);
            file<<std::endl;
			write_point(p3,file);
            file<<std::endl;

			write_point(p1,file);
            file<<std::endl;
			write_point(p4,file);
            file<<std::endl;
			write_point(p2,file);
            file<<std::endl;

            free(p1);free(p2);free(p3);free(p4);
		}
	}

    file.close();
    return 0;
}

void box_front_back(float size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------FRONT & BACK--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            //BACK
            float p1x=(float)j*sub_size - (size/2);
            float p1y=(float)i*sub_size - (size/2);
            float p1z=0 - (size/2);                                          
                                                                                
            float p2x=p1x;
            float p2y=p1y+sub_size;
            float p2z=p1z;

            float p3x=p1x+sub_size;
            float p3y=p1y;
            float p3z=p1z;

            float p4x=p1x;
            float p4y=p1y+sub_size;
            float p4z=p1z;

            float p5x=p4x+sub_size;
            float p5y=p4y;
            float p5z=p4z;

            float p6x=p4x+sub_size;
            float p6y=p4y-sub_size;
            float p6z=p4z;

            //FRONT
            float p7x=p2x;
            float p7y=p2y;                                                 
            float p7z=p2z+size;

            float p9x=p3x;
            float p9y=p3y;
            float p9z=p3z+size;                                                         
                                                                                
            float p8x=p1x;
            float p8y=p1y;
            float p8z=p1z+size;

            float p10x=p6x;
            float p10y=p6y;
            float p10z=p6z+size;
            float p11x=p5x;
            float p11y=p5y;
            float p11z=p5z+size;

            float p12x=p4x;
            float p12y=p4y;
            float p12z=p4z+size;

            write_point(p1x, p1y, p1z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p2x, p2y, p2z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p3x, p3y, p3z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;

            write_point(p4x, p4y, p4z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p5x, p5y, p5z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p6x, p6y, p6z, 0.0f, 0.0f, -1.0f, ????, ?????, file);
            file<<std::endl;

            write_point(p7x, p7y, p7z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p8x, p8y, p8z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p9x, p9y, p9z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;

            write_point(p10x, p10y, p10z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p11x, p11y, p11z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;
            write_point(p12x, p12y, p12z, 0.0f, 0.0f, 1.0f, ????, ?????, file);
            file<<std::endl;

        }
    }
}
void box_left_right(float size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------LEFT & RIGHT--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            //LEFT
            float p1x= 0 -(size/2);
            float p1y=(float)i*sub_size - (size/2);
            float p1z=(float)j*sub_size - (size/2);

            float p2x=p1x;
            float p2y=p1y;
            float p2z=p1z+sub_size;

            float p3x=p1x;
            float p3y=p1y+sub_size;
            float p3z=p1z;

            float p4x=p1x;
            float p4y=p1y+sub_size;
            float p4z=p1z;

            float p5x=p4x;
            float p5y=p4y-sub_size;
            float p5z=p4z+sub_size;

            float p6x=p4x;
            float p6y=p4y;
            float p6z=p4z+sub_size;

            //RIGHT
            float p7x=p3x+size;
            float p7y=p3y;
            float p7z=p3z;                                            
                                                                          
            float p8x=p2x+size;
            float p8y=p2y;
            float p8z=p2z;

            float p9x=p1x+size;
            float p9y=p1y;                                                 
            float p9z=p1z;

            float p10x=p6x+size;
            float p10y=p6y;
            float p10z=p6z;

            float p11x=p5x+size;
            float p11y=p5y;
            float p11z=p5z;

            float p12x=p4x+size;
            float p12y=p4y;
            float p12z=p4z;

            write_point(p1x, p1y, p1z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p2x, p2y, p2z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p3x, p3y, p3z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;

            write_point(p4x, p4y, p4z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p5x, p5y, p5z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p6x, p6y, p6z, -1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;

            write_point(p7x, p7y ,p7z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p8x, p8y ,p8z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p9x, p9y ,p9z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;

            write_point(p10x, p10y, p10z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p11x, p11y, p11z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p12x, p12y, p12z, 1.0f, 0.0f, 0.0f, ???, ???, file);
            file<<std::endl;
        }
    }
}
void box_top_bottom(float size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------TOP & BOTTOM--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            
            //BOTTOM
            float p1x=(float)i*sub_size - (size/2);
            float p1y=0 - (size/2);                                                          
            float p1z=(float)j*sub_size - (size/2);
                                                                                
            float p2x=p1x+sub_size;
            float p2y=p1y;
            float p2z=p1z;

            float p3x=p1x;
            float p3y=p1y;
            float p3z=p1z+sub_size;

            float p4x=p1x;
            float p4y=p1y;
            float p4z=p1z+sub_size;

            float p5x=p4x+sub_size;
            float p5y=p4y;
            float p5z=p4z-sub_size;

            float p6x=p4x+sub_size;
            float p6y=p4y;
            float p6z=p4z;

            //TOP
            float p7x=p3x;
            float p7y=p3y+size;
            float p7z=p3z;                                                     
                                                                                
            float p8x=p2x;
            float p8y=p2y+size;
            float p8z=p2z;

            float p9x=p1x;
            float p9y=p1y+size;                                                          
            float p9z=p1z;
            
            float p10x=p6x;
            float p10y=p6y+size;
            float p10z=p6z;

            float p11x=p5x;
            float p11y=p5y+size;
            float p11z=p5z;

            float p12x=p4x;
            float p12y=p4y+size;
            float p12z=p4z;

            write_point(p1x, p1y, p1z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p2x, p2y, p2z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p3x, p3y, p3z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;

            write_point(p4x, p4y, p4z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p5x, p5y, p5z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p6x, p6y, p6z, 0.0f, -1.0f, 0.0f, ???, ???, file);
            file<<std::endl;

            write_point(p7x, p7y, p7z, 0.0f, 1.0f, 0.0f, ????, ???, file);
            file<<std::endl;
            write_point(p8x, p8y, p8z, 0.0f, 1.0f, 0.0f, ????, ???, file);
            file<<std::endl;
            write_point(p9x, p9y, p9z, 0.0f, 1.0f, 0.0f, ????, ???, file);
            file<<std::endl;

            write_point(p10x, p10y, p10z, 0.0f, 1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p11x, p11y, p11z, 0.0f, 1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
            write_point(p12x, p12y, p12z, 0.0f, 1.0f, 0.0f, ???, ???, file);
            file<<std::endl;
        }
    }
}
int gen_box(char** args){
    int size = std::atoi(args[2]);
    int grid = std::atoi(args[3]);
    float sub_size = (float)size/(float)grid;

    std::ofstream file;
    file.open(args[4]);
    
    box_left_right((float)size,grid,sub_size,file);
    box_top_bottom((float)size,grid,sub_size,file);
    box_front_back((float)size,grid,sub_size,file);

    file.close();
    return 0;
}

int gen_plane(char** args){
    float len = (float)std::atoi(args[2]);
    int divisions = std::atoi(args[3]);
    float sub_size = (float)len/(float)divisions;

    float textureIncrement = 1/(divisions+1);
    float tx = 0.0f;
    float ty = 0.0f;

    std::ofstream file;
    file.open(args[4]);

    for(int i=0; i<divisions ; i++){
        for(int j=0; j<divisions; j++){
            float p1x=(float)i*sub_size-(len/2);
            float p1y=0;                                                          
            float p1z=(float)j*sub_size-(len/2);

            float p2x=p1x;
            float p2y=p1y;
            float p2z=p1z+sub_size;                                              
                                                                                
            float p3x=p1x+sub_size;
            float p3y=p1y;
            float p3z=p1z;

            float p4x=p1x;
            float p4y=p1y;
            float p4z=p1z+sub_size;

            float p5x=p4x+sub_size;
            float p5y=p4y;
            float p5z=p4z;

            float p6x=p4x+sub_size;
            float p6y=p4y;
            float p6z=p4z-sub_size;

            

            write_point(p1x, p1y, p1z, 0.0f, 1.0f, 0.0f, tx, ty, file);
            file<<std::endl;
            write_point(p2x, p2y, p2z, 0.0f, 1.0f, 0.0f, tx, ty+textureIncrement, file);
            file<<std::endl;
            write_point(p3x, p3y, p3z, 0.0f, 1.0f, 0.0f, tx+textureIncrement, ty, file);
            file<<std::endl;

            write_point(p4x, p4y, p4z, 0.0f, 1.0f, 0.0f, tx, ty+textureIncrement, file);
            file<<std::endl;
            write_point(p5x, p5y, p5z, 0.0f, 1.0f, 0.0f, tx+textureIncrement, ty+textureIncrement, file);
            file<<std::endl;
            write_point(p6x, p6y, p6z, 0.0f, 1.0f, 0.0f, tx+textureIncrement, ty, file);
            file<<std::endl;

            ty+=textureIncrement;
        }
        ty = 0.0f;
        tx += textureIncrement;
    }
    
    file.close();
    return 0;
}

int check_args(int n, char **args){
    for(int i = 0; i < n; i++)
        if(!args[i] || !*args[i]) return 0;
    return 1;
}


int main(int argc, char **argv) {
    /*
    Point p1 = new_point(0,0,0);
    Point p3 = new_point(1,0,0);
    Point p4 = new_point(0,0,1);
    Point p2 = new_point(1,0,1);
    Point p = normal_vector_4p(p1,p2,p3,p4);
    std::cout<<p->x<<", "<<p->y<<", "<<p->z;*/

    if(!argv[1] || !*argv[1]){
        std::cout<<"Invalid input"<<std::endl;
        return 1;
    }
    
    std::string figure = argv[1];

    if(figure == "sphere" && check_args(4, argv+2))
        gen_sphere(argv);

    else if (figure == "plane" && check_args(3, argv+2))
        gen_plane(argv);

    else if (figure == "cone" && check_args(5, argv+2))
        gen_cone(argv);

    else if (figure == "box" && check_args(3, argv+2))
        gen_box(argv);

    else if (figure == "torus" && check_args(4, argv+2))
        gen_torus(argv);
    
    else if (figure == "bezier" && check_args(3, argv+2))
        gen_bezier(argv);

    else {
        std::cout<<"Invalid input"<<std::endl;
        return 1;
    }

    return 0;
}