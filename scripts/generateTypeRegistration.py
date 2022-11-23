import json
import argparse
import os

parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, description = 'Generate kengine type registration functions', epilog = '''
Type files should contain the following format:
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

Registration files should contain the following format:
{
	"components": [
		{
			"registration": "kengine::registerComponents",
			"header": "helpers/registerTypeHelper.hpp"
		},
		...
	],
	"types": [
		{
			"registration": "kengine::registerTypes",
			"header": "helpers/registerTypeHelper.hpp"
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

parser.add_argument('--types', help = 'input types, as JSON files', nargs = '+', required = True)
parser.add_argument('--registrations', help = 'registrations to use, as JSON files', nargs = '+', required = True)
parser.add_argument('--namespace', help = 'namespace in which the functions will be generated', required = True)
parser.add_argument('--output', default = '', help = 'output directory', required = True)
parser.add_argument('--force', action = 'store_true', help = 'overwrite existing files (only use if a component has changed headers)')
parser.add_argument('--export-macro', help = 'DLL export macro', default = '')
parser.add_argument('--export-header', help = 'header defining DLL export macro')

#
# Parsing
#

# Types

all_types = [] # output

def process_type_file(input_file):
	json_file = open(input_file)
	json_data = json.load(json_file)

	def process_types(json_array_name, is_component):
		if not json_array_name in json_data:
			return
		for type in json_data[json_array_name]:
			process_type(type, is_component)

	process_types('components', True)
	process_types('types', False)

def process_type(type, is_component):
	clean_name = type['type'].replace(':', '')
	function_name = 'register' + clean_name
	all_types.append({ 'json_type': type, 'is_component': is_component, 'function_name': function_name, 'clean_name': clean_name })

all_registrations = [] # output

# Registrations

def process_registration_file(input_file):
	json_file = open(input_file)
	json_data = json.load(json_file)

	def process_registrations(json_array_name, is_component):
		if not json_array_name in json_data:
			return
		for p in json_data[json_array_name]:
			process_registration(p['registration'], p['header'], is_component)

	process_registrations('components', True)
	process_registrations('types', False)

def process_registration(registration, header, is_component):
	all_registrations.append({ 'registration': registration, 'header': header, 'is_component': is_component })

#
# Generation
#

def generate_registration(type):
	out_path = os.path.join(args.output, type['function_name'] + '.cpp')

	if os.path.exists(out_path) and not args.force:
		print('Skipping "' + type['json_type']['type'] + '": ' + out_path + ' exists')
		return

	print('Generating registration for "' + type['json_type']['type'] + '" from header "' + type['json_type']['header'] + '" in "' + out_path + '"')

	open(out_path, 'w').write(generate_registration_headers(type) +
'''#include "''' + type['json_type']['header'] + '''"

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace ''' + args.namespace + '''{
	void ''' + type['function_name'] + '''(entt::registry & r) noexcept {''' + generate_conditional_registration(type) + '''	}
}''')

def generate_registration_headers(type):
	ret = ''
	for registration in all_registrations:
		if type['is_component'] != registration['is_component']:
			continue
		ret += '#include "' + registration['header'] + '"\n'
	return ret

def generate_conditional_registration(type):
	ret = ''
	if 'condition' in type['json_type']:
		ret += '''
#ifdef ''' + type['json_type']['condition']
	ret += '''
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init/registerTypes", "Registering \'''' + type['json_type']['type'] + '''\'");
		''' + generate_registration_implementation(type)
	if 'condition' in type['json_type']:
		ret += '''#else
		kengine_log(r, Log, "Init/registerTypes", "Not registering \'''' + type['json_type']['type'] + '''\' because \'''' + type['json_type']['condition'] + '''\' is not defined");
#endif
'''
	return ret

def generate_registration_implementation(type):
	ret = ''
	for registration in all_registrations:
		if type['is_component'] != registration['is_component']:
			continue
		ret += registration['registration'] + '<' + type['json_type']['type'] + '>(r);\n'
	return ret

#
# Main
#

args = parser.parse_args()

for f in args.types:
	process_type_file(f)

for f in args.registrations:
	process_registration_file(f)

for type in all_types:
	generate_registration(type)

main_file = os.path.join(args.output, 'registerTypes')

main_file_cpp = '''
#include "registerTypes.hpp"

// kengine helpers
#include "helpers/logHelper.hpp"
#include "helpers/profilingHelper.hpp"

namespace ''' + args.namespace + '''{
	void registerTypes(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, Log, "Init", "Registering types");
'''

for type in all_types:
	main_file_cpp += '''
		extern void ''' + type['function_name'] + '''(entt::registry &) noexcept;
		''' + type['function_name'] + '''(r);
'''

main_file_cpp += '''
	}
}
'''

open(main_file + '.cpp', 'w').write(main_file_cpp)
open(main_file + '.hpp', 'w').write('''
#pragma once

''' + (f'#include <{args.export_header}>' if args.export_header else '') + '''

// entt
#include <entt/entity/fwd.hpp>

namespace ''' + args.namespace + '''{
	''' + args.export_macro + ''' void registerTypes(entt::registry & r) noexcept;
}
''')