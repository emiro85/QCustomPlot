PROJECT(qcustomplot CXX)
CMAKE_MINIMUM_REQUIRED(VERSION 3.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

SET(Q_MAJOR_VERSION "2")
SET(Q_MINOR_VERSION "1")
SET(Q_PATCH_VERSION "0")

find_package(Qt6 COMPONENTS Widgets PrintSupport REQUIRED)
find_package(OpenGL REQUIRED)

include_directories(src)
add_definitions(-DQCUSTOMPLOT_COMPILE_LIBRARY -DQCUSTOMPLOT_USE_OPENGL)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(Q_VERSION "${Q_MAJOR_VERSION}.${Q_MINOR_VERSION}.${Q_PATCH_VERSION}")
set(Q_SOVERSION "${Q_MAJOR_VERSION}.${Q_MINOR_VERSION}")

if(NOT MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast -Wlogical-op -Wduplicated-branches -Wduplicated-cond")
endif()

set(PUBLIC_HEADERS
    src/global.h
    src/painter.h
    src/paintbuffer.h
    src/layer.h
    src/axis/range.h
    src/axis/axis.h
    src/axis/axisticker.h
    src/plottable.h
    src/item.h
    src/lineending.h
    src/core.h
    src/layout.h
    src/plottables/plottable-graph.h
    src/plottables/plottable-curve.h
    src/plottables/plottable-bars.h
    src/plottables/plottable-statisticalbox.h
    src/plottables/plottable-colormap.h
    src/plottables/plottable-financial.h
    src/plottables/plottable-errorbar.h
    src/items/item-straightline.h
    src/items/item-line.h
    src/items/item-curve.h
    src/items/item-rect.h
    src/items/item-text.h
    src/items/item-ellipse.h
    src/items/item-pixmap.h
    src/items/item-tracer.h
    src/items/item-bracket.h
    src/layoutelements/layoutelement-axisrect.h
    src/layoutelements/layoutelement-legend.h
    src/layoutelements/layoutelement-textelement.h
    src/layoutelements/layoutelement-colorscale.h
    src/colorgradient.h
    src/vector2d.h
    src/axis/axistickerdatetime.h
    src/axis/axistickertime.h
    src/axis/axistickerfixed.h
    src/axis/axistickertext.h
    src/axis/axistickerpi.h
    src/axis/axistickerlog.h
    src/datacontainer.h
    src/selection.h
    src/selectionrect.h
    src/plottable1d.h
    src/scatterstyle.h
    src/selectiondecorator-bracket.h
)

set(SOURCES
    src/painter.cpp
    src/paintbuffer.cpp
    src/layer.cpp
    src/axis/range.cpp
    src/axis/axis.cpp
    src/axis/axisticker.cpp
    src/plottable.cpp
    src/item.cpp
    src/lineending.cpp
    src/core.cpp
    src/layout.cpp
    src/global.cpp
    src/plottables/plottable-graph.cpp
    src/plottables/plottable-curve.cpp
    src/plottables/plottable-bars.cpp
    src/plottables/plottable-statisticalbox.cpp
    src/plottables/plottable-colormap.cpp
    src/plottables/plottable-financial.cpp
    src/plottables/plottable-errorbar.cpp
    src/items/item-straightline.cpp
    src/items/item-line.cpp
    src/items/item-curve.cpp
    src/items/item-rect.cpp
    src/items/item-text.cpp
    src/items/item-ellipse.cpp
    src/items/item-pixmap.cpp
    src/items/item-tracer.cpp
    src/items/item-bracket.cpp
    src/layoutelements/layoutelement-axisrect.cpp
    src/layoutelements/layoutelement-legend.cpp
    src/layoutelements/layoutelement-textelement.cpp
    src/layoutelements/layoutelement-colorscale.cpp
    src/colorgradient.cpp
    src/vector2d.cpp
    src/axis/axistickerdatetime.cpp
    src/axis/axistickertime.cpp
    src/axis/axistickerfixed.cpp
    src/axis/axistickertext.cpp
    src/axis/axistickerpi.cpp
    src/axis/axistickerlog.cpp
    src/datacontainer.cpp
    src/selection.cpp
    src/selectionrect.cpp
    src/plottable1d.cpp
    src/scatterstyle.cpp
    src/selectiondecorator-bracket.cpp
)

add_library(QCustomPlot STATIC
    ${SOURCES})

set_target_properties(QCustomPlot
    PROPERTIES
    VERSION ${Q_VERSION}
    SOVERSION ${Q_SOVERSION}
    PUBLIC_HEADER "${PUBLIC_HEADERS}")

target_link_libraries(QCustomPlot
    Qt6::Widgets
    Qt6::PrintSupport)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    QCustomPlotConfigVersion.cmake
    VERSION ${Q_VERSION}
    COMPATIBILITY AnyNewerVersion
    )

install(TARGETS QCustomPlot
        EXPORT QCustomPlotTarget
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(EXPORT QCustomPlotTarget
        FILE QCustomPlotConfig.cmake
        NAMESPACE QCustomPlot::
        DESTINATION lib/cmake/QCustomPlot
        )

#INSTALL(TARGETS qcustomplot DESTINATION "${CMAKE_INSTALL_LIBDIR}")
#INSTALL(FILES qcustomplot.h DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
#INSTALL(FILES cmake/QCustomPlotConfig.cmake DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/QCustomPlot)
