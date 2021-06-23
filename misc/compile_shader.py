
import subprocess
import os
import glob
import traceback


GLSLC_EXECUTABLE = 'glslc'
INPUT_FOLDER = '../shader'
OUTPUT_FOLDER = '../../toy_generated/shader'

# TODO should test outdated based on #include dependencies.
FORCE_RECOMPILE = True

def glob_endswith(folder, end):
	return glob.glob(os.path.join(folder, '*' + end))

def is_file_outdated(src_filename, dst_filename):
	if os.path.isfile(dst_filename):
		src_modified = os.path.getmtime(src_filename)
		dst_modified = os.path.getmtime(dst_filename)
		if src_modified < dst_modified:
			return False
	return True

def compile_glsl(filename, output_filename):
	if not is_file_outdated(filename, output_filename) and not FORCE_RECOMPILE:
		print('skip', filename)
		return

	if filename.endswith('vert.glsl'):
		stage = 'vertex'
	elif filename.endswith('frag.glsl'):
		stage = 'fragment'
	else:
		print('unknown shader stage', filename)
		assert False

	cmd = [
		GLSLC_EXECUTABLE,
		'-fshader-stage={}'.format(stage),
		'-o', output_filename,
		filename
	]
	print('run', cmd)
	try:
		subprocess.run(cmd, check=True)
	except subprocess.CalledProcessError:
		traceback.print_exc()

def compile_folder(folder, output_folder):
	os.makedirs(output_folder, exist_ok=True)
	vertex_files = glob_endswith(folder, '.vert.glsl')
	fragment_files = glob_endswith(folder, '.frag.glsl')
	shader_files = vertex_files + fragment_files
	output_spvnames = []
	for shader_file in shader_files:
		basename = shader_file[len(folder) + 1:]
		spvname = basename[:-5] + '.spv'
		output_filename = os.path.join(output_folder, spvname)
		compile_glsl(shader_file, output_filename)
		output_spvnames.append(spvname)
	return output_spvnames

def remove_lost_shader_spv(folder, keeps):
	spvfiles = glob_endswith(folder, '.spv')
	spvnames = set([os.path.basename(spvfile) for spvfile in spvfiles])
	keepnames = set(keeps)
	to_remove_names = spvnames - keepnames
	for to_remove_name in to_remove_names:
		path = os.path.join(folder, to_remove_name)
		print('remove lost shader spv', path)
		os.remove(path)

if __name__ == '__main__':
	output_spvnames = compile_folder(INPUT_FOLDER, OUTPUT_FOLDER)
	remove_lost_shader_spv(OUTPUT_FOLDER, output_spvnames)
