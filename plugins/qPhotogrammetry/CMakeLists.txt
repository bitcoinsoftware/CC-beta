cmake_minimum_required(VERSION 3.0)

option( INSTALL_QPHOTOGRAMMETRY_PLUGIN "Check to install QPHOTOGRAMMETRY plugin" OFF )

# CloudCompare 'qPhotogrammetry' plugin
if (INSTALL_QPHOTOGRAMMETRY_PLUGIN)
    add_subdirectory (SocketStub)

    project( QPHOTOGRAMMETRY_PLUGIN )

	find_package( OpenGL REQUIRED )
	if( NOT OPENGL_FOUND )
		message( SEND_ERROR "OpenGL required, but not found with 'find_package()'" )
	endif()
    
    include( ../CMakePluginTpl.cmake )

    include_directories( ${SOCKETSTUB_LIB_SOURCE_DIR} )

    target_link_libraries( ${PROJECT_NAME} SOCKETSTUB_LIB )

endif()
