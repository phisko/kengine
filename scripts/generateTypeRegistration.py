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

parser.add_argument('input_file', help = 'JSON input file')
parser.add_argument('--output', default = '', help = 'output directory')

args = parser.parse_args()

json_file = open(args.input_file)
json_data = json.load(json_file)

all_functions = []

def generate_files(json_array_name, kengine_register_helper):
	if not json_data[json_array_name]:
		return
	for p in json_data[json_array_name]:
		clean_name = p['type'].replace(':', '') 
		function_name = 'register' + clean_name
		all_functions.append(function_name)

		out_path = os.path.join(args.output, clean_name + '.cpp')

		print('Generating ' + json_array_name + ' registration for "' + p['type'] + '" from header "' + p['header'] + '" in "' + out_path + '"')

		open(out_path, 'w').write('''
#include "helpers/registerTypeHelper.hpp"
#include "''' + p['header'] + '''"

void ''' + function_name + '''() noexcept {
	''' + kengine_register_helper + '''<''' + p['type'] + '''>();
}
			''')

generate_files('components', 'kengine::registerComponents')
generate_files('types', 'kengine::registerComponents')

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