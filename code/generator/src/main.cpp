#include <iostream>

int gen_sphere(int radius, int slices, int stacks, std::string filename){
    return 0;
}
int gen_box(int size, int grid, std::string filename){
    return 0;
}
int gen_cone(int radius, int height, int slices, int stacks, std::string filename){
    return 0;
}
int gen_plane(int len, int divisions, std::string filename){
    return 0;
}

int check_args(int n, char **args){
    for(int i = 0; i < n; i++)
        if(!args[i] || !*args[i]) return 0;
    return 1;
}


int main(int argc, char **argv) {
    if(!argv[1] || !*argv[1]){
        std::cout<<"Invalid input"<<std::endl;
        return 1;
    }

    std::string figure = argv[1];

    if(figure == "sphere" && check_args(4, argv+2))
        gen_sphere(std::atoi(argv[2]),std::atoi(argv[3]),std::atoi(argv[4]),argv[5]);

    else if (figure == "plane" && check_args(3, argv+2))
        gen_plane(std::atoi(argv[2]),std::atoi(argv[3]), argv[4]);

    else if (figure == "cone" && check_args(5, argv+2))
        gen_cone(std::atoi(argv[2]),std::atoi(argv[3]),std::atoi(argv[4]),std::atoi(argv[5]), argv[6]);

    else if (figure == "box" && check_args(3, argv+2))
        gen_box(std::atoi(argv[2]),std::atoi(argv[3]), argv[4]);

    else {
        std::cout<<"Invalid input"<<std::endl;
        return 1;
    }

    return 0;
}