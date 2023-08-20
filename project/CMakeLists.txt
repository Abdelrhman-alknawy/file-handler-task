cmake_minimum_required(VERSION 3.10)
project(project)

# Find Boost components
#find_package(Boost REQUIRED)

# Add subdirectories for fh and requester
add_subdirectory(file_handler)
add_subdirectory(request)
add_subdirectory(logger)
