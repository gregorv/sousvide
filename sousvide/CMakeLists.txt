cmake_minimum_required(VERSION 2.8.11)
project(sousvide)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5PrintSupport REQUIRED)
find_package(Qt5SerialPort REQUIRED)

include_directories("${CMAKE_SOURCE_DIR}/src")
add_definitions("--std=c++17")

set(sousvide_SRC
  src/main.cpp
  src/sousvide.cpp
  src/sousvidecontrol.cpp
  src/modifyconstants.cpp
  src/qcustomplot.cpp
  src/statusplot.cpp
  icons.qrc
)

# Create code from a list of Qt designer ui files.
#set(CMAKE_AUTOUIC ON) # use this if you have CMake 3.x instead of the following
qt5_wrap_ui(sousvide_SRC src/sousvide.ui src/pidconst.ui)

# Tell CMake to create the helloworld executable
add_executable(sousvide ${sousvide_SRC})

# Use the Widgets module from Qt 5.
target_link_libraries(sousvide Qt5::Widgets Qt5::PrintSupport Qt5::SerialPort)

# Install the executable
install(TARGETS sousvide DESTINATION bin)
