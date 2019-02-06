# cmake_minimum_required(VERSION 3.1.0)

# project(q_example)
# add_compile_options(-std=c++11)


# # Find includes in corresponding build directories
# set(CMAKE_INCLUDE_CURRENT_DIR ON)
# # Instruct CMake to run moc automatically when needed
# set(CMAKE_AUTOMOC ON)
# # Create code from a list of Qt designer ui files
# set(CMAKE_AUTOUIC ON)

# # Find the QtWidgets library
# find_package(Qt5Widgets CONFIG REQUIRED)
# #find_package(Qt5 COMPONENTS Widgets REQUIRED)

# # Populate a CMake variable with the sources
# set(q_example_HDRS
#     include
#     include/main_window.hpp
# )

# set(q_example_SRCS
#     src/main_window.ui
#     src/main.cpp
#     #src/main_window.hpp
#     #include/main_window.hpp
#     src/main_window.cpp
# )
# #QT5_WRAP_CPP(q_example q_example_HDRS)

# include_directories(${q_example_HDRS})



# qt5_wrap_cpp(moc_SRC include/main_window.hpp)

# #include_directories(${SDL2_INCLUDE_DIRS})


# add_executable(${CMAKE_PROJECT_NAME} ${q_example_SRCS} ${moc_SRC})


# # Use the Widgets module from Qt 5
# # target_link_libraries(${CMAKE_PROJECT_NAME} Qt5::Widgets)
# target_include_directories(${CMAKE_PROJECT_NAME} PUBLIC ${q_example_HDRS} ${example_HEADERS_MOC})
cmake_minimum_required(VERSION 3.1.0)
set(UI_NAME Gui)
project(${UI_NAME})
add_compile_options(-std=c++11 -g)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)

# Populate a CMake variable with the sources
set(${UI_NAME}_HDRS
    include
)
set(FFMPEG_INCLUDE_DIRS
	# "/usr/local/lib"
	# "/usr/local/include"
	# "/usr/include"
	# "/usr/include/include"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libavcodec"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libavdevice"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libavfilter"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libavformat"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libavutil"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libpostproc"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libswresample"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/include/libswscale"
)

set(FFMPEG_LIBS
	# /usr/local/lib/libavformat.a
	# /usr/local/lib/libavcodec.a
    # /usr/local/lib/libavutil.a
    # /usr/local/lib/libswscale.a
    # /usr/local/lib/libswresample.a
	# /usr/local/lib/libavfilter.a

	# /usr/lib/x86_64-linux-gnu/libavformat.a
	# /usr/lib/x86_64-linux-gnu/libavcodec.a
    # /usr/lib/x86_64-linux-gnu/libavutil.a
    # /usr/lib/x86_64-linux-gnu/libswscale.a
    # /usr/lib/x86_64-linux-gnu/libswresample.a
	# /usr/lib/x86_64-linux-gnu/libavfilter.a
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavformat.a"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavcodec.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libavutil.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libswscale.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libswresample.a"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavfilter.a"
)
set(SDL_INCLUDE
	"/usr/include/SDL2"
	"/usr/include/SDL"	
)
include_directories(
	src
	src/player
	src/playlist_backup
	${FFMPEG_INCLUDE_DIRS}
	${SDL_INCLUDE}
	${SDL_INCLUDE_DIRS}
	${SDL_IMAGE_INCLUDE_DIRS}
	${SDL_INCLUDE}
)
set(sui
	src/main_window.ui
	src/meta_editor/meta_editor.ui

)

set(${UI_NAME}_SRCS
	${sui}	
	src/main.cpp
	src/main_window.cpp
	src/player/player.cpp
	src/playlist_backup/playlist_backup.cpp
	src/meta_editor/meta_editor.cpp
)

#qt5_wrap_cpp(moc_SRC src/metadata/meta_editor.hpp)

MESSAGE(${Qt5Widgets_INCLUDE_DIRS})
# Tell CMake to create the helloworld executable
add_executable(${UI_NAME} ${${UI_NAME}_SRCS}
	#${moc_SRC}
)
# Use the Widgets module from Qt 5
target_link_libraries(${UI_NAME}
    ${FFMPEG_LIBS}
	${SDL_LIBRARIES}
	${SDL_IMAGE_LIBRARY} 
	Qt5::Widgets
	-lm
	-lz
    -llzma
    -lbz2
	-lSDL2
    -lpthread
)
