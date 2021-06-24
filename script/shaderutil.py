
import os
import shadercompiler


def spv_folder_to_glsl_folder(spv_folder):
    return os.path.join(spv_folder, '../../toy/shader')

def glsl_from_spv(spv):
    spv_folder, spv_name = os.path.split(spv)
    glsl_folder = spv_folder_to_glsl_folder(spv_folder)
    glsl_name = spv_name[:-4] + '.glsl'
    glsl = os.path.join(glsl_folder, glsl_name)
    return glsl

def reload_pipelines(pipelines):
    spv_pipeline_map = {}
    for pipeline in pipelines:
        spvs = pipeline.get_shader_spvs()
        for spv in spvs:
            spv_pipeline_map.setdefault(spv, set()).add(pipeline)
    outdated_pipelines = set()
    for spv in spv_pipeline_map:
        glsl = glsl_from_spv(spv)
        if shadercompiler.is_shader_outdated(glsl, spv):
            res = shadercompiler.compile_glsl(glsl, spv)
            if not res:
                print('ERROR reload failed')
                return
            outdated_pipelines.update(spv_pipeline_map[spv])
    for pipeline in outdated_pipelines:
        pipeline.reload_shader()
