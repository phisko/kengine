import json
import argparse
import os

parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, description = 'Generate kengine type registration functions', epilog = '''
Type files should contain the following format:
[
	{
		"type": "kengine::data::transform",
		"header": "kengine/data/transform.hpp"
	},
	...
]

Registration files should contain the following format:
[
	{
		"registration": "kengine::register_components",
		"header": "kengine/helpers/register_type_helper.hpp"
	},
	...
]

The script will generate a separate cpp file for each type,
as well as a 'add_type_registrator.cpp' file and a corresponding header.

All you need to do to have your types registered with the various
kengine systems is to include 'add_type_registrator.hpp' and call 'add_type_registrator()'
before calling `kengine::register_all_types()`.

For more information, see kengine/helpers/register_type_helper.md''')

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

	for type in json_data:
		if 'enabled' in type and not type['enabled']:
			continue
		type['clean_name'] = type['type'].replace('::', '_')
		type['function_name'] = f'register_{type["clean_name"]}'
		all_types.append(type)

all_registrations = [] # output

# Registrations

def process_registration_file(input_file):
	json_file = open(input_file)
	json_data = json.load(json_file)

	for p in json_data:
		process_registration(p['registration'], p['header'])

def process_registration(registration, header):
	all_registrations.append({ 'registration': registration, 'header': header })

#
# Generation
#

def generate_registration(type):
	out_path = os.path.join(args.output, type['function_name'] + '.cpp')

	if os.path.exists(out_path) and not args.force:
		print('Skipping "' + type['type'] + '": ' + out_path + ' exists')
		return

	print('Generating registration for "' + type['type'] + '" from header "' + type['header'] + '" in "' + out_path + '"')

	content = generate_registration_headers(type)

	if 'condition' in type:
		content += f'\n#ifdef {type["condition"]}\n'

	content += f'#include "{type["header"]}"'

	if 'condition' in type:
		content += '\n#endif'

	content += '''

// entt
#include <entt/entity/fwd.hpp>

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace ''' + args.namespace + ''' {
	void ''' + type['function_name'] + '''(entt::registry & r) noexcept {
''' + generate_registration_function_body(type) + '''
	}
}'''

	open(out_path, 'w').write(content)

def generate_registration_headers(type):
	ret = ''
	for registration in all_registrations:
		ret += f'#include "{registration["header"]}"\n'
	return ret

def generate_registration_function_body(type):
	ret = ''
	if 'condition' in type:
		ret += f'#ifdef {type["condition"]}\n'

	ret += '\t\tKENGINE_PROFILING_SCOPE;\n'
	ret += f'\t\tkengine_log(r, log, "init/register_types", "Registering \'{type["type"]}\'");'

	for registration in all_registrations:
		ret += f'\n\t\t{registration["registration"]}<{type["type"]}>(r);'

	if 'condition' in type:
		ret += '\n#else\n'
		ret += f'\t\tkengine_log(r, log, "init/register_types", "Not registering \'{type["type"]}\' because \'{type["condition"]}\' is not defined");\n'
		ret += '#endif'

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

main_file = os.path.join(args.output, 'add_type_registrator')

main_file_cpp = '''
#include "add_type_registrator.hpp"

// entt
#include <entt/entity/registry.hpp>

// kengine functions
#include "kengine/functions/register_types.hpp"

// kengine helpers
#include "kengine/helpers/log_helper.hpp"
#include "kengine/helpers/profiling_helper.hpp"

namespace ''' + args.namespace + ' {\n'

for type in all_types:
	main_file_cpp += f'\textern void {type["function_name"]}(entt::registry &) noexcept;\n'

main_file_cpp += '''
	void add_type_registrator(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		r.emplace<kengine::functions::register_types>(e, [](entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "Init", "Registering types");
'''

for type in all_types:
	main_file_cpp += f'\n\t\t\t{type["function_name"]}(r);'

main_file_cpp += '''
		});
	}
}'''

open(main_file + '.cpp', 'w').write(main_file_cpp)
open(main_file + '.hpp', 'w').write('''
#pragma once
''' + (f'#include <{args.export_header}>' if args.export_header else '') + '''
// entt
#include <entt/entity/fwd.hpp>

namespace ''' + args.namespace + ''' {
	''' + args.export_macro + ''' void add_type_registrator(entt::registry & r) noexcept;
}''')