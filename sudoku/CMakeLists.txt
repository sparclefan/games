# 项目信息
project (sudoku)

file(GLOB Sources src/*.cpp)
file(GLOB Headers src/*.h)
file(GLOB Forms src/*.ui)

QT5_WRAP_UI(UISrcs ${Forms})
QT5_WRAP_CPP(MocSrcs ${Headers})
include_directories ( ${CMAKE_CURRENT_BINARY_DIR} ./src )

source_group("Header Files" FILES ${Headers}) 
source_group("Generator Files" FILES ${UISrcs} ${MocSrcs}) 

add_executable(${PROJECT_NAME} ${GUI_TYPE} ${Headers} ${Sources} ${UISrcs} ${MocSrcs})
