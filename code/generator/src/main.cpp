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
    std::string filename = args[5];

    return 0;
}
int gen_box(char** args){
    int size = std::atoi(args[2]);
    int grid = std::atoi(args[3]);
    std::string filename = args[4];

    return 0;
}
int gen_cone(char** args){
    int radius = std::atoi(args[2]);
    int height = std::atoi(args[3]);
    int slices = std::atoi(args[4]);
    int stacks = std::atoi(args[5]);
    std::string filename = args[6];
    return 0;
}
int gen_plane(char** args){
    int len = std::atoi(args[2]);
    int divisions = std::atoi(args[3]);
    std::string filename = args[4];
    return 0;
}

int check_args(int n, char **args){
    for(int i = 0; i < n; i++)
        if(!args[i] || !*args[i]) return 0;
    return 1;
}

void write_in_file(std::string file){
    std::ofstream writer(file, std::ios::out);

    //insercoes aqui

    writer.close();
}

std::string point2String(Point pt){
    std::string str = std::to_string(pt->x) + " " +
                      std::to_string(pt->y) + " " +
                      std::to_string(pt->z);
    return str;
}

void write_triangle(Point pt1, Point pt2, Point pt3, std::ofstream &writer){
    std::string str = point2String(pt1) + " " + 
                      point2String(pt2) + " " + 
                      point2String(pt3) + "\n";

    writer.write(str.c_str(), str.size() + 1);
}

void write_point(float x, float y, float z, std::ofstream& file) {
    file<<

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