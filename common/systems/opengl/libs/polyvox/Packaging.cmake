################################################################################
# The MIT License (MIT)
#
# Copyright (c) 2015 Matthew Williams and David Williams
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
################################################################################

#INCLUDE(InstallRequiredSystemLibraries)

SET(CPACK_PACKAGE_NAME "PolyVox SDK")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "PolyVox SDK")
SET(CPACK_PACKAGE_VENDOR "Thermite 3D Team")
#SET(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#SET(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
SET(CPACK_PACKAGE_VERSION_MAJOR ${POLYVOX_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${POLYVOX_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${POLYVOX_VERSION_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "PolyVox SDK ${POLYVOX_VERSION}")
IF(WIN32 AND NOT UNIX)
  # There is a bug in NSIS that does not handle full unix paths properly.
  # Make sure there is at least one set of four backslashes.
  #SET(CPACK_PACKAGE_ICON "${CMake_SOURCE_DIR}/Utilities/Release\\\\InstallIcon.bmp")
  #SET(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\MyExecutable.exe")
  SET(CPACK_NSIS_DISPLAY_NAME "PolyVox SDK ${POLYVOX_VERSION}")
  SET(CPACK_NSIS_HELP_LINK "http:\\\\\\\\thermite3d.org/phpBB/")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\thermite3d.org")
  SET(CPACK_NSIS_CONTACT "matt@milliams.com")
  SET(CPACK_NSIS_MODIFY_PATH ON)
ELSE(WIN32 AND NOT UNIX)
  #SET(CPACK_STRIP_FILES "bin/MyExecutable")
  #SET(CPACK_SOURCE_STRIP_FILES "")
ENDIF(WIN32 AND NOT UNIX)
#SET(CPACK_PACKAGE_EXECUTABLES "MyExecutable" "My Executable")

INCLUDE(CPack)

CPACK_ADD_COMPONENT(library DISPLAY_NAME "Library" DESCRIPTION "The runtime libraries" REQUIRED)
CPACK_ADD_COMPONENT(development DISPLAY_NAME "Development" DESCRIPTION "Files required for developing with PolyVox" DEPENDS library)
CPACK_ADD_COMPONENT(example DISPLAY_NAME "OpenGL Example" DESCRIPTION "A PolyVox example application using OpenGL" DEPENDS library)
cpack_add_component_group(bindings DISPLAY_NAME "Bindings" DESCRIPTION "Language bindings")
CPACK_ADD_COMPONENT(python DISPLAY_NAME "Python Bindings" DESCRIPTION "PolyVox bindings for the Python language" DISABLED GROUP bindings DEPENDS library)
