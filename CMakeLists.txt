project (audex)

set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/README")
set(CPACK_GENERATOR "DEB;RPM;TGZ")
set(DEBIAN_PACKAGE_DESCRIPTION "Audex")
set(DEBIAN_PACKAGE_SECTION "Multimedia")
set(CPACK_SOURCE_GENERATOR "TBZ2")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "71")
set(CPACK_PACKAGE_CONTACT "Marco Nelles <audex@maniatek.de>")
set(AUDEX_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}")
set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${AUDEX_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${AUDEX_VERSION}")
include(CPack)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)
configure_file(config.h.cmake ${CMAKE_BINARY_DIR}/config.h)

find_package(KDE4 REQUIRED)
find_package(KdeMultimedia REQUIRED)
include(MacroLibrary)

find_package(Cdparanoia)
macro_log_feature(CDPARANOIA_FOUND "cdparanoia" "Reads digital audio directly from a CD, then writes the data to a file or pipe in several different formats" "http://www.xiph.org/paranoia/" FALSE "" "Required for building Audex.")

include_directories(
    ${KDE4_INCLUDES}
    ${KDEMULTIMEDIA_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${CDPARANOIA_INCLUDE_DIR}
)

set(audex_SRCS
  main.cpp
  mainwindow.cpp
  models/cddamodel.cpp
  models/profilemodel.cpp
  utils/diskfreespace.cpp
  utils/pid.cpp
  utils/coverfetcher.cpp
  utils/patternparser.cpp
  utils/playlist.cpp
  utils/wavefilewriter.cpp
  utils/cddaparanoia.cpp
  utils/encoderwrapper.cpp
  utils/cddaextractthread.cpp
  utils/upload.cpp
  utils/encoderassistant.cpp
  utils/hashlist.cpp
  utils/3rdparty/crc32.cpp
  utils/parameters.cpp
  widgets/cddaheaderwidget.cpp
  widgets/generalsettingswidget.cpp
  widgets/profilewidget.cpp
  widgets/remoteserversettingswidget.cpp
  widgets/lamewidget.cpp
  widgets/oggencwidget.cpp
  widgets/flacwidget.cpp
  widgets/faacwidget.cpp
  widgets/wavewidget.cpp
  widgets/customwidget.cpp
  dialogs/coverbrowserdialog.cpp
  dialogs/cddaheaderdatadialog.cpp
  dialogs/profiledatadialog.cpp
  dialogs/profiledatacoverdialog.cpp
  dialogs/profiledataplaylistdialog.cpp
  dialogs/profiledatainfodialog.cpp
  dialogs/profiledatahashlistdialog.cpp
  dialogs/patternwizarddialog.cpp
  dialogs/commandwizarddialog.cpp
  dialogs/simplepatternwizarddialog.cpp
  dialogs/extractingprogressdialog.cpp
  dialogs/protocoldialog.cpp
  core/audex.cpp
)

kde4_add_ui_files(audex_SRCS
  widgets/generalsettingswidgetUI.ui
  widgets/profilewidgetUI.ui
  widgets/remoteserversettingswidgetUI.ui
  widgets/lamewidgetUI.ui
  widgets/oggencwidgetUI.ui
  widgets/flacwidgetUI.ui
  widgets/faacwidgetUI.ui
  widgets/wavewidgetUI.ui
  widgets/customwidgetUI.ui
  dialogs/coverbrowserwidgetUI.ui
  dialogs/cddaheaderdatawidgetUI.ui
  dialogs/profiledatawidgetUI.ui
  dialogs/profiledatacoverwidgetUI.ui
  dialogs/profiledataplaylistwidgetUI.ui
  dialogs/profiledatainfowidgetUI.ui
  dialogs/profiledatahashlistwidgetUI.ui
  dialogs/patternwizardwidgetUI.ui
  dialogs/commandwizardwidgetUI.ui
  dialogs/simplepatternwizardwidgetUI.ui
  dialogs/extractingprogresswidgetUI.ui
  dialogs/protocolwidgetUI.ui
)

kde4_add_kcfg_files(audex_SRCS preferences.kcfgc)

kde4_add_executable(audex ${audex_SRCS})
target_link_libraries(audex ${CDPARANOIA_LIBRARIES} ${KDE4_KDEUI_LIBS} ${KDE4_KUTILS_LIBS} ${KDEMULTIMEDIA_LIBRARIES})

install(TARGETS audex DESTINATION ${BIN_INSTALL_DIR})
install(FILES audexui.rc DESTINATION  ${DATA_INSTALL_DIR}/audex)
install(FILES audex.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})
install(FILES audex-rip-audiocd.desktop DESTINATION ${DATA_INSTALL_DIR}/solid/actions)

add_subdirectory(icons)
add_subdirectory(images)
add_subdirectory(po)

macro_display_feature_log()
