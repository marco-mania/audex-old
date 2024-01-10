project (audex)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules)

find_package(KDE4 REQUIRED)
include(MacroLibrary)

find_package(Cdparanoia)
macro_log_feature(CDPARANOIA_FOUND "cdparanoia" "Reads digital audio directly from a CD, then writes the data to a file or pipe in several different formats" "http://www.xiph.org/paranoia/" FALSE "" "Required for building Audex.")

include_directories(
	${KDE4_INCLUDES}
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
  utils/maskparser.cpp
  utils/playlist.cpp
  utils/wavefilewriter.cpp
  utils/cddaparanoia.cpp
  utils/encoderwrapper.cpp
  utils/cddaextractthread.cpp
  utils/ftpupload.cpp
  utils/encoderassistant.cpp
  utils/hashlist.cpp
  utils/3rdparty/crc32.cpp
  widgets/cddaheaderwidget.cpp
  widgets/generalsettingswidget.cpp
  widgets/profilewidget.cpp
  widgets/ftpsettingswidget.cpp
  widgets/assistant/greetingswidget.cpp
  widgets/assistant/basepathwidget.cpp
  widgets/assistant/codecselectionwidget.cpp
  widgets/assistant/filenamewidget.cpp
  widgets/assistant/filenameschemewidget.cpp
  widgets/assistant/additionswidget.cpp
  widgets/assistant/goodbyewidget.cpp
  dialogs/coverbrowserdialog.cpp
  dialogs/cddaheaderdatadialog.cpp
  dialogs/profiledatadialog.cpp
  dialogs/profiledatacoverdialog.cpp
  dialogs/profiledataplaylistdialog.cpp
  dialogs/profiledatainfodialog.cpp
  dialogs/profiledatahashlistdialog.cpp
  dialogs/maskwizarddialog.cpp
  dialogs/commandwizarddialog.cpp
  dialogs/simplemaskwizarddialog.cpp
  dialogs/extractingprogressdialog.cpp
  dialogs/protocoldialog.cpp
  dialogs/assistantdialog.cpp
  core/audex.cpp
)

kde4_add_ui_files(audex_SRCS
  widgets/generalsettingswidgetUI.ui
  widgets/profilewidgetUI.ui
  widgets/ftpsettingswidgetUI.ui
  widgets/assistant/greetingswidgetUI.ui
  widgets/assistant/basepathwidgetUI.ui
  widgets/assistant/codecselectionwidgetUI.ui
  widgets/assistant/filenamewidgetUI.ui
  widgets/assistant/filenameschemewidgetUI.ui
  widgets/assistant/additionswidgetUI.ui
  widgets/assistant/goodbyewidgetUI.ui
  dialogs/coverbrowserwidgetUI.ui
  dialogs/cddaheaderdatawidgetUI.ui
  dialogs/profiledatawidgetUI.ui
  dialogs/profiledatacoverwidgetUI.ui
  dialogs/profiledataplaylistwidgetUI.ui
  dialogs/profiledatainfowidgetUI.ui
  dialogs/profiledatahashlistwidgetUI.ui
  dialogs/maskwizardwidgetUI.ui
  dialogs/commandwizardwidgetUI.ui
  dialogs/simplemaskwizardwidgetUI.ui
  dialogs/extractingprogresswidgetUI.ui
  dialogs/protocolwidgetUI.ui
)

kde4_add_kcfg_files(audex_SRCS preferences.kcfgc)

kde4_add_executable(audex ${audex_SRCS})
target_link_libraries(audex ${CDPARANOIA_LIBRARIES} ${KDE4_KDEUI_LIBS} ${KDE4_KUTILS_LIBS} kcddb kcompactdisc)

install(TARGETS audex DESTINATION ${BIN_INSTALL_DIR})
install(FILES audexui.rc DESTINATION  ${DATA_INSTALL_DIR}/audex)
install(FILES audex.desktop DESTINATION ${XDG_APPS_INSTALL_DIR})

add_subdirectory(icons)
add_subdirectory(images)

macro_display_feature_log()