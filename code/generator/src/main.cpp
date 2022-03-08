#include <iostream>
#include <fstream>
#include <string>


typedef struct point{
    float x;
    float y;
    float z;
}*Point;

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
int gen_cone(char** args){
    int radius = std::atoi(args[2]);
    int height = std::atoi(args[3]);
    int slices = std::atoi(args[4]);
    int stacks = std::atoi(args[5]);

    std::ofstream file;
    file.open(args[6]);
    //cenas
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


void write_point(Point p, std::ofstream& file) {
    file << std::to_string(p->x) << " "
         << std::to_string(p->y) << " "
         << std::to_string(p->z) << ";";
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