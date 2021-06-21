
import os
import subprocess
import traceback


GLSLC_EXECUTABLE = 'glslc'

def spv_folder_to_glsl_folder(spv_folder):
    return os.path.join(spv_folder, '../../toy/shader')

def is_file_outdated(src_filename, dst_filename):
    if os.path.isfile(dst_filename):
        src_modified = os.path.getmtime(src_filename)
        dst_modified = os.path.getmtime(dst_filename)
        if src_modified < dst_modified:
            return False
    return True

def glsl_from_spv(spv):
    spv_folder, spv_name = os.path.split(spv)
    glsl_folder = spv_folder_to_glsl_folder(spv_folder)
    glsl_name = spv_name[:-4] + '.glsl'
    glsl = os.path.join(glsl_folder, glsl_name)
    return glsl

def compile_glsl(filename, output_filename):
    if filename.endswith('vert.glsl'):
        stage = 'vertex'
    elif filename.endswith('frag.glsl'):
        stage = 'fragment'
    else:
        raise RuntimeError('unknown shader stage', filename)

    cmd = [
        GLSLC_EXECUTABLE,
        '-fshader-stage={}'.format(stage),
        '-o', output_filename,
        filename
    ]
    print('compile glsl', cmd)
    try:
        subprocess.run(cmd, check=True)
    except subprocess.CalledProcessError:
        print('ERROR compiling glsl', filename)
        traceback.print_exc()
        return False
    else:
        return True

def reload_pipelines(pipelines):
    spv_pipeline_map = {}
    for pipeline in pipelines:
        spvs = pipeline.get_shader_spvs()
        for spv in spvs:
            spv_pipeline_map.setdefault(spv, set()).add(pipeline)
    outdated_pipelines = set()
    for spv in spv_pipeline_map:
        glsl = glsl_from_spv(spv)
        if is_file_outdated(glsl, spv):
            res = compile_glsl(glsl, spv)
            if not res:
                print('ERROR reload failed')
                return
            outdated_pipelines.update(spv_pipeline_map[spv])
    for pipeline in outdated_pipelines:
        pipeline.reload_shader()
