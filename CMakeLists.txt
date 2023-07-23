cmake_minimum_required(VERSION 3.0)

# git clone https://github.com/ImageMagick/ImageMagick.git
# git checkout tags/7.1.1-13
# ./configure --with-quantum-depth=16 --disable-hdri
# make; make install
# sudo ldconfig

project(dtm)
message(PROJECT_SOURCE_DIR="${PROJECT_SOURCE_DIR}")


set (cxxflags "-I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
# set (cppflags "-g -I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
set (cppflags "-O2 -I/usr/local/include/ImageMagick-7 -DMAGICKCORE_HDRI_ENABLE=0 -DMAGICKCORE_QUANTUM_DEPTH=16")
set (ldflags "-L/usr/local/lib -lMagick++-7.Q16 -lMagickWand-7.Q16 -lMagickCore-7.Q16")
set (libs "-L/usr/local/lib -lMagick++-7.Q16 -lMagickWand-7.Q16 -lMagickCore-7.Q16")


add_executable(dtm main.cpp ColorStuff.cpp ThomsonStuff.cpp ErrorDiffusionDither.cpp)

set(CMAKE_CXX_FLAGS ${cxxflags})
set (CMAKE_SHARED_LINKER_FLAGS ${ldflags})
target_include_directories(dtm PRIVATE ${cppflags})
target_link_libraries(dtm ${libs})

install(TARGETS dtm RUNTIME DESTINATION bin)
