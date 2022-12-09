cmake_minimum_required(VERSION 3.17)
project(wet1mivne)

set(CMAKE_CXX_STANDARD 11)

add_executable(wet1mivne

        main23a1.cpp
        AVLTree.h
        Player.h
        Team.h
        wet1util.h
        worldcup23a1.h
        worldcup23a1.cpp)

add_executable(wet1mivne_testyadani

        main.cpp
        AVLTree.h
        Player.h
        Team.h
        wet1util.h
        worldcup23a1.h
        worldcup23a1.cpp)