add_executable(cru_graphics_experiment_1 1.cpp)
target_link_libraries(cru_graphics_experiment_1 PRIVATE cru_demo_base)

add_executable(cru_graphics_experiment_2 2.cpp)
target_link_libraries(cru_graphics_experiment_2 PRIVATE cru_demo_base)

find_package(dlib CONFIG REQUIRED)

add_executable(cru_graphics_experiment_4 4.cpp)
target_link_libraries(cru_graphics_experiment_4 PRIVATE cru_demo_base dlib::dlib)
