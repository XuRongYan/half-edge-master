cmake_minimum_required(VERSION 3.13)
project(HalfEdge)

set(CMAKE_CXX_STANDARD 11)

add_executable(HalfEdge main.cpp tramesh_types.h trimesh_t.cpp trimesh_t.h vtk.h)