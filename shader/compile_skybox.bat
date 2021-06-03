del skybox.vert.spv skybox.frag.spv
glslc -fshader-stage=vertex -o skybox.vert.spv skybox.vert.glsl
glslc -fshader-stage=fragment -o skybox.frag.spv skybox.frag.glsl
pause