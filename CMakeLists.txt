cmake_minimum_required(VERSION 3.17)
project(project)

set(CMAKE_CXX_STANDARD 14)

target_include_directories(clauseForm PRIVATE ${./project/})

set(SOURCES project/clause_form.cpp ${YOUR_DIRECTORY}/file1.h ${YOUR_DIRECTORY}/file2.h)
add_executable(clauseForm ${SOURCES})