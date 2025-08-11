# CMakeList.txt : CMake project for VPN_drop_detection, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("VPN_drop_detection")

add_executable (VPN_drop_detection WIN32 "source/VPN_drop_detection.cpp" 
									     "source/VPN_drop_detection.h"
										 "Resource/Resource.h"
										 "Resource/Resource.rc")# WIN32 = no console window

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VPN_drop_detection PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(VPN_drop_detection PRIVATE user32 shell32 Wininet.lib)

