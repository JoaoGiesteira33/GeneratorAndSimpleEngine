#include <iostream>

#include "tinyxml2.h"

using namespace tinyxml2;
using namespace std;

int main(int argc, char **argv){
    const char* file_name = argv[1];
    cout << "FileName: " << file_name << endl;

    XMLDocument doc;
    XMLPrinter printer;
    
    int x = doc.LoadFile(file_name);
    cout << "X: " << x << endl;
    doc.Print();

    return 0;
}