import argparse
from pathlib import PurePosixPath
c_string = "const char {0}[] = {1} {2} {3}\n"
c_comment_string = "// File {}\n"

def string_to_c_array(var_name, string_data):
	hex_data = [ hex(ord(i)) for i in string_data]
	result_variable = c_string.format(var_name, "{", ",".join(hex_data), "};")
	return result_variable

def formate_c_variable(c_variable, file_name):
	result_string = c_comment_string.format(file_name)
	result_string += c_variable
	result_string += "\n"
	return result_string


def read_file(file_name):
	with open(file_name, 'r') as reader:
		return reader.read()

if __name__ == '__main__':
	files_in = argparse.ArgumentParser(description='files for embed, example: --f "core.cl kernal.cl" --o "../gen_headers"')
	files_in.add_argument("--f")
	args = files_in.parse_args()
	print(args.f)
	files_to_embed = args.f.split(" ")

	source_strings = []

	for file in files_to_embed:
		file_name = PurePosixPath(file).stem + "_source"

		file_data = read_file(file)
		c_variable = string_to_c_array(file_name, file_data)
		source_strings.append(formate_c_variable(c_variable,file))

	with open("source.h", 'w') as writer:
		writer.write("\n".join(source_strings))