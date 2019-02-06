# cmake_minimum_required(VERSION 2.8)
# project(gtest1)

# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")

# find_package(GTest REQUIRED)
# find_package(Threads REQUIRED)

# include_directories(${GTEST_INCLUDE_DIRS})

# add_executable(MyTests test.cpp)

# target_link_libraries(MyTests ${GTEST_BOTH_LIBRARIES})
# target_link_libraries(MyTests ${CMAKE_TREAD_LIB_INIT})
# add_test(gt MyTests)
# enable_testing()

cmake_minimum_required (VERSION 3.1.0)
project(mediatest)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -std=c++11 -Wall")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)
# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Find the QtWidgets library
find_package(Qt5Widgets CONFIG REQUIRED)
find_package(GTest REQUIRED)
find_package(Threads REQUIRED)

set(meta_editor_dir
    "${CMAKE_SOURCE_DIR}/../src/src/meta_editor"
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
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavformat.a"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavcodec.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libavutil.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libswscale.a"
    "${CMAKE_SOURCE_DIR}/../ffmpeg/libswresample.a"
	"${CMAKE_SOURCE_DIR}/../ffmpeg/libavfilter.a"
)
message( ${meta_editor_dir} )
include_directories(${GTEST_INCLUDE_DIRS} ${meta_editor_dir})
add_executable(metatest test.cpp)
target_link_libraries(metatest ${GTEST_BOTH_LIBRARIES} Qt5::Widgets)
target_link_libraries(metatest ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(metatest
    ${FFMPEG_LIBS}
	-lm
	-lz
    -llzma
    -lbz2
)
add_test(Test metatest)
enable_testing()