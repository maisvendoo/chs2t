set (TARGET chs2t)

project (${TARGET})

set (LIBRARY_OUTPUT_PATH "../../modules/${TARGET}")

set (CMAKE_SHARED_MODULE_PREFIX "")

find_package(Qt5 COMPONENTS Xml REQUIRED)

find_package(CfgReader 1.3.0 REQUIRED)
find_package(physics 1.3.0 REQUIRED)
find_package(Journal 1.3.0 REQUIRED)
find_package(filesystem 1.3.0 REQUIRED)
find_package(device 1.3.0 REQUIRED)
find_package(vehicle 1.3.0 REQUIRED)

file (GLOB INCLUDES "./include/*.h")
file (GLOB EQP_INCLUDES "../chs2t-equipment/include/*.h")
file (GLOB SOURCES "./src/*.cpp")
file (GLOB EQP_SOURCES "../chs2t-equipment/src/*.cpp")

add_library (${TARGET} MODULE ${SOURCES} ${INCLUDES} ${EQP_SOURCES} ${EQP_INCLUDES})

target_include_directories (${TARGET} PRIVATE ./include/)
target_include_directories (${TARGET} PRIVATE ../chs2t-equipment/include)

target_link_libraries (${TARGET} Qt5::Xml)
target_link_libraries (${TARGET} CfgReader)
target_link_libraries (${TARGET} physics)
target_link_libraries (${TARGET} Journal)
target_link_libraries (${TARGET} filesystem)
target_link_libraries (${TARGET} device)
target_link_libraries (${TARGET} vehicle)
