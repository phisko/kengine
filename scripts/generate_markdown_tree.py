#!/usr/bin/env python3

import argparse
import os
import pathlib

parser = argparse.ArgumentParser(description='Generate a markdown representation of a directory tree')
parser.add_argument('directory', help='directory to walk')
parser.add_argument('-e', '--extensions', help='file extensions to filter', nargs='*')
args = parser.parse_args()

def search_directory(path):
	dirs = []
	files = []

	for entry in os.listdir(path):
		entry_path = f'{path}/{entry}'

		if os.path.isdir(entry_path):
			dir_results = search_directory(entry_path)
			if dir_results['dirs'] or dir_results['files']:
				dirs.append(dir_results)
		else:
			extension = os.path.splitext(entry_path)[1]
			if (not args.extensions) or (extension in args.extensions):
				files.append(entry_path)

	return { 'path': path, 'dirs': dirs, 'files': files }

root = search_directory(args.directory)

def print_directory(dir, indentation):
	def print_entry(entry):
		def relative_to(path, root):
			return os.path.relpath(path, args.directory).replace('\\', '/')
		path = relative_to(entry, args.directory)
		entry = pathlib.Path(path).stem
		print('\t' * indentation + f'* [{entry}]({path})')

	for f in dir['files']:
		print_entry(f)
	for d in dir['dirs']:
		print_entry(d['path'])
		print_directory(d, indentation + 1)

print_directory(root, 0)