
from sys import argv, byteorder
from ctypes import sizeof, c_uint32




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

	fmt = f"""
#include<array>
static std::array<uint32_t, {size}> {argv[2]} = {{
{fmt_data}
}};
"""
	print(fmt)


if __name__ == '__main__':
	main()