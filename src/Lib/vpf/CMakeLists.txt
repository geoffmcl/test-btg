

add_library(vpf STATIC 
    component.cxx
    component.hxx
    contour.cxx
    contour.hxx
    coverage.cxx
    coverage.hxx
    database.cxx
    database.hxx
    feature.cxx
    feature.hxx
    label.cxx
    label.hxx
    library.cxx
    library.hxx
    line.cxx
    line.hxx
    polygon.cxx
    polygon.hxx
    property.cxx
    property.hxx
    table.cxx
    table.hxx
    tablemgr.cxx
    tablemgr.hxx
    tile.cxx
    tile.hxx
    value.cxx
    value.hxx
    vpf.hxx
    vpfbase.cxx
    vpfbase.hxx
)

add_executable(vpf-dump vpf-dump.cxx)
target_link_libraries(vpf-dump vpf)
if (MSVC)
    set_target_properties(vpf-dump PROPERTIES DEBUG_POSTFIX d )
endif ()

add_executable(vpf-summary vpf-summary.cxx)
target_link_libraries(vpf-summary vpf)
if (MSVC)
    set_target_properties(vpf-summary PROPERTIES DEBUG_POSTFIX d )
endif ()

add_executable(vpf-topology vpf-topology.cxx)
target_link_libraries(vpf-topology vpf)
if (MSVC)
    set_target_properties(vpf-topology PROPERTIES DEBUG_POSTFIX d )
endif ()

# eof
