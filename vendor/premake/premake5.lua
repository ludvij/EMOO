-- Yoinked from https://github.com/TheCherno/Hazel/blob/master/vendor/premake/premake5.lua very helpful

project "Premake"
	kind "Utility"

	targetdir ("%{wks.location}/bin/" .. outputDir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin/intermediates/" .. outputDir .. "/%{prj.name}")

	files{
		"%{wks.location}/**.lua"
	}

	postbuildmessage "Regenerating project files with Premake5!"
	postbuildcommands {
		'"%{prj.location}bin/premake5" %{_ACTION} --file="%{wks.location}premake5.lua"',
		'py "%{prj.location}/custom/add_output_type.py" "%{wks.location}/Graphics/Graphics.vcxproj"'
	}