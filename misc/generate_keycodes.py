
"""
Generate keycodes.py from WinUser.h

Find WinUser.h file in the Windows SDK
Run this script as
> py -3 generate_keycodes.py path_to_WinUser.h
The output file is OUTPUT_FILEPATH
https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
"""

OUTPUT_FILEPATH = '../script/keycodes.py'
OUTPUT_FILEPATH_LUA = '../luascript/keycodes.lua'


def parse_line(line):
	if line.startswith('#define VK_'):
		parts = line.split()
		name = parts[1]
		value = parts[2]
		return name, value
	return None

def write_keycodes(keycodes):
	desc = 'key codes generated from WinUser.h using misc/generate_keycodes.py'
	with open(OUTPUT_FILEPATH, 'w', newline='\n') as f:
		f.write('# {}\n'.format(desc))
		f.write('\n')
		for keycode in keycodes:
			name, value = keycode
			line = '{} = {}\n'.format(name, value)
			f.write(line)

	with open(OUTPUT_FILEPATH_LUA, 'w', newline='\n') as f:
		f.write('-- {}\n'.format(desc))
		f.write('\n')
		f.write('return {\n')
		for keycode in keycodes:
			name, value = keycode
			line = '\t{} = {},\n'.format(name, value)
			f.write(line)
		f.write('}\n')

def generate_keycodes(vk_lines):
	keycodes = []
	for line in vk_lines:
		res = parse_line(line)
		if not res:
			continue
		name, value = res
		keycodes.append((name, value))
		if value == '0x2F':
			# add VK_0 - VK_9 0x30 - 0x39
			for i in range(0x30, 0x39 + 1):
				char = chr(i)
				name = 'VK_{}'.format(char)
				value = '0x{:02X}'.format(i)
				keycodes.append((name, value))
			# add VK_A - VK_Z 0x41 - 0x5A
			for i in range(0x41, 0x5A + 1):
				char = chr(i)
				name = 'VK_{}'.format(char)
				value = '0x{:02X}'.format(i)
				keycodes.append((name, value))

	for keycode in keycodes:
		print(keycode)

	write_keycodes(keycodes)

def main():
	import sys
	if len(sys.argv) != 2:
		print('ERROR, need WinUser.h file path as argument')
		return
	winuser_file = sys.argv[1]
	tag = 'NOVIRTUALKEYCODES'
	vk_lines = []
	started = False
	with open(winuser_file, 'r') as f:
		for line in f:
			if tag in line:
				if started:
					# stop
					break
				else:
					# start
					started = True
			if started:
				vk_lines.append(line)

	generate_keycodes(vk_lines)
	
if __name__ == '__main__':
	main()
