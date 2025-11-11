cmake_minimum_required(VERSION 3.10)
project(SqlManager)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(PostgreSQL REQUIRED)

find_library(PQXX_LIB pqxx REQUIRED)
find_path(PQXX_INCLUDE_DIR pqxx/pqxx REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/include
    ${PostgreSQL_INCLUDE_DIRS}
    ${PQXX_INCLUDE_DIR}
)

set(SOURCES
    src/main.cpp
    src/Employee.cpp
    src/DatabaseManager.cpp
    src/Application.cpp
    src/Commands.cpp
    src/CommandFactory.cpp
    src/IDataGenerator.cpp
)

add_executable(SqlManager ${SOURCES})

target_link_libraries(SqlManager 
    ${PostgreSQL_LIBRARIES}
    ${PQXX_LIB}
)

