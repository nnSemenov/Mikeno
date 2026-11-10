lnInclude(${CMAKE_CURRENT_SOURCE_DIR})

wmake_parse_files(${CMAKE_CURRENT_SOURCE_DIR}/Make/files sources_from_wmake)

add_library(Lagrangian SHARED
    ${sources_from_wmake}
)


target_include_directories(Lagrangian PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/lnInclude"
)

target_link_libraries(Lagrangian PRIVATE
    OpenFOAM_Defines
    OpenFOAM
    OSspecific

    meshTools
    tracking
    finiteVolume
)

target_compile_definitions(Lagrangian PRIVATE
    LIB_NAME=libLagrangian.so
)

set(FOAM_regular_libraries "${FOAM_regular_libraries};Lagrangian" PARENT_SCOPE)


