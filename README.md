# particles
I made a renderer for meshes and now I'm using it for particles.    

**To compile:** In the solution file or makefile, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and properly linked in CMakeLists.txt. I have included the headers and some of the binaries for portability purposes.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene. There are a few other commands, and I will probably add more.  To control the camera, use WASD and the mouse.  There is currently an odd bug which causes the orientation of the controls to go crazy.  This is very bad on linux for some reason.  

*close:* closes the scene being rendered.

*exit:* exits the program.
"# particles" 
