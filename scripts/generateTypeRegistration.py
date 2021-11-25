import json
import argparse
import os

parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, description = 'Generate kengine type registration functions', epilog = '''
Input files should contain the following format:
{
	"components": [
		{
			"type": "kengine::TransformComponent",
			"header": "data/TransformComponent.hpp"
		},
		...
	],
	"types": [
		{
			"type": "putils::Point3f",
			"header": "Point.hpp"
		},
		...
	]
}

The script will generate a separate cpp file for each type,
as well as a 'registerTypes.cpp' file and a corresponding header.

All you need to do to have your types registered with the various
kengine systems is to include 'registerTypes.hpp' and call 'registerTypes()'
after 'kengine::init()'.

For more information, see kengine/helpers/registerTypeHelper.md
''')

parser.add_argument('input_files', help = 'JSON input files', nargs = '+')
parser.add_argument('--output', default = '', help = 'output directory')
parser.add_argument('--force', action = 'store_true', help = 'overwrite existing files (only use if a component has changed headers)')

# Impl

all_functions = []

def process_file(input_file):
	json_file = open(input_file)
	json_data = json.load(json_file)

	def generate_files(json_array_name, kengine_register_helper):
		if not json_array_name in json_data:
			return
		for p in json_data[json_array_name]:
			process_type(p['type'], p['header'], json_array_name, kengine_register_helper)

	generate_files('components', 'kengine::registerComponents')
	generate_files('types', 'kengine::registerComponents')

def process_type(type, header, json_array_name, kengine_register_helper):
	clean_name = type.replace(':', '') 
	function_name = 'register' + clean_name
	all_functions.append(function_name)

	out_path = os.path.join(args.output, function_name + '.cpp')

	if os.path.exists(out_path) and not args.force:
		print('Skipping "' + type + '": ' + out_path + ' exists')
		return

	print('Generating ' + json_array_name + ' registration for "' + type + '" from header "' + header + '" in "' + out_path + '"')

	open(out_path, 'w').write('''
#include "helpers/registerTypeHelper.hpp"
#include "''' + header + '''"

void ''' + function_name + '''() noexcept {
	''' + kengine_register_helper + '''<''' + type + '''>();
}''')

# Main

args = parser.parse_args()

for f in args.input_files:
	process_file(f)

main_file = os.path.join(args.output, 'registerTypes')

main_file_cpp = '''
void registerTypes() noexcept {
'''

for func in all_functions:
	main_file_cpp += '''
	extern void ''' + func + '''() noexcept;
	''' + func + '''();
'''

main_file_cpp += '''
}
'''

open(main_file + '.cpp', 'w').write(main_file_cpp)
open(main_file + '.hpp', 'w').write('''
#pragma once

void registerTypes() noexcept;
''')