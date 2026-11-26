set (TARGET chs2t-iocontroller)

project (${TARGET})

# Метаобъектный компилятор Qt (Q_OBJECT)
set(CMAKE_AUTOMOC ON)

# Путь к собранному модулю
set (LIBRARY_OUTPUT_PATH "../../modules/chs2t")
set (CMAKE_SHARED_MODULE_PREFIX "")

find_package(Qt6 COMPONENTS Xml REQUIRED)

find_package(RRS ${RRS_VERSION} REQUIRED)

file (GLOB INCLUDES "./include/*.h")
file (GLOB SOURCES "./src/*.cpp")

add_library (${TARGET} SHARED ${SOURCES} ${INCLUDES})

target_include_directories (${TARGET} PRIVATE ./include/)

target_link_libraries (${TARGET} Qt6::Xml)
target_link_libraries (${TARGET} io-controller)
target_link_libraries (${TARGET} core)
