## intsall for desktop files for mate/kde
install(FILES surguch-kde.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/kio/servicemenus/ RENAME surguch.desktop)
install(FILES surguch-mate.desktop DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/file-manager/actions/ RENAME surguch.desktop)

## install extension for gnome
find_package(Gettext REQUIRED)
set(GETTEXT_DOMAIN surguch-gnome-extension)

file(GLOB PO_FILES po/*.po)

set(LANGUAGES "")
foreach(PO_FILE ${PO_FILES})
    get_filename_component(LANG ${PO_FILE} NAME_WE)
    list(APPEND LANGUAGES ${LANG})
endforeach()

GETTEXT_PROCESS_PO_FILES(
    ${GETTEXT_DOMAIN}
    ALL
    PO_FILES ${PO_FILES}
)

foreach(LANG ${LANGUAGES})
    install(
        FILES ${CMAKE_CURRENT_BINARY_DIR}/${LANG}.gmo
        DESTINATION share/locale/${LANG}/LC_MESSAGES
        RENAME ${GETTEXT_DOMAIN}.mo
    )
endforeach()

install(
    FILES surguch-gnome-extension.py
    DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}/nautilus-python/extensions
)
