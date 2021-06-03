del frag.spv frag.spv.txt vert.spv vert.spv.txt
glslangValidator.exe -V -H shader.vert.glsl > vert.spv.txt
glslangValidator.exe -V -H shader.frag.glsl > frag.spv.txt
pause
