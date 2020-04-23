#!/bin/sh

# I'm too stupid to convince cmake to link dl, hence this script

# platform/unix/automagic
# ./configure --enable-static --disable-shared

make -j8
(cd src ; g++ -o love love.o  ./.libs/liblove.a /usr/lib/x86_64-linux-gnu/libfreetype.so -lSDL2 -lopenal -lz -lmodplug /usr/lib/x86_64-linux-gnu/libvorbisfile.so -ltheoradec -logg -lluajit-5.1 -lmpg123 -lpthread -ldl)
