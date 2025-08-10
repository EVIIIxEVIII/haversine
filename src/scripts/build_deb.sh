#!/bin/bash

g++ -O1 -DPROFILE=1  -g -fno-omit-frame-pointer \
    -fsanitize=address,undefined \
    -Wall -Wextra -Wpedantic \
    -Wshadow -Wconversion -Wsign-conversion \
    -Wfloat-conversion -Wdouble-promotion \
    -Wformat=2 -Wundef -Wcast-align \
    -Wnon-virtual-dtor -Woverloaded-virtual \
    -Wnull-dereference -Wold-style-cast \
    -Wstringop-overflow=4 -Warray-bounds=2 \
    main.cpp -o haversine_deb

