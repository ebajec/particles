# particles
Simulates particle dynamics in three dimensions.  Currently, there is a version which simulates gravitational interactions with collision, and one which simulates dynamical systems.  The collision works alright for small numbers of particles, but once >2 particle collisions become common it gets a little weird.

**To compile:** In CMakeLists, make sure to include a path to "lib" folder from [this repository](https://github.com/ebajec/linear-algebra) as an include directory— this uses the matrix class I made there.  Also, make sure that GLEW and GLFW are installed and properly linked in CMakeLists.txt. The headers and some of the binaries are included for portability purposes.

**How to use:** Once the program has opened, type "launch" in the console window to launch the scene. There are a few other commands, and I will probably add more.  To control the camera, use WASD and the mouse.  There is currently an odd bug which causes the orientation of the controls to invert sometimes.  This is very bad on linux for some reason.  

**Issues:** The spheres around particles will sometimes not render when enabled.  It is likely an issue with the meshes, so I will eventually change it to render them from a dedicated shader. 

*close:* closes the scene being rendered.

*exit:* exits the program.
"# particles" 
