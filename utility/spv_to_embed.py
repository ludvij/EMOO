
from sys import argv, byteorder
from ctypes import sizeof, c_uint32
from os.path import basename



def main():
	if len(argv) < 2:
		print(f'usage: {argv[0]} path/to/spv')
		return
	data = []
	with open(argv[1], 'rb') as f:
		while chunk := f.read(sizeof(c_uint32)):
			data.append(int.from_bytes(chunk, byteorder, signed=False))

	data = ['0x' + format(x, '08X') for x in data]

	size = len(data)
	fmt_data = '\t'
	for i, v in enumerate(data):
		if i % 11 == 0 and i > 0:
			fmt_data += '\n\t'
		fmt_data += v
		if i < len(data) - 1:
			fmt_data += ', '
	name = '_'.join(basename(argv[1]).split('.')[:-1])
	fmt = f"""
#include <cstdint>
extern const size_t sg_embed_{name}_size = {size * 4};
extern const uint32_t sg_embed_{name}[] = {{
{fmt_data}
}};
"""
	print(fmt)


if __name__ == '__main__':
	main()