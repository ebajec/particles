# particles
Simulates a bunch of particles.  The shader named "gravity" lets them collide with each other and experience attractive forces.  The "system" one is cool for messing around with dynamical systems.

**To compile:** In the solution file or makefile, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and properly linked in CMakeLists.txt. I have included the headers and some of the binaries for portability purposes.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene. There are a few other commands, and I will probably add more.  To control the camera, use WASD and the mouse.  There is currently an odd bug which causes the orientation of the controls to go crazy.  This is very bad on linux for some reason.  

**Issues:** Frequently, the sphere around the particles will not render.  This is likely related to the fact that current version usues actual sphere meshes around the particles. I will soon fix this by drawing the balls around particles in a separate shader.  The collision could also be improved, but it works well enough.

*close:* closes the scene being rendered.

*exit:* exits the program.
"# particles" 
