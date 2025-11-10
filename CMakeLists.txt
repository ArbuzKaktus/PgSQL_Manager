cmake_minimum_required(VERSION 3.10)
project(SqlMngr)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find PostgreSQL
find_package(PostgreSQL REQUIRED)

# Find libpqxx
find_library(PQXX_LIB pqxx REQUIRED)
find_path(PQXX_INCLUDE_DIR pqxx/pqxx REQUIRED)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIR}
)

# Source files
set(SOURCES
    src/main.cpp
    src/Employee.cpp
    src/DatabaseManager.cpp
    src/Application.cpp
    src/Commands.cpp
    src/CommandFactory.cpp
    src/IDataGenerator.cpp
)

# Create executable
add_executable(myApp ${SOURCES})

# Link libraries
target_link_libraries(myApp 
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIB}
)

# Compiler flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(myApp PRIVATE -Wall -Wextra)
endif()
