Ryan Barber
CISC 440
HW 5


Game: Simple Qbert
See game-info.txt for how game works/ how to play.

Bottom of file has CMakeLists info

Requirement satisfaction:
	Environment of mostly 3d objects:
		pyramid of cubes is all 3d cubes using draw_cube from hw1
		Qbert and Bad-Qbert (purple one) are 3D mesh made in blender
		All text is 2d (made referencing tutorial 11)

	User control:
		Keyboard controls
			t - up, left
			y - up, right
			g - down, left
			h - down, right
			space - restart
			escape - quit program

	Objects must move smoothly:
		The jump path of the qberts is made using catmull-rom spline from hw4

		The upon reset, next level, and loss of life, the positions are hard reset.

	At least one texture-mapped element:
		Both Qberts are texture mapped with texture painting in blender.
		Each has its own dds image
		
		The text is a texture mapped elment as well. Using the Holstein.dds file
		and referencing tutorial 11 I load the texure for the characters.

	At least one 3D element which is lit with diffuse and/or specular lighting:
		Both Qberts have diffuse lighting like hw2

	Additional Element:
		Both Qberts are complicated 3d objects made in blender
		by manipulating and joining different shapes.
		Technically they are the same object.
		I have included the blend file if you wish to see it.

	BUGS:
		When positions hard reset upon lost life or game reset,
		the first jump the player makes is not smooth. I don't
		know why.



Lines that need to go in CMakeLists.txt

# hw5
add_executable(hw5 
	hw5/main.cpp
	common/shader.cpp
	common/shader.hpp
	common/controls.cpp
	common/controls.hpp
	common/texture.cpp
	common/texture.hpp
	common/objloader.cpp
	common/objloader.hpp
	common/text2D.hpp
	common/text2D.cpp
	common/vboindexer.cpp
	common/vboindexer.hpp

	hw5/MultiColorSimpleTransform.vertexshader
	hw5/MultiColor.fragmentshader
	hw5/TransformVertexShader.vertexshader
	hw5/TextureFragmentShader.fragmentshader
	hw5/TextVertexShader.vertexshader
	hw5/TextVertexShader.fragmentshader
)
#set_target_properties(tutorial03_matrices PROPERTIES RUNTIME_OUTPUT_DIRECTORY /test1)
target_link_libraries(hw5
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(hw5 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/hw5/")
create_target_launcher(hw5 WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/hw5/") # Visual





