lnInclude(${CMAKE_CURRENT_SOURCE_DIR})

set(target_name populationBalance)

wmake_parse_files(${CMAKE_CURRENT_SOURCE_DIR}/Make/files sources_from_wmake)

add_library(${target_name} SHARED
    ${sources_from_wmake}
)


target_include_directories(${target_name} PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/lnInclude"
)

target_include_directories(${target_name} PRIVATE
    #    ${LIB_SRC}/meshTools/lnInclude
    #    ${LIB_SRC}/finiteVolume/lnInclude
    #    ${LIB_SRC}/sampling/lnInclude
    #    ${LIB_SRC}/physicalProperties/lnInclude
    ${LIB_SRC}/thermophysicalModels/basic/lnInclude
    ${LIB_SRC}/thermophysicalModels/specie/lnInclude
    ${LIB_SRC}/thermophysicalModels/multicomponentThermo/lnInclude
    #    ${LIB_SRC}/twoPhaseModels/twoPhaseMixture/lnInclude
    #    ${LIB_SRC}/twoPhaseModels/compressibleTwoPhases/lnInclude
    ${LIB_SRC}/MomentumTransportModels/momentumTransportModels/lnInclude
    ${LIB_SRC}/MomentumTransportModels/compressible/lnInclude
    ${LIB_SRC}/MomentumTransportModels/phaseCompressible/lnInclude
    ${LIB_SRC}/ThermophysicalTransportModels/thermophysicalTransportModel/lnInclude
    ${LIB_SRC}/ThermophysicalTransportModels/fluid/lnInclude
)

target_link_libraries(${target_name} PRIVATE
    OpenFOAM_Defines
    OpenFOAM
    OSspecific
)

target_link_libraries(${target_name} PUBLIC
    phaseSystem
)

target_compile_definitions(${target_name} PRIVATE
    LIB_NAME=lib${target_name}.so
)

set(FOAM_regular_libraries "${FOAM_regular_libraries};${target_name}" PARENT_SCOPE)


