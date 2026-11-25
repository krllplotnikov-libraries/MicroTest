# Removes the files listed in install_manifest.txt (created by "cmake --install").
if(NOT EXISTS "${MANIFEST}")
	message(FATAL_ERROR "Install manifest not found: ${MANIFEST}. Run the install first.")
endif()

file(STRINGS "${MANIFEST}" installedFiles)
foreach(installedFile IN LISTS installedFiles)
	if(EXISTS "${installedFile}" OR IS_SYMLINK "${installedFile}")
		message(STATUS "Removing: ${installedFile}")
		file(REMOVE "${installedFile}")
	endif()
endforeach()
