# acad_network
This is a fork of Kar Epker's network code, originally forked so it could be updated to fit with the libraries on flux.
To start with, this meant altering the CMakeLists at the top level and in src/. At top level, we had to change the paths
to the boost libaries. At the src level, compiling the serialize and build binaries return linking errors that we solved 
by adding -lstdc++ and -lm switches in a few places.
To do:
- add comments in both CMakeList.txt to indicate where we changed stuff and why.
