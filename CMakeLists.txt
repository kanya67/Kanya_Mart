cmake_minimum_required(VERSION 3.14)
project(KanyaMart)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

# Fetch cpp-httplib
FetchContent_Declare(
    httplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.15.3
)
FetchContent_MakeAvailable(httplib)

# Fetch nlohmann_json
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

# Fetch a bcrypt implementation
FetchContent_Declare(
    bcrypt
    GIT_REPOSITORY https://github.com/hilch/Bcrypt.cpp.git
    GIT_TAG main
)
FetchContent_MakeAvailable(bcrypt)

# Setup our executable
add_executable(kanyamart 
    src/main.cpp 
    src/controllers/AuthController.cpp 
    src/services/AuthService.cpp
)

target_include_directories(kanyamart PRIVATE include)

# Link libraries
find_package(Threads REQUIRED)
target_link_libraries(kanyamart PRIVATE httplib json::json bcrypt Threads::Threads)
