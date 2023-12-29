# particles
Simulates trajectories of dynamical systems in 3D with some visual effects. 

**To compile:** In CMakeLists, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and built; the paths to the root directories must be specified in CMakeLists.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene.  To control the camera, use WASD and the mouse.

**Other commands**
*close:* closes the scene being rendered. Editing the shaders and entering "launch" again, you can play around with it.

*exit:* exits the program.

