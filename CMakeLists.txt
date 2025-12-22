# Yêu cầu phiên bản CMake tối thiểu
cmake_minimum_required(VERSION 3.10)

# Project
project(PersonalFinanceManager VERSION 0.1 LANGUAGES CXX)

# Use C++11 (matches original project setting)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

#find raylib 
find_package(raylib REQUIRED)

# Include our headers and raylib include dirs from pkg-config
include_directories(include ${RAYLIB_INCLUDE_DIRS})
link_directories(${RAYLIB_LIBRARY_DIRS})

# Source files (explicitly list existing .cpp files from the workspace)
set(SOURCE_FILES
    GUI/src/App.cpp
    GUI/src/Button.cpp
    GUI/src/DataManager.cpp
    GUI/src/Dialog.cpp
    GUI/src/Dropdown.cpp
    GUI/src/Helper.cpp
    GUI/src/LayoutHelper.cpp
    GUI/src/MasterDataWindow.cpp
    GUI/src/ScrollArea.cpp
    GUI/src/TextInput.cpp
    GUI/src/RecurringWindow.cpp
    GUI/src/TransactionWindow.cpp
    GUI/src/dashboard.cpp
    GUI/mainGUI.cpp
    
    src/date.cpp
    src/ExpenseTransaction.cpp
    src/IncomeTransaction.cpp
    src/mainCLI.cpp
    src/NameTable.cpp
    src/recurring.cpp
    src/statistic.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Link and include raylib using variables provided by pkg-config
target_include_directories(${PROJECT_NAME} PRIVATE
    ${RAYLIB_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/include
    include
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/include
)
target_link_libraries(${PROJECT_NAME} PRIVATE ${RAYLIB_LIBRARIES} raylib)
target_compile_options(${PROJECT_NAME} PRIVATE ${RAYLIB_CFLAGS_OTHER})