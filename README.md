# particles
Simulates trajectories of dynamical systems in 3D with some visual effects.  Mainly uses Runge-Kutta methods.

**To compile:** In CMakeLists, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and properly linked in CMakeLists.txt. The headers and some of the binaries are included for portability purposes.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene.  To control the camera, use WASD and the mouse.  There is currently an odd bug which causes the orientation of the controls to invert sometimes.  This is very bad on linux for some reason.  

**Other commands**
*close:* closes the scene being rendered. Editing the shaders and entering "launch" again, you can play around with it.

*exit:* exits the program.
"# particles" 
