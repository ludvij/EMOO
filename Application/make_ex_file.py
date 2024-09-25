from os import listdir, makedirs
txf = [''.join(['resources/texts/', x]) for x in listdir('resources/texts')]



def build_cpp_file():
	return """
#ifndef EXPLANATIONS_MAP_HEADER
#define EXPLANATIONS_MAP_HEADER
#include <unordered_map>
{ex}
#endif
"""

def append_to_cpp_file(x, fmt):
	res = ''
	umap = """
inline const std::unordered_map<const char*, const char*> map_{name} = {{
	{fields}
}};
"""
	with open(x, 'r', encoding='utf-8') as f:
		lines = f.readlines()
	maps = {x.split('_')[0] for x in lines if x}
	for m in maps:
		elements = [[x.split(': ')[0].replace(f'{m}_',''), ''.join(x.split(': ')[1:]).strip()] for x in lines if m in x]
		res += umap.format(name=m, fields=',\n\t'.join([f'{{ "{x[0]}", "{x[1]}" }}' for x in elements]))
	return res
fmt = build_cpp_file()

res = fmt.format(ex=''.join([append_to_cpp_file(x, fmt) for x in txf]))

makedirs('src/embedding', exist_ok=True)

with open('src/embedding/explanations.hpp', 'w') as f:
	f.write(res)

# output_cpp_file()