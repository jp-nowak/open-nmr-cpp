qt_add_executable(open-nmr
    mainwindow.h mainwindow.cpp
    main.cpp
    file_io/ag.cpp file_io/ag.h
    file_io/big_endian.cpp file_io/big_endian.h
    file_io/general.cpp file_io/general.h
    spectrum/spectrum.h spectrum/spectrum.cpp
    file_io/string_operations.cpp file_io/string_operations.h
    3rd_party/kissfft/_kiss_fft_guts.h 3rd_party/kissfft/kiss_fft.c 3rd_party/kissfft/kiss_fft.h 3rd_party/kissfft/kiss_fft_log.h
    processing/fft_1d_transformer.h processing/fft_1d_transformer.cpp
    processing/general.h processing/general.cpp

    spectrum/spectrum_info.h
    gui/spectrumpainter.h gui/spectrumpainter.cpp
    gui/spectrumdisplayer.h gui/spectrumdisplayer.cpp
    gui/xaxis.h gui/xaxis.cpp
    gui/gui_utilities.h gui/gui_utilities.cpp
    gui/spectrum_displayer_actions.h
    spectrum/spectrum_view.h spectrum/spectrum_view.cpp
    gui/tabwidget.h gui/tabwidget.cpp
)

target_link_libraries(open-nmr PRIVATE Qt6::Widgets)

set_target_properties(open-nmr PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
