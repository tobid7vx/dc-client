# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Project33/dc/build/_deps/discord_game_sdk-src"
  "C:/Project33/dc/build/_deps/discord_game_sdk-build"
  "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix"
  "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/tmp"
  "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/src/discord_game_sdk-populate-stamp"
  "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/src"
  "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/src/discord_game_sdk-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/src/discord_game_sdk-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Project33/dc/build/_deps/discord_game_sdk-subbuild/discord_game_sdk-populate-prefix/src/discord_game_sdk-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
