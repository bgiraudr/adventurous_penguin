# Adventurous penguin

An OpenGL project where we need to make a small video about the "north pole" and including a boat

To compile everyting :

```bash
mkdir sfmlGraphicsPipeline/build && cd $_
cmake ..
make -j6

cd ../../project
mkdir build && cd $_
cmake ..
make {scene}
./{scene}
```

Or you can use the makefile at the root of the project.

```bash
make init
make TARGET={SCENE_TO_COMPILE}
```

Need to rebuild the sfmlGraphicsPipeline every time a texture, object or shader is added.
Need to rebuild the project scenes every once the cpp associated is changed.
