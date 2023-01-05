import argparse
import json

parser = argparse.ArgumentParser(formatter_class = argparse.RawTextHelpFormatter, description = 'Check for missing patches from branches file')
parser.add_argument('files', nargs='+')

args = parser.parse_args()

for file in args.files:
	print(f"Processing {file}")
	with open(file) as f:
		types_json = json.load(f)
	with open(file, 'w') as f:
		types_json['components'].sort(key=lambda x: x['header'])
		types_json['types'].sort(key=lambda x: x['header'])
		str = json.dumps(types_json, indent='\t')
		f.write(str)
