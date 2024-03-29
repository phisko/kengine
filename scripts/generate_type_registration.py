#!/usr/bin/env python3

import json
import argparse
import os
import re

from clang.cindex import *

import sys
current_path = os.path.realpath(__file__)
kengine_root = os.path.dirname(os.path.dirname(current_path))
sys.path.append(kengine_root)
from putils.reflection.scripts import clang_helpers

parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, description = 'Generate kengine type registration functions', epilog = '''
Registration files should contain the following format:
{
	"pre_registrations": [
		{
			"registration": "kengine::meta::register_metadata",
			"header": "kengine/helpers/meta/register_metadata.hpp"
		},
	]
	"registrations": [
		{
			"registration": "kengine::meta::register_component_with_script_languages",
			"header": "kengine/helpers/meta/register_with_script_languages.hpp"
		},
		...
	]
}

Registration can be disabled for a specific type by placing a `//! kengine registration off`
comment before it.

The script will generate a separate cpp file for each type,
as well as a 'add_type_registrator.cpp' file and a corresponding header.

All you need to do to have your types registered with the various
kengine systems is to include 'add_type_registrator.hpp' and call 'add_type_registrator()'
before calling `kengine::register_all_types()`.

For more information, see kengine/helpers/register_type_helper.md''')

parser.add_argument('files', help = 'input headers to parse', nargs = '+')
parser.add_argument('--no-write-main-file', help = 'write the main registrator file', action = 'store_true', required = False)
parser.add_argument('--no-write-type-files', help = 'write the individual type registration files', action = 'store_true', required = False)
parser.add_argument('--registrations', help = 'registrations to use, as JSON files', nargs = '+', required = True)
parser.add_argument('--namespace', help = 'namespace in which the functions will be generated', required = True)
parser.add_argument('--output', default = '', help = 'output directory', required = True)
parser.add_argument('--root', help = 'root directory for input files (hierarchy will be reproduced in output directory)', required = False)
parser.add_argument('--export-macro', help = 'DLL export macro', default = '')
parser.add_argument('--export-header', help = 'header defining DLL export macro')
parser.add_argument('--clang-args', help = 'extra arguments to pass to clang', nargs = argparse.REMAINDER, required = False)
parser.add_argument('--diagnostics', help = 'print clang diagnostic messages', action = 'store_true', required = False)

#
# Generation
#

def generate_registration(file_name, types):
	file_name_relative = file_name
	if args.root:
		file_name_relative = os.path.relpath(file_name, args.root)

	out_path = os.path.join(args.output, f'{file_name_relative}_registration.cpp')

	content = generate_registration_headers()
	content += f'#include "{file_name}"'
	content += '''

// entt
#include <entt/entity/fwd.hpp>

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

namespace ''' + args.namespace + ' {'
	content += generate_registration_functions(types)
	content += '}'

	open(out_path, 'w').write(content)

def generate_registration_headers():
	ret = ''
	for registration in pre_registrators:
		ret += f'#include "{registration["header"]}"\n'
	for registration in registrators:
		ret += f'#include "{registration["header"]}"\n'
	return ret

def generate_registration_functions(types):
	ret = ''

	for type in types:
		ret += '''
	void pre_''' + type['function_name'] + '''(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_types", "Pre-registering \'''' + type['type'] + '''\'");
'''

		for registration in pre_registrators:
			ret += '''
		''' + registration['registration'] + '<' + type['type'] + '>(r);'

		ret += '''
	}

	void ''' + type['function_name'] + '''(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;
		kengine_log(r, verbose, "register_types", "Registering \'''' + type['type'] + '''\'");
'''

		for registration in registrators:
			ret += '''
		''' + registration['registration'] + '<' + type['type'] + '>(r);'

		ret += '''
	}
'''
	return ret

#
# Main
#

args = parser.parse_args()

all_types = {}
parsed_files = clang_helpers.parse_files(args.files, args.clang_args)
for file_name, parsed_file in parsed_files.items():
	if not 'nodes' in parsed_file:
		continue

	all_types[file_name] = []

	if args.diagnostics:
		diagnostics = parsed_file['diagnostics']
		if diagnostics:
			print(f'Diagnostics for {file_name}:')
			for diagnostic in diagnostics:
				print(f'\t{diagnostic}')

	def visit_node(node):
		if node.kind == CursorKind.NAMESPACE:
			for child in node.get_children():
				visit_node(child)
			return

		if node.kind not in [CursorKind.STRUCT_DECL, CursorKind.CLASS_DECL]:
			return
		if not node.is_definition():
			return
		if node.brief_comment and re.match(r'.*kengine registration off', node.brief_comment):
			return

		full_name = clang_helpers.get_fully_qualified_symbol(node)
		if full_name == 'putils::reflection::type_info':
			return

		type = {}
		type['type'] = full_name
		clean_name = type['type'].replace('::', '_')
		type['function_name'] = f'register_{clean_name}'
		all_types[file_name].append(type)

	for node in parsed_file['nodes']:
		visit_node(node)

if not args.no_write_type_files:
	pre_registrators = []
	registrators = []
	for registration_file in args.registrations:
		json_file = open(registration_file)
		json_data = json.load(json_file)
		pre_registrators += json_data['pre_registrations']
		registrators += json_data['registrations']

	for file_name, types in all_types.items():
		generate_registration(file_name, types)

if not args.no_write_main_file:
	main_file = os.path.join(args.output, 'add_type_registrator')

	main_file_cpp = '''
#include "add_type_registrator.hpp"

// stl
#include <algorithm>
#include <execution>

// entt
#include <entt/entity/registry.hpp>

// putils
#include "putils/range.hpp"

// kengine core
#include "kengine/core/log/helpers/kengine_log.hpp"
#include "kengine/core/profiling/helpers/kengine_profiling_scope.hpp"

// kengine meta
#include "kengine/meta/functions/register_types.hpp"

namespace ''' + args.namespace + ' {'

	for file_name, types in all_types.items():
		for type in types:
			main_file_cpp += '''
	extern void pre_''' + type['function_name'] + '''(entt::registry &) noexcept;
	extern void ''' + type['function_name'] + '''(entt::registry &) noexcept;
'''

	main_file_cpp += '''
	using registrator_function = void (*)(entt::registry &) noexcept;
	static constexpr registrator_function pre_registrators[] = {'''

	for file_name, types in all_types.items():
		for type in types:
			main_file_cpp += '''
		pre_''' + type['function_name'] + ','

	main_file_cpp += '''
	};

	static constexpr registrator_function registrators[] = {'''

	for file_name, types in all_types.items():
		for type in types:
			main_file_cpp += '''
		''' + type['function_name'] + ','

	main_file_cpp += '''
	};'''

	main_file_cpp += '''
	void add_type_registrator(entt::registry & r) noexcept {
		KENGINE_PROFILING_SCOPE;

		const auto e = r.create();
		r.emplace<kengine::functions::pre_register_types>(e, [](entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "register_types", "Pre-registering ''' + args.namespace + ''' types");

			for (const auto pre_registrator : pre_registrators)
				pre_registrator(r);
		});

		r.emplace<kengine::functions::register_types>(e, [](entt::registry & r) noexcept {
			KENGINE_PROFILING_SCOPE;
			kengine_log(r, log, "register_types", "Registering ''' + args.namespace + ''' types");

			for (const auto registrator : registrators)
				registrator(r);
		});
	}
}'''

	open(main_file + '.cpp', 'w').write(main_file_cpp)
	open(main_file + '.hpp', 'w').write('''
#pragma once

// entt
#include <entt/entity/fwd.hpp>

namespace ''' + args.namespace + ''' {
	''' + args.export_macro + ''' void add_type_registrator(entt::registry & r) noexcept;
}''')