cmake_minimum_required(VERSION 2.8)
add_definitions(-std=c++11)
project( Ball_N_Stick )

include_directories(include)

add_executable(ballnstick src/ballnstick.cpp)
set_target_properties(ballnstick PROPERTIES COMPILE_FLAGS "-DBT_USE_DOUBLE_PRECISION -I/usr/local/include/bullet")
target_link_libraries(ballnstick glut GL GLU BulletSoftBody BulletDynamics BulletCollision LinearMath)

add_executable(game src/play_game.cpp include/game.h src/game.cpp src/CSocketServer.cpp include/CSocketServer.h)
set_target_properties(game PROPERTIES COMPILE_FLAGS "-DBT_USE_DOUBLE_PRECISION -I/usr/local/include/bullet -L/usr/X11/lib")
target_link_libraries(game glut GL GLU BulletSoftBody BulletDynamics BulletCollision LinearMath X11 pthread)

add_executable(game_ga src/play_game_ga.cpp include/game.h src/game.cpp src/CSocketServer.cpp include/CSocketServer.h)
set_target_properties(game_ga PROPERTIES COMPILE_FLAGS "-DBT_USE_DOUBLE_PRECISION -I/usr/local/include/bullet -L/usr/X11/lib")
target_link_libraries(game_ga glut GL GLU BulletSoftBody BulletDynamics BulletCollision LinearMath X11 pthread)
