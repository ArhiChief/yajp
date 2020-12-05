#
# FindLemon.cmake
# Copyright (C) 2020 Sergei Kosivchenko <arhichief@gmail.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
# documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
# rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all copies or substantial portions of
# the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
# THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
# TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

#[========================================[.rst:
FindLemon
---------

A ``lemon`` module for CMake.

Finds the ``lemon`` executable installed on current system. The following variables will also be set:

``LEMON_FOUND``
  if lemon executable was found
``LEMON_EXECUTABLE``
  pathname of the lemon program
``LEMON_VERSION_STRING``
  version of lemon

#]========================================]

set(LEMON_VERSION 1)

# find lemon, use LEMON if set
if ((NOT LEMON_EXECUTABLE) AND (NOT "$ENV{LEMON}" STREQUAL ""))
    set(LEMON_EXECUTABLE "$ENV{LEMON}" CACHE FILEPATH "lemon executable")
endif ()

set(LEMON_NAMES "lemon")

find_program(LEMON_EXECUTABLE
        NAMES ${LEMON_NAMES}
        DOC "lemon executable"
        )
mark_as_advanced(LEMON_EXECUTABLE)

set(_LEMON_FAILURE_MESSAGE "")
if (LEMON_EXECUTABLE)
    execute_process(
            COMMAND ${LEMON_EXECUTABLE} -x
            OUTPUT_VARIABLE LEMON_VERSION_STRING OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE _LEMON_VERSION_ERROR ERROR_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _LEMON_VERSION_RESULT
    )

    if (NOT _LEMON_VERSION_RESULT EQUAL 0)
        set(LEMON_EXECUTABLE "")
        unset(LEMON_VERSION_STRING)
        string(REPLACE "\n" "\n    " _LEMON_VERSION_ERROR "      ${_LEMON_VERSION_ERROR}")
        string(APPEND _LEMON_FAILURE_MESSAGE
                "The command\n"
                "      \"${LEMON_EXECUTABLE}\" --version\n"
                "    failed with output:\n${LEMON_VERSION_STRING}\n"
                "    stderr:  \n${_LEMON_VERSION_ERROR}\n"
                "    result:  \n${_LEMON_VERSION_RESULT}"
                )
    endif ()
    # lemon version output format is: "Lemon version x.x". Use regex to remove unused content
    string(REGEX REPLACE "[A-Za-z ]+" "" LEMON_VERSION ${LEMON_VERSION_STRING})
    set(LEMON_FOUND 1)
    unset(LEMON_VERSION_RESULT)
endif ()