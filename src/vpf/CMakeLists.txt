
# vpf EXECUTABLE from [C:\GTools\ConApps\tests\test-btg\src\vpf],
# have 19 C/C++ sources, 16 headers
set(name pd_vpf)
set(dir .)
set(${name}_SRCS
    ${dir}/component.cxx
    ${dir}/contour.cxx
    ${dir}/coverage.cxx
    ${dir}/database.cxx
    ${dir}/feature.cxx
    ${dir}/label.cxx
    ${dir}/library.cxx
    ${dir}/line.cxx
    ${dir}/polygon.cxx
    ${dir}/property.cxx
    ${dir}/table.cxx
    ${dir}/tablemgr.cxx
    ${dir}/tile.cxx
    ${dir}/value.cxx
    ${dir}/vpfbase.cxx
    )
set(${name}_HDRS
    ${dir}/component.hxx
    ${dir}/contour.hxx
    ${dir}/coverage.hxx
    ${dir}/database.hxx
    ${dir}/feature.hxx
    ${dir}/label.hxx
    ${dir}/library.hxx
    ${dir}/line.hxx
    ${dir}/polygon.hxx
    ${dir}/property.hxx
    ${dir}/table.hxx
    ${dir}/tablemgr.hxx
    ${dir}/tile.hxx
    ${dir}/value.hxx
    ${dir}/vpf.hxx
    ${dir}/vpfbase.hxx
    )
add_library( ${name} ${${name}_SRCS} ${${name}_HDRS} )
set( VPF_LIB ${name} )

set(name vpf-dump2)
set(dir .)
set(${name}_SRCS
    ${dir}/vpf-dump.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
target_link_libraries( ${name} ${VPF_LIB} )
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
    
#    ${dir}/vpf-summary.cxx
set(name vpf-summary2)
set(dir .)
set(${name}_SRCS
    ${dir}/vpf-summary.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
target_link_libraries( ${name} ${VPF_LIB} )
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
#    ${dir}/vpf-topo-org.cxx
#    ${dir}/vpf-topology.cxx
set(name vpf-topology2)
set(dir .)
set(${name}_SRCS
    ${dir}/vpf-topology.cxx
    )
add_executable( ${name} ${${name}_SRCS} )
target_link_libraries( ${name} ${VPF_LIB} )
if (MSVC)
    set_target_properties( ${name} PROPERTIES DEBUG_POSTFIX d )
endif ()
    
# deal with install, if any...
#install( TARGETS ${name} DESTINATION bin )

# eof
