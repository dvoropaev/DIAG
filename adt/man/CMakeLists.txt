configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/en/adt.man.in
    ${CMAKE_CURRENT_BINARY_DIR}/en/adt.1
    @ONLY
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/ru/adt.man.in
    ${CMAKE_CURRENT_BINARY_DIR}/ru/adt.1
    @ONLY
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/en/adt.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/man1)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/ru/adt.1 DESTINATION ${CMAKE_INSTALL_MANDIR}/ru/man1)
