
import re
import os
import subprocess
import traceback


GLSLC_EXECUTABLE = 'glslc'

INCLUDE_RE = re.compile(r'\s*#include ["<](.+)[">]')


def get_file_includes_direct(filename):
	includes = []
	with open(filename, 'r') as f:
		for line in f:
			if 'include' in line:
				res = INCLUDE_RE.match(line)
				if res:
					includes.append(res.group(1))
	return includes

def _gather_file_includes_recursive(filename, visited):
	if filename in visited:
		return
	visited.add(filename)
	folder = os.path.dirname(filename)
	includes = get_file_includes_direct(filename)
	include_filenames = [os.path.join(folder, include) for include in includes]
	for include_filename in include_filenames:
		_gather_file_includes_recursive(include_filename, visited)

def get_file_includes_recursive(filename):
	visited = set()
	_gather_file_includes_recursive(filename, visited)
	visited.remove(filename)
	return visited

def get_shader_mtime(src_filename):
	"""get mtime based on #include dependencies"""
	includes = get_file_includes_recursive(src_filename)
	latest_mtime = os.path.getmtime(src_filename)
	for include_filename in includes:
		mtime = os.path.getmtime(include_filename)
		if mtime > latest_mtime:
			latest_mtime = mtime
	return latest_mtime

def is_shader_outdated(src_filename, dst_filename):
	if not os.path.isfile(dst_filename):
		return True
	dst_modified = os.path.getmtime(dst_filename)
	src_modified = get_shader_mtime(src_filename)
	return src_modified > dst_modified

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
