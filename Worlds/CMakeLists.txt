# Configure Worlds build

# Add android JNI binding example
if(OGRE_BUILD_ANDROID_JNI_WORLD)
  add_subdirectory(AndroidJNI)
endif()

if (OGRE_BUILD_WORLDS)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Common/include)

  if (OGRE_BUILD_COMPONENT_RTSHADERSYSTEM)
    add_definitions(-DINCLUDE_RTSHADER_SYSTEM)
  	set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreRTShaderSystem)
  endif ()

  if (OGRE_BUILD_COMPONENT_BULLET)
    set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreBullet)
  endif ()

  if (OGRE_BUILD_COMPONENT_OVERLAY)
    set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreOverlay)
  endif ()
  
  if (OGRE_BUILD_COMPONENT_TERRAIN)
  	set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreTerrain)
  endif ()

  if (OGRE_BUILD_COMPONENT_VOLUME)
  	set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreVolume)
  endif ()
  
  if (OGRE_BUILD_COMPONENT_MESHLODGENERATOR AND OGRE_CONFIG_ENABLE_MESHLOD)
  	set(WORLD_DEPENDENCIES ${WORLD_DEPENDENCIES} OgreMeshLodGenerator)
  endif ()
  
  if(EMSCRIPTEN)
    add_subdirectory(Emscripten)
    return()
  endif()

  ## Default Worlds Plugin
  file (GLOB WORLDS_HEADERS ${PROJECT_SOURCE_DIR}/Worlds/*/include)
  include_directories(${WORLDS_HEADERS})

  file(GLOB WORLDS_SOURCES ${PROJECT_SOURCE_DIR}/Worlds/*/src/*.cpp)

  set(BLACKLIST Creator TerrainTessellation)
  if(NOT OGRE_BUILD_PLUGIN_PCZ)
    list(APPEND BLACKLIST PCZTestApp)
  endif()
  if(NOT OGRE_BUILD_COMPONENT_TERRAIN)
    list(APPEND BLACKLIST EndlessWorld)
  endif()

  foreach(FILTER_WORLD ${BLACKLIST})
    file(GLOB BLACKLIST_CPP ${PROJECT_SOURCE_DIR}/Worlds/${FILTER_WORLD}/src/*.cpp)
    list(REMOVE_ITEM WORLDS_SOURCES ${BLACKLIST_CPP})
  endforeach()

  add_definitions(${OGRE_VISIBILITY_FLAGS})
  add_library(DefaultWorlds ${OGRE_LIB_TYPE}
              ${CMAKE_CURRENT_SOURCE_DIR}/Common/src/DefaultWorldsPlugin.cpp
              ${WORLDS_SOURCES} ${WORLDS_HEADERS})
  target_link_libraries(DefaultWorlds OgreMain ${WORLD_DEPENDENCIES} OgreBites)

  if(OGRE_BUILD_PLUGIN_PCZ)
    target_link_libraries(DefaultWorlds Plugin_PCZSceneManager)
    target_compile_definitions(DefaultWorlds PRIVATE -DHAVE_PCZ_PLUGIN)
  endif()

  if(OGRE_BUILD_COMPONENT_OVERLAY_IMGUI)
    target_compile_definitions(DefaultWorlds PRIVATE -DHAVE_IMGUI)
  endif()

  ogre_config_world_lib(DefaultWorlds)

  if (APPLE AND NOT APPLE_IOS)
      # Set the INSTALL_PATH so that Worlds can be installed in the application package
      set_target_properties(DefaultWorlds
        PROPERTIES BUILD_WITH_INSTALL_RPATH 1
        INSTALL_NAME_DIR "@executable_path/../Plugins"
      )
  endif()

  ## Add Creator last
  add_subdirectory(Creator)
endif ()
		
if (MSVC AND OGRE_BUILD_WORLDS)
  find_package(Wix)
  if (Wix_FOUND)
    # Create WiX setup for demo build
    configure_file(${OGRE_TEMPLATES_DIR}/demos.wxs.in ${CMAKE_CURRENT_BINARY_DIR}/demos.wxs @ONLY)
    configure_file(${OGRE_TEMPLATES_DIR}/demomedia.wxi.in ${CMAKE_CURRENT_BINARY_DIR}/demomedia.wxi @ONLY)
    configure_file(${OGRE_TEMPLATES_DIR}/DemoLicense.rtf ${CMAKE_CURRENT_BINARY_DIR}/DemoLicense.rtf COPYONLY)
	# Configure files, set media dir temporarily
	set(OGRE_MEDIA_DIR_TMP ${OGRE_MEDIA_DIR_REL})
	set(OGRE_MEDIA_DIR_REL "Media")
    configure_file(${OGRE_TEMPLATES_DIR}/resources.cfg.in ${CMAKE_CURRENT_BINARY_DIR}/resources.cfg @ONLY)
	# restore
	set(OGRE_MEDIA_DIR_REL ${OGRE_MEDIA_DIR_TMP})
    add_custom_target(demo_installer 
    COMMAND ${Wix_BINARY_DIR}/candle demos.wxs 
	  COMMAND ${Wix_BINARY_DIR}/light -ext WixUIExtension -cultures:en-us -out OgreDemos_v${OGRE_VERSION_DASH_SEPARATED}.msi demos.wixobj
      WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
      COMMENT "Building demo installer" VERBATIM
    )
	# Make sure we build worlds first
	add_dependencies(demo_installer Creator)
  endif()
endif()
