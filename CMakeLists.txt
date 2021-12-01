# CMake 最低版本号要求
cmake_minimum_required (VERSION 3.2)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#--------- QT ----------------
set(CMAKE_PREFIX_PATH $ENV{QTDIR})
find_package(Qt5 REQUIRED Core Gui Widgets)

link_libraries(Qt5::Core Qt5::Gui Qt5::Widgets)
#----------- QT end  --------------------

#SET(GUI_TYPE $<$<CONFIG:Release>:WIN32> $<$<CONFIG:Debug>:>)
SET(GUI_TYPE WIN32)

ADD_SUBDIRECTORY(llk)
ADD_SUBDIRECTORY(spider)
ADD_SUBDIRECTORY(terris)
ADD_SUBDIRECTORY(2048)
ADD_SUBDIRECTORY(sudoku)


