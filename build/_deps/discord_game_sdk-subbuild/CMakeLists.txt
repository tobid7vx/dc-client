# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.24.3)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(discord_game_sdk-populate NONE)



include(ExternalProject)
ExternalProject_Add(discord_game_sdk-populate
                     "UPDATE_DISCONNECTED" "False" "DOWNLOAD_EXTRACT_TIMESTAMP" "TRUE" "URL" "https://dl-game-sdk.discordapp.net/2.5.6/discord_game_sdk.zip"
                    SOURCE_DIR          "C:/Project33/dc/build/_deps/discord_game_sdk-src"
                    BINARY_DIR          "C:/Project33/dc/build/_deps/discord_game_sdk-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
                    USES_TERMINAL_PATCH     YES
)


