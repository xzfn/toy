del vertex_color.vert.spv vertex_color.vert.spv.txt vertex_color.frag.spv vertex_color.frag.spv.txt
glslangValidator.exe -V -H -o vertex_color.vert.spv vertex_color.vert.glsl > vertex_color.vert.spv.txt
glslangValidator.exe -V -H -o vertex_color.frag.spv vertex_color.frag.glsl > vertex_color.frag.spv.txt
pause
