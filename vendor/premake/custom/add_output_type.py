import xml.etree.ElementTree as ET
from sys import argv

with open(argv[1], 'r', encoding='utf-8') as f:
	data = f.read()

tree = ET.parse(argv[1])
ET.register_namespace('', 'http://schemas.microsoft.com/developer/msbuild/2003')
root = tree.getroot()

custombuild = root.findall('.//{http://schemas.microsoft.com/developer/msbuild/2003}CustomBuild')

# <OutputItemType Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">ClCompile</OutputItemType>
# <OutputItemType Condition="'$(Configuration)|$(Platform)'=='Release|x64'">ClCompile</OutputItemType>
for cb in custombuild:
	output = ET.SubElement(cb, 'OutputItemType')
	output.text = 'ClCompile'



tree.write(argv[1])
