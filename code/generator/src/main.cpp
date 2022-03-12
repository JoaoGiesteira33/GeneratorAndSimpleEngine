#include <iostream>
#include <fstream>
#include <string>

#define _USE_MATH_DEFINES
#include <math.h>


typedef struct point{
    float x;
    float y;
    float z;
}*Point;

void write_point(float x, float y, float z, std::ofstream& file){
    file << std::to_string(x) << " "
         << std::to_string(y) << " "
         << std::to_string(z)
         << std::endl;
}

void write_point(Point p, std::ofstream& file) {
    file << std::to_string(p->x) << " "
         << std::to_string(p->y) << " "
         << std::to_string(p->z)
         << std::endl;
}

int gen_sphere(char** args){
    int radius = std::atoi(args[2]);
    int slices = std::atoi(args[3]);
    int stacks = std::atoi(args[4]);

    std::ofstream file;
    file.open(args[5]);
    //cenas
    file.close();
    return 0;
}
int gen_box(char** args){
    int size = std::atoi(args[2]);
    int grid = std::atoi(args[3]);

    std::ofstream file;
    file.open(args[4]);
    //cenas
    file.close();
    return 0;
}


void generate_cone(float radius, float height, int slices , int stacks, std::ofstream& file){
    float alpha = 2*M_PI / slices;
    float yratio = height/(float)stacks;
    int iaux, jaux;

    for (iaux=0; iaux<slices; iaux++){
        float i = (float) iaux;
        float s1 = radius*sin(i*alpha);
        float s2 = radius*sin((i+1)*alpha);
        float c1 = radius*cos(i*alpha);
        float c2 = radius*cos((i+1)*alpha);


        write_point(s2, 0, c2, file);
        write_point(s1, 0, c1, file);
        write_point(0, 0, 0, file);

        for(jaux=0;jaux<stacks;jaux++){
            float j = (float)jaux;

            float newR = -1*((((j+1)*yratio)-height)*radius)/height;
            float news1 = newR*sin(i*alpha);
            float news2 = newR*sin((i+1)*alpha);
            float newc1 = newR*cos(i*alpha);
            float newc2 = newR*cos((i+1)*alpha);

            write_point(s1, yratio*j, c1, file);
            write_point(s2, yratio*j, c2, file);
            write_point(news1, yratio*(j+1), newc1, file);

            if(j+1<stacks){
                write_point(news2, yratio*(j+1), newc2, file);
                write_point(news1, yratio*(j+1), newc1, file);
                write_point(s2, yratio*j, c2, file);
            }
            s1=news1;
            s2=news2;
            c1=newc1;
            c2=newc2;
        }
    }
}
int gen_cone(char** args){
    int radius = std::atoi(args[2]);
    int height = std::atoi(args[3]);
    int slices = std::atoi(args[4]);
    int stacks = std::atoi(args[5]);

    std::ofstream file;
    file.open(args[6]);
    generate_cone(radius, height, slices , stacks, file);
    file.close();
    return 0;
}

int gen_plane(char** args){
    int len = std::atoi(args[2]);
    int divisions = std::atoi(args[3]);

    std::ofstream file;
    file.open(args[4]);
    //cenas
    file.close();
    return 0;
}

int check_args(int n, char **args){
    for(int i = 0; i < n; i++)
        if(!args[i] || !*args[i]) return 0;
    return 1;
}
Point new_point(float x, float y, float z) {
    Point p = (Point) (malloc(sizeof(struct point)));
    p->x = x; p->y = y; p->z = z;
    return p;
}






int main(int argc, char **argv) {
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

    else {
        std::cout<<"Invalid input"<<std::endl;
        return 1;
    }

    return 0;
}