
#
# Dependencies
#

# GSL
macro(add_gsl)
find_package (GSL REQUIRED)
    message(STATUS "GSL includes: " ${GSL_INCLUDE_DIR} )
    message(STATUS "GSL libs: ${GSL_LIBRARIES}" )
    target_include_directories(${PROJECT_NAME} PUBLIC ${GSL_INCLUDE_DIR})
    target_link_libraries(${PROJECT_NAME} PUBLIC ${GSL_LIBRARIES})
endmacro(add_gsl)
