#
# FindRE2C.cmake
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
FindRE2C
--------

A ``re2c`` module for CMake.

Finds the ``re2c`` executable installed on current system. The following variables will also be set:

``RE2C_FOUND``
  if re2c executable was found
``RE2C_EXECUTABLE``
  pathname of the re2c program
``RE2C_VERSION_STRING``
  version of re2c

#]========================================]

set(RE2C_VERSION 1)

# find re2c, use RE2C if set
if ((NOT RE2C_EXECUTABLE) AND (NOT "$ENV{RE2C}" STREQUAL ""))
    set(RE2C_EXECUTABLE "$ENV{RE2C}" CACHE FILEPATH "re2c executable")
endif ()

set(RE2C_NAMES "re2c")

find_program(RE2C_EXECUTABLE
        NAMES ${RE2C_NAMES}
        DOC "re2c executable"
        )
mark_as_advanced(RE2C_EXECUTABLE)

set(_RE2C_FAILURE_MESSAGE "")
if (RE2C_EXECUTABLE)
    execute_process(
            COMMAND ${RE2C_EXECUTABLE} --version
            OUTPUT_VARIABLE RE2C_VERSION_STRING OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_VARIABLE _RE2C_VERSION_ERROR ERROR_STRIP_TRAILING_WHITESPACE
            RESULT_VARIABLE _RE2C_VERSION_RESULT
    )



    if (NOT _RE2C_VERSION_RESULT EQUAL 0)
        set(RE2C_EXECUTABLE "")
        unset(RE2C_VERSION_STRING)
        string(REPLACE "\n" "\n    " _RE2C_VERSION_ERROR "      ${_RE2C_VERSION_ERROR}")
        string(APPEND _RE2C_FAILURE_MESSAGE
                "The command\n"
                "      \"${RE2C_EXECUTABLE}\" --version\n"
                "    failed with output:\n${RE2C_VERSION_STRING}\n"
                "    stderr:  \n${_RE2C_VERSION_ERROR}\n"
                "    result:  \n${_RE2C_VERSION_RESULT}"
                )
    endif ()

    # re2c version output format is: "re2c x.x.x". Use regex to remove unused content
    string(REGEX REPLACE "re2c[ ]+" "" RE2C_VERSION ${RE2C_VERSION_STRING})
    set(RE2C_FOUND 1)
    unset(RE2C_VERSION_RESULT)
endif ()