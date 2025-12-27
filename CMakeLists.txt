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
    GUI/src/StatisticWindow.cpp
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

# Windows: copy raylib.dll beside the built EXE and into prebuild
if(WIN32)
    # Expected vcpkg-installed raylib DLL path relative to this project
    set(RAYLIB_DLL_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/installed/x64-windows/bin/raylib.dll")

    if(EXISTS "${RAYLIB_DLL_PATH}")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${RAYLIB_DLL_PATH}"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/raylib.dll"
        )

        # Also stage Windows deliverables into prebuild folder
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_SOURCE_DIR}/prebuild"
            COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:${PROJECT_NAME}>" "${CMAKE_CURRENT_SOURCE_DIR}/prebuild/PersonalFinanceManager-Windows.exe"
            COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE_DIR:${PROJECT_NAME}>/raylib.dll" "${CMAKE_CURRENT_SOURCE_DIR}/prebuild/raylib.dll"
        )
    else()
        message(WARNING "raylib.dll not found at: ${RAYLIB_DLL_PATH}. Ensure the DLL is next to the EXE for distribution.")
    endif()
endif()