#include "pointsANDvectors.h"



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

void normalizeVector(Point& vec){
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

    normalizeVector(p0); //norma 1

    p0->x*=radius;// multiplicamos pelo raio para ficar na circunferencia central
    p0->y*=radius;
    p0->z*=radius;

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

SimplePoint normal_cone(float x, float y, float z,float height, float radius, float declive){
    SimplePoint vetor;
    if(y == height) 
        vetor = new_simplePoint(0.0f, 1.0f, 0.0f);
    else if(y==0){
        vetor = new_simplePoint(x, 0.0f, z);
    }
    else{
        vetor = new_simplePoint(x, declive * sqrt(pow(x,2)+pow(z,2)) , z);
        normalizeVector(vetor);
    }
    return vetor;
}

SimplePoint bernsteins_polinomials(float t, SimplePoint p0, SimplePoint p1, SimplePoint p2, SimplePoint p3){//mudar para simple point
    SimplePoint p =(SimplePoint)malloc(sizeof(float)*3);
    //B(t) = t³*P3 + 3t²*(1-t)*P2 + 3t*(1-t)²*P1 + (1-t)³*P0
    p->x = pow(t,3) * p3->x + 3*pow(t,2) * (1-t)* p2->x + 3*t* pow(1-t,2) * p1->x + pow(1-t,3) * p0->x;
    p->y = pow(t,3) * p3->y + 3*pow(t,2) * (1-t)* p2->y + 3*t* pow(1-t,2) * p1->y + pow(1-t,3) * p0->y;
    p->z = pow(t,3) * p3->z + 3*pow(t,2) * (1-t)* p2->z + 3*t* pow(1-t,2) * p1->z + pow(1-t,3) * p0->z;

    return p;
}

SimplePoint bernsteins_derivative(float t, SimplePoint p0, SimplePoint p1, SimplePoint p2, SimplePoint p3){//mudar para simple point
    SimplePoint p =(SimplePoint)malloc(sizeof(float)*3);
    //B(t) = t³*P3 + 3t²*(1-t)*P2 + 3t*(1-t)²*P1 + (1-t)³*P0
    p->x = 3*pow(t,2) * p3->x + (-9*pow(t,2)+6*t) * p2->x + (9*pow(t,2)-12*t+3) * p1->x + (-3*pow(t,2)+6*t-3) * p0->x;
    p->y = 3*pow(t,2) * p3->y + (-9*pow(t,2)+6*t) * p2->y + (9*pow(t,2)-12*t+3) * p1->y + (-3*pow(t,2)+6*t-3) * p0->y;
    p->z = 3*pow(t,2) * p3->z + (-9*pow(t,2)+6*t) * p2->z + (9*pow(t,2)-12*t+3) * p1->z + (-3*pow(t,2)+6*t-3) * p0->z;

    return p;
}

SimplePoint cross(SimplePoint a, SimplePoint b){
    return new_simplePoint(a->y*b->z - a->z*b->y,
                           a->z*b->x - a->x*b->z,
                           a->x*b->y - a->y*b->x);
}

SimplePoint *get_patch_points(SimplePoint points[], int patch[], int N){
    SimplePoint *pts =(SimplePoint*)malloc(sizeof(Point)*N);
    for(int i=0; i<N; i++){
        pts[i]=(SimplePoint)malloc(sizeof(float)*3);
        pts[i]->x = points[patch[i]]->x;
        pts[i]->y = points[patch[i]]->y;
        pts[i]->z = points[patch[i]]->z;
    }
    return pts;
}