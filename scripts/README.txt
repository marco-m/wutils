These are a set of files needed to build and test Wutils on travis-ci.

Since travis-ci provides an old version of Ubuntu (12.04) and the first g++ to provide a working std::regex is g++4.9, it took me an absurd amount of time to figure out how to build and test on travis.

I am giving up on g++ and using the very good clang++.

Clang is composed of two parts, the compiler clang++ and the standard library libc++. There is a PPA on Ubuntu that provides the compiler, but not the library, so we get and install a pre-built version I did.
