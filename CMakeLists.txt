cmake_minimum_required(VERSION 3.10)
project(PJ1 VERSION 0.1.0 LANGUAGES C CXX)

# Manually specify OpenCV path (Make sure this is correct)
# Find OpenCV
find_package(OpenCV REQUIRED)

# Add the executable
add_executable(PJ1 main.cpp)

# Include OpenCV headers (Automatically set by OpenCV package)
include_directories(${OpenCV_INCLUDE_DIRS})

# Link OpenCV libraries
target_link_libraries(PJ1 ${OpenCV_LIBS})

# Enable testing (Optional)
include(CTest)
enable_testing()
