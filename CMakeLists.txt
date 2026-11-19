cmake_minimum_required(VERSION 3.16)
project(CFunc3dViewer)


if(NOT EMSCRIPTEN) 
	set(CMAKE_C_STANDARD 99)
	set(CMAKE_C_STANDARD_REQUIRED ON)
	set(CMAKE_C_EXTENSIONS OFF)
endif()

file(GLOB_RECURSE all_SRCS
		"${PROJECT_SOURCE_DIR}/src/*.cpp"
		"${PROJECT_SOURCE_DIR}/src/*.c"
		)

add_executable(
	${PROJECT_NAME}
	${all_SRCS}
)


if(EMSCRIPTEN)
	message("Checking if vendored/SDL exists...")

	if(NOT EXISTS "./vendored/SDL")
		message("vendored/SDL does not existing, cloning from SDL repository.")
		execute_process(COMMAND sh -c "git clone https://github.com/libsdl-org/SDL.git vendored/SDL")
	else()
		message("vendored/SDL exits, using it...")
	endif()

	add_subdirectory(vendored/SDL EXCLUDE_FROM_ALL)
	target_link_libraries(${PROJECT_NAME} PRIVATE SDL3::SDL3)

	set(CMAKE_EXECUTABLE_SUFFIX ".html" CACHE INTERNAL "")
else()
	include(FindPkgConfig)
	pkg_search_module(SDL3 REQUIRED sdl3)

	target_include_directories(${PROJECT_NAME} PRIVATE
		${SDL3_INCLUDE_DIRS}
	)
	
	target_link_libraries(${PROJECT_NAME} PRIVATE
		${SDL3_LIBRARIES}
	)
endif()

target_include_directories(${PROJECT_NAME} PRIVATE
	"${PROJECT_SOURCE_DIR}/data/"
	"${PROJECT_SOURCE_DIR}/include/"
	"${PROJECT_SOURCE_DIR}/external/stb/"
	"${PROJECT_SOURCE_DIR}/external/MParse/"
	"${PROJECT_SOURCE_DIR}/external/TinyCG/"
	"${PROJECT_SOURCE_DIR}/external/cglm/include/"

)

target_link_libraries(${PROJECT_NAME} PRIVATE
	m
)

target_compile_options(${PROJECT_NAME} PRIVATE 
	-Wall
	-Wextra
	-Wpedantic
)
