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
         << std::to_string(z) << " ";
}

void write_point(Point p, std::ofstream& file) {
    file << std::to_string(p->x) << " "
         << std::to_string(p->y) << " "
         << std::to_string(p->z) << " ";
}

int gen_sphere(char** args){
    int radius = std::atoi(args[2]);
    int slices = std::atoi(args[3]);
    int stacks = std::atoi(args[4]);

    std::ofstream file;
    file.open(args[5]);
    float ang_stack = M_PI / stacks;
	float ang_slice = (2 * M_PI) / slices;

	for(int i = 0 ; i < slices ; i++){ //Iterate Slices
		float alpha = (i * ang_slice);
		float next_alpha = ((i+1) * ang_slice);

		for(int j = 0 ; j < stacks ; j++){ //Iterate Stacks
			float beta = (j * ang_stack) - (M_PI / 2);
			float next_beta = ((j+1) * ang_stack) - (M_PI / 2);

			//Point 1 Coords
			float x_1 = radius * cosf(beta) * sinf( alpha );
			float y_1 = radius * sinf( beta );
			float z_1 = radius * cosf(beta) * cosf( alpha );
			//Point 2 Coords
			float x_2 = radius * cosf(next_beta) * sinf( next_alpha );
			float y_2 = radius * sinf( next_beta );
			float z_2 = radius * cosf(next_beta) * cosf( next_alpha );
			//Point 3 Coords
			float x_3 = radius * cosf(next_beta) * sinf( alpha );
			float y_3 = radius * sinf( next_beta );
			float z_3 = radius * cosf(next_beta) * cosf( alpha );
			//Point 4 Coords
			float x_4 = radius * cosf(beta) * sinf( next_alpha );
			float y_4 = radius * sinf( beta );
			float z_4 = radius * cosf(beta) * cosf( next_alpha );
	
			write_point(x_1,y_1,z_1,file);
			write_point(x_2,y_2,z_2,file);
			write_point(x_3,y_3,z_3,file);
            file<<std::endl;

			write_point(x_1,y_1,z_1,file);
			write_point(x_4,y_4,z_4,file);
			write_point(x_2,y_2,z_2,file);
            file<<std::endl;
		}
	}

    file.close();
    return 0;
}

void box_front_back(int size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------FRONT & BACK--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            //BACK
            float p1x=j*sub_size;
            float p1y=i*sub_size;                                                    
            float p1z=0;                                          
                                                                                
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

            write_point(p1x,p1y,p1z,file);
            write_point(p2x,p2y,p2z,file);
            write_point(p3x,p3y,p3z,file);
            file<<std::endl;
            write_point(p4x,p4y,p4z,file);
            write_point(p5x,p5y,p5z,file);
            write_point(p6x,p6y,p6z,file);
            file<<std::endl;
            write_point(p7x,p7y,p7z,file);
            write_point(p8x,p8y,p8z,file);
            write_point(p9x,p9y,p9z,file);
            file<<std::endl;
            write_point(p10x,p10y,p10z,file);
            write_point(p11x,p11y,p11z,file);
            write_point(p12x,p12y,p12z,file);
            file<<std::endl;

        }
    }
}

void box_left_right(int size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------LEFT & RIGHT--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            //LEFT
            float p1x=0;
            float p1y=i*sub_size;                                                        
            float p1z=j*sub_size;

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

            write_point(p1x,p1y,p1z,file);
            write_point(p2x,p2y,p2z,file);
            write_point(p3x,p3y,p3z,file);
            file<<std::endl;
            write_point(p4x,p4y,p4z,file);
            write_point(p5x,p5y,p5z,file);
            write_point(p6x,p6y,p6z,file);
            file<<std::endl;
            write_point(p7x,p7y,p7z,file);
            write_point(p8x,p8y,p8z,file);
            write_point(p9x,p9y,p9z,file);
            file<<std::endl;
            write_point(p10x,p10y,p10z,file);
            write_point(p11x,p11y,p11z,file);
            write_point(p12x,p12y,p12z,file);
            file<<std::endl;
        }
    }
}

void box_top_bottom(int size, int grid, float sub_size, std::ofstream& file){
    //std::cout<<"------TOP & BOTTOM--------\n";
    for(int i=0; i<grid ; i++){
        for(int j=0; j<grid; j++){
            
            //BOTTOM
            float p1x=i*sub_size;
            float p1y=0;                                                          
            float p1z=j*sub_size;                                                 
                                                                                
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

            write_point(p1x,p1y,p1z,file);
            write_point(p2x,p2y,p2z,file);
            write_point(p3x,p3y,p3z,file);
            file<<std::endl;
            write_point(p4x,p4y,p4z,file);
            write_point(p5x,p5y,p5z,file);
            write_point(p6x,p6y,p6z,file);
            file<<std::endl;
            write_point(p7x,p7y,p7z,file);
            write_point(p8x,p8y,p8z,file);
            write_point(p9x,p9y,p9z,file);
            file<<std::endl;
            write_point(p10x,p10y,p10z,file);
            write_point(p11x,p11y,p11z,file);
            write_point(p12x,p12y,p12z,file);
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
    
    box_left_right(size,grid,sub_size,file);
    box_top_bottom(size,grid,sub_size,file);
    box_front_back(size,grid,sub_size,file);

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
        file<<std::endl;

        for(jaux=0;jaux<stacks;jaux++){
            float j = (float)jaux;

            float newR = (-1)*((((j+1)*yratio)-height)*radius)/height;
            float news1 = newR*sin(i*alpha);
            float news2 = newR*sin((i+1)*alpha);
            float newc1 = newR*cos(i*alpha);
            float newc2 = newR*cos((i+1)*alpha);

            write_point(s1, yratio*j, c1, file);
            write_point(s2, yratio*j, c2, file);
            write_point(news1, yratio*(j+1), newc1, file);
            file<<std::endl;

            if(j+1<stacks){
                write_point(news2, yratio*(j+1), newc2, file);
                write_point(news1, yratio*(j+1), newc1, file);
                write_point(s2, yratio*j, c2, file);
                file<<std::endl;
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