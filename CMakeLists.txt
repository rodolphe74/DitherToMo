cmake_minimum_required(VERSION 3.0)

# git clone https://github.com/ImageMagick/ImageMagick.git
# git checkout tags/7.1.1-13
# ./configure --with-quantum-depth=16 --disable-hdri
# make; make install
# sudo ldconfig

project(dtm)
message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")

# sapfs cmake compilation
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -fomit-frame-pointer -funsigned-char")
add_library(sapfslib STATIC sap2/sapfs.c sap2/libsap.c)
add_executable(sapfs sap2/sapfs.c)
target_link_libraries(sapfs sapfslib)

# main executable
set (cxxflags "-I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
set (cppflags "-g -I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
# set (cppflags "-O2 -I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
set (ldflags "-L/usr/local/lib -lMagick++-7.Q16 -lMagickWand-7.Q16 -lMagickCore-7.Q16")
set (libs "-L/usr/local/lib -lMagick++-7.Q16 -lMagickWand-7.Q16 -lMagickCore-7.Q16")

set(CMAKE_CXX_FLAGS ${cxxflags})
set (CMAKE_SHARED_LINKER_FLAGS ${ldflags})

add_executable(dtm40 DTM40.cpp Floppy.cpp ColorStuff.cpp KMean.cpp ThomsonStuff.cpp ErrorDiffusionDither.cpp)
target_include_directories(dtm40 PRIVATE ${cppflags})
target_link_libraries(dtm40 ${libs} sapfslib)

add_executable(dtm16 DTM16.cpp Floppy.cpp ColorStuff.cpp KMean.cpp ThomsonStuff.cpp ErrorDiffusionDither.cpp)
target_include_directories(dtm16 PRIVATE ${cppflags})
target_link_libraries(dtm16 ${libs} sapfslib)

install(TARGETS dtm40 RUNTIME DESTINATION bin)
install(TARGETS dtm16 RUNTIME DESTINATION bin)
