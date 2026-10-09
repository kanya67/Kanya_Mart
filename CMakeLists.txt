cmake_minimum_required(VERSION 3.14)
project(KanyaMart)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Fetch Drogon and its dependencies
include(FetchContent)

FetchContent_Declare(
    Drogon
    GIT_REPOSITORY https://github.com/drogonframework/drogon
    GIT_TAG v1.9.3
)
FetchContent_MakeAvailable(Drogon)

# Add local bcrypt directory
add_subdirectory(third_party/Bcrypt.cpp)

# Source files
set(SOURCES
    src/main.cpp
    src/controllers/AuthController.cpp
    src/controllers/ProductController.cpp
    src/controllers/CartController.cpp
    src/controllers/OrderController.cpp
    src/controllers/SellerController.cpp
    src/controllers/ReviewController.cpp
    src/controllers/HealthController.cpp
    src/services/AuthService.cpp
    src/services/ProductService.cpp
    src/services/CartService.cpp
    src/services/OrderService.cpp
    src/services/SellerService.cpp
    src/services/ReviewService.cpp
)

# Setup executable
add_executable(kanyamart ${SOURCES})

target_include_directories(kanyamart PRIVATE include)

# Link libraries
target_link_libraries(kanyamart PRIVATE drogon bcrypt)

# Copy config.json and web directory to build output
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/config.json DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/web DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
