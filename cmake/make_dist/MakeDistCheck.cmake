include (MakeDistFiles)

function (RUN_DIST_CHECK PACKAGE_PREFIX EXT)

   set (tarball ${PACKAGE_PREFIX}.tar${EXT})
   if (NOT EXISTS ${tarball})
      message (FATAL_ERROR "Can't find dist tarball '${tarball}'")
   endif ()

   # Remove the directory to which we're about to extract
   file (REMOVE_RECURSE ${PACKAGE_PREFIX})

   # Untar the distribution we want to check
   set (TAR_OPTION "zxf")
   if (${EXT} STREQUAL ".bz2")
      set (TAR_OPTION "jxf")
   endif ()

   set (MY_CMAKE_COMMAND "")
   set (MY_CMAKE_COMMAND ${CMAKE_COMMAND} -E env)

   find_program (MAKE_COMMAND NAMES make gmake)
   if (${MAKE_COMMAND} STREQUAL "MAKE_COMMAND-NOTFOUND")
      message (FATAL_ERROR "Can't find the 'make' or 'gmake' program.")
   endif ()

   execute_process_and_check_result (
      COMMAND ${CMAKE_COMMAND} -E tar ${TAR_OPTION} ${tarball}
      WORKING_DIRECTORY .
      ERROR_MSG "Command to untar ${tarball} failed."
   )

   set (BUILD_DIR "_cmake_build")
   set (INSTALL_DIR "_cmake_install")
   file (REMOVE_RECURSE ${BUILD_DIR} ${INSTALL_DIR})

   file (MAKE_DIRECTORY ${BUILD_DIR} ${INSTALL_DIR})

   execute_process_and_check_result (
      COMMAND ${CMAKE_COMMAND}
         -DCMAKE_BUILD_TYPE=Release
         -DMONGOCXX_ENABLE_SLOW_TESTS=ON
         -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
         -DCMAKE_INSTALL_PREFIX=../${INSTALL_DIR}
         -DMONGOCXX_ENABLE_SLOW_TESTS=ON
         ../${PACKAGE_PREFIX}
      WORKING_DIRECTORY ${BUILD_DIR}
      ERROR_MSG "CMake configure command failed."
   )

   # Run make in the build directory
   if (DEFINED ENV{DISTCHECK_BUILD_OPTS})
      set (build_opts $ENV{DISTCHECK_BUILD_OPTS})
   else ()
      set (build_opts "-j 8")
   endif ()
   separate_arguments (build_opts)
   execute_process_and_check_result (
      COMMAND ${MY_CMAKE_COMMAND} ${MAKE_COMMAND} ${build_opts}
      WORKING_DIRECTORY ${BUILD_DIR}
      ERROR_MSG "Make build failed."
   )

   # Run make install
   set (install_opts $ENV{DISTCHECK_INSTALL_OPTS})
   separate_arguments (install_opts)
   execute_process_and_check_result (
      COMMAND ${MY_CMAKE_COMMAND} ${MAKE_COMMAND} install ${install_opts}
      WORKING_DIRECTORY ${BUILD_DIR}
      ERROR_MSG "Make install failed."
   )

   # Run make dist
   execute_process_and_check_result (
      COMMAND ${MY_CMAKE_COMMAND} ${MAKE_COMMAND} dist
      WORKING_DIRECTORY ${BUILD_DIR}
      ERROR_MSG "Make dist failed."
   )

   message ("distcheck complete.")

endfunction ()

run_dist_check (${PACKAGE_PREFIX} .gz)
