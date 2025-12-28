cmake_minimum_required(VERSION 3.10)
project(PersonalFinanceManager VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Raylib paths for Linux build
set(RAYLIB_INCLUDE_DIR /tmp/raylib/src)
set(RAYLIB_LIB_DIR /tmp/raylib/src)

# Source files
set(SOURCE_FILES
    GUI/src/App.cpp
    GUI/src/button.cpp
    GUI/src/DataManager.cpp
    GUI/src/Dialog.cpp
    GUI/src/Dropdown.cpp
    GUI/src/Helper.cpp
    GUI/src/LayoutHelper.cpp
    GUI/src/MasterDataWindow.cpp
    GUI/src/ScrollArea.cpp
    GUI/src/TextInput.cpp
    GUI/src/RecurringWindow.cpp
    GUI/src/StatisticWindow.cpp
    GUI/src/TransactionWindow.cpp
    GUI/src/dashboard.cpp
    GUI/mainGUI.cpp
    
    src/date.cpp
    src/ExpenseTransaction.cpp
    src/IncomeTransaction.cpp
    src/NameTable.cpp
    src/recurring.cpp
    src/statistic.cpp
)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/GUI/include
    ${RAYLIB_INCLUDE_DIR}
)

# Link raylib and dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${RAYLIB_LIB_DIR}/libraylib.a
    glfw
    GL
    m
    pthread
    dl
)
