"make" -f nbproject/Makefile-Debug.mk QMAKE= SUBPROJECTS= .build-conf
make[1]: Entering directory '/home/kurt/NetBeansProjects/23tree-in-cpp'
"make"  -f nbproject/Makefile-Debug.mk dist/Debug/GNU-Linux/23tree-in-cpp
make[2]: Entering directory '/home/kurt/NetBeansProjects/23tree-in-cpp'
mkdir -p build/Debug/GNU-Linux
rm -f "build/Debug/GNU-Linux/debug.o.d"
g++ -std=c++1z   -c -g -MMD -MP -MF "build/Debug/GNU-Linux/debug.o.d" -o build/Debug/GNU-Linux/debug.o debug.cpp
mkdir -p build/Debug/GNU-Linux
rm -f "build/Debug/GNU-Linux/main.o.d"
g++ -std=c++1z   -c -g -MMD -MP -MF "build/Debug/GNU-Linux/main.o.d" -o build/Debug/GNU-Linux/main.o main.cpp
mkdir -p build/Debug/GNU-Linux
rm -f "build/Debug/GNU-Linux/test.o.d"
g++ -std=c++1z   -c -g -MMD -MP -MF "build/Debug/GNU-Linux/test.o.d" -o build/Debug/GNU-Linux/test.o test.cpp
mkdir -p dist/Debug/GNU-Linux
g++ -std=c++1z    -o dist/Debug/GNU-Linux/23tree-in-cpp build/Debug/GNU-Linux/debug.o build/Debug/GNU-Linux/main.o build/Debug/GNU-Linux/test.o 
make[2]: Leaving directory '/home/kurt/NetBeansProjects/23tree-in-cpp'
make[1]: Leaving directory '/home/kurt/NetBeansProjects/23tree-in-cpp'
