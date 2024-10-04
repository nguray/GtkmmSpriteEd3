cmake_minimum_required(VERSION 3.5)
project(GtkmmSpriteEd VERSION 0.1 LANGUAGES CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

find_package(PkgConfig)
pkg_check_modules(GTKMM gtkmm-3.0)

include_directories(${GTKMM_INCLUDE_DIRS})
link_directories(${GTKMM_LIBRARY_DIRS})

set(PROJECT_SOURCES
    main.cpp 
    myApplication.cpp myApplication.h
    editArea.cpp editArea.h
    editMode.cpp editMode.h 
    editModeEllipse.cpp editModeEllipse.h 
    editModeFill.cpp editModeFill.h 
    editModePencil.cpp editModePencil.h 
    editModeRect.cpp editModeRect.h 
    editModeSelect.cpp editModeSelect.h 
    palette.cpp palette.h 
    resources.cpp 
    RHandle.cpp RHandle.h 
    RPoint.cpp RPoint.h 
    RRect.cpp RRect.h 
    spriteArea.cpp spriteArea.h 

)

add_executable(${CMAKE_PROJECT_NAME} ${PROJECT_SOURCES})
target_compile_features(${CMAKE_PROJECT_NAME} PUBLIC cxx_std_20)
target_link_libraries(${CMAKE_PROJECT_NAME} ${GTKMM_LIBRARIES})
