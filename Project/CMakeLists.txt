cmake_minimum_required(VERSION 3.14)
project(VRMSBackend LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(SQLite3 REQUIRED)

add_executable(vrms_backend
    backend.cpp
    http_server.cpp
    database.cpp
    models.cpp
    repositories.cpp
    booking_service.cpp
    payment_worker.cpp
)
target_link_libraries(vrms_backend PRIVATE SQLite::SQLite3 ws2_32)
