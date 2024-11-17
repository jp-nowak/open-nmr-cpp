cmake_minimum_required(VERSION 3.16)

project(nmr_program VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

add_compile_definitions(kiss_fft_scalar=double)



set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(nmr_program
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}


        file_io/ag.cpp
        file_io/ag.h
        file_io/big_endian.cpp
        file_io/big_endian.h
        file_io/general.cpp
        file_io/general.h
        spectrum/spectrum.h spectrum/spectrum.cpp
        file_io/string_operations.cpp
        file_io/string_operations.h
        3rd_party/kissfft-master/_kiss_fft_guts.h 3rd_party/kissfft-master/kiss_fft.cpp 3rd_party/kissfft-master/kiss_fft.h 3rd_party/kissfft-master/kiss_fft_log.h
        processing/fft_1d_transformer.h processing/fft_1d_transformer.cpp
        processing/general.h
        processing/general.cpp



    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET nmr_program APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(nmr_program SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(nmr_program
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(nmr_program PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.nmr_program)
endif()
set_target_properties(nmr_program PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS nmr_program
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(nmr_program)
endif()
