del basic.vert.spv basic.vert.spv.txt basic.frag.spv basic.frag.spv.txt
glslangValidator.exe -V -H -o basic.vert.spv basic.vert.glsl > basic.vert.spv.txt
glslangValidator.exe -V -H -o basic.frag.spv basic.frag.glsl > basic.frag.spv.txt
pause
