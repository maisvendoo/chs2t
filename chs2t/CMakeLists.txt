set (TARGET chs2t)

project (${TARGET})

set (LIBRARY_OUTPUT_PATH "../../modules/${TARGET}")

set (CMAKE_SHARED_MODULE_PREFIX "")

find_package(Qt5 COMPONENTS Xml REQUIRED)

find_package(CfgReader REQUIRED)
find_package(physics REQUIRED)
find_package(Journal REQUIRED)
find_package(filesystem REQUIRED)
find_package(device REQUIRED)
find_package(vehicle REQUIRED)

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
