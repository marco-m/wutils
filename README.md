WUtils
======

[![Build Status](https://travis-ci.org/marco-m/wutils.svg)](https://travis-ci.org/marco-m/wutils)

WUtils is a set of C++11 utility classes for the Wt C++/JavaScript web framework.
I am not associated with Wt (Wt: http://www.webtoolkit.eu/wt) or Emweb, but I like it :-)

The code needs a C++11 compiler, at least g++ 4.9 (unfortunately g++ 4.8 has a non-working std::regex) or clang 3.4.

Usage and documentation
-----------------------

Urlrouter: A regex-based URL router for Wt inspired by the smart Django "URLconf". See https://docs.djangoproject.com/en/dev/topics/http/urls

See also the comments in `urlrouter.hpp` and the files in the `test` directory.

Build
-----

For the time being it is an include-only library, no need to compile. Just include `urlrouter.hpp` in your project.

Testing
-------

For running the UT, you need Google Mock, Google Test, Wt built with the UT library and Boost.

Under the directory `scripts` there are script to fetch and build Google Mock and Wt.

Known issues
------------

* This is innocuous and has been fixed in wt after the release.

    test-urlrouter.cpp:85:16: warning: deleting object of polymorphic class type ‘Wt::Test::WTestEnvironment’ which has non-virtual destructor might cause undefined behaviour [-Wdelete-non-virtual-dtor]


* g++ 4.8 comes with broken std::regex (http://gcc.gnu.org/bugzilla/show_bug.cgi?id=53631), so you need either clang 3.4+ or g++ 4.9+


License
-------

WUtils is released under the the BSD 2-clause license (see file LICENSE.txt).

Pull requests appreciated.
