# From: https://stackoverflow.com/a/7216542
function(JOIN VALUES GLUE OUTPUT)
  string (REGEX REPLACE "([^\\]|^);" "\\1${GLUE}" _TMP_STR "${VALUES}")
  string (REGEX REPLACE "[\\](.)" "\\1" _TMP_STR "${_TMP_STR}") #fixes escaping
  set (${OUTPUT} "${_TMP_STR}" PARENT_SCOPE)
endfunction()

# convert msys paths like /c/foo to windows paths like c:/foo
# for variables set by FindWxWidgets
function(normalize_wx_paths)
    if(MSYS)
        unset(new_paths)
        foreach(p ${wxWidgets_LIBRARY_DIRS})
            execute_process(COMMAND cygpath -m "${p}" OUTPUT_VARIABLE p_win OUTPUT_STRIP_TRAILING_WHITESPACE)
            list(APPEND new_paths "${p_win}")
        endforeach()

        set(wxWidgets_LIBRARY_DIRS ${new_paths} PARENT_SCOPE)

        string(REGEX REPLACE "((^| )[^/]*)/([a-zA-Z])/" "\\1\\3:/" new_libs "${wxWidgets_LIBRARIES}")

        set(wxWidgets_LIBRARIES ${new_libs} PARENT_SCOPE)
    endif()
endfunction()

macro(cleanup_wx_vars)
    if(wxWidgets_CXX_FLAGS)
        list(REMOVE_ITEM wxWidgets_CXX_FLAGS -fpermissive)
    endif()
endmacro()

function(cygpath var path)
    execute_process(
        COMMAND cygpath -m ${path}
        OUTPUT_VARIABLE cyg_path
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
    set(${var} ${cyg_path} PARENT_SCOPE)
endfunction()

function(check_clean_exit var)
    execute_process(
        COMMAND ${ARGN}
        RESULT_VARIABLE exit_status
        OUTPUT_QUIET
        ERROR_QUIET
    )

    if(NOT ${exit_status} EQUAL 0)
        # special case for msys2, where programs might complain about
        # not being win32 programs
        unset(cmd_str)
        foreach(param IN LISTS ARGN)
            set(cmd_str "${cmd_str} ${param}")
        endforeach()
        string(STRIP "${cmd_str}" cmd_str)

        execute_process(
            COMMAND sh -c "${cmd_str}"
            RESULT_VARIABLE exit_status
            OUTPUT_QUIET
            ERROR_QUIET
        )
    endif()

    set(${var} ${exit_status} PARENT_SCOPE)
endfunction()

function(find_wx_util var util)
    # on win32, including cross builds we prefer the plain utility name first from PATH
    if(WIN32)
        set(conf_suffixes  "" gtk4u gtk4 gtk3u gtk3 gtk2u gtk2)
        set(major_versions "" 4 3 2)
    else()
        set(conf_suffixes  gtk4u gtk4 gtk3u gtk3 gtk2u gtk2 "")
        set(major_versions 4 3 2 "")
    endif()

    foreach(conf_suffix IN LISTS conf_suffixes)
        foreach(major_version IN LISTS major_versions)
            foreach(minor_version RANGE 100 -1 -1)
                unset(suffix)
                if(conf_suffix)
                    set(suffix "-${conf_suffix}")
                endif()
                if(major_version)
                    set(suffix "${suffix}-${major_version}")

                    if(NOT minor_version EQUAL -1)
                        set(suffix "${suffix}.${minor_version}")
                    endif()
                endif()

                # find_program caches the result
                set(exe NOTFOUND CACHE INTERNAL "" FORCE)
                find_program(exe NAMES "${util}${suffix}")

                # try infix variant, as on FreeBSD
                if(NOT EXISTS ${exe})
                    string(REGEX REPLACE "^-" "" suffix "${suffix}")

                    string(REGEX REPLACE "-" "${suffix}-" try ${util})

                    set(exe NOTFOUND CACHE INTERNAL "" FORCE)
                    find_program(exe NAMES ${try})
                endif()

                if(EXISTS ${exe})
                    # check that the utility can be executed cleanly
                    # in case we find e.g. the wrong architecture binary
                    # when cross-compiling
                    check_clean_exit(exit_status ${exe} --help)

                    if(exit_status EQUAL 0)
                        set(${var} ${exe} PARENT_SCOPE)
                        return()
                    endif()
                endif()

                # don't iterate over minor versions for empty major version
                if(major_version STREQUAL "")
                    break()
                endif()
            endforeach()
        endforeach()

        # default to util name if not found, so the error is more clear
        set(${var} ${util} PARENT_SCOPE)
    endforeach()
endfunction()
