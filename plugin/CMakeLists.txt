include(ECMQmlModule)
find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Qml Multimedia)
find_package(KNightTime ${PROJECT_DEP_VERSION} CONFIG REQUIRED)
find_package(MpvQt REQUIRED)
find_package(KF6 ${KF6_MIN_VERSION} REQUIRED COMPONENTS KCMUtils)

set(svw_SRCS
    daynight.cpp daynight.h
    mpvitem.cpp mpvitem.h
)

add_library(plasma_wallpaper_svw_static STATIC ${svw_SRCS})
set_property(TARGET plasma_wallpaper_svw_static PROPERTY POSITION_INDEPENDENT_CODE ON)

target_link_libraries(plasma_wallpaper_svw_static
    Qt6::Qml
    Qt6::Multimedia
    KF6::KCMUtils
    KNightTime
    MpvQt::MpvQt
)

ecm_add_qml_module(plasma_wallpaper_svw URI "com.github.luisbocanegra.svwr" VERSION 1.0 CLASS_NAME DayNight)

qt_generate_foreign_qml_types(plasma_wallpaper_svw_static plasma_wallpaper_svw)

target_sources(plasma_wallpaper_svw PRIVATE smartvideowallpaperplugin.cpp)

target_link_libraries(plasma_wallpaper_svw PRIVATE
    plasma_wallpaper_svw_static
)

ecm_finalize_qml_module(plasma_wallpaper_svw)
