kcp is a A Fast and Reliable ARQ Protocol (Can use with UDP)   https://github.com/skywind3000/kcp
This project wrap kcp by boost asio. Making it easy to use.
And it's a bench mark test also.
You can see the bench mark test result at https://github.com/libinzhangyuan/reliable_udp_bench_mark


Compile
1. I using gcc 4.8
   Other version of gcc that support c++11 should be OK. You can compile asio_kcp as verifying.

2. I using boost 1.58
   Other version of boost should be OK. You can compile asio_kcp as verifying.

3. Build g2log
   download g2log from http://www.codeproject.com/Articles/288827/g-log-An-efficient-asynchronous-logger-using-Cplus#TOC_initialization
       or just use the package in third_party.
   cope g2log folder to third_party folder
   $ cd third_party/g2log/ &&  mkdir build && cd build && cmake .. && make

4. Build muduo
   $ cd third_party/
   $ git clone https://github.com/chenshuo/muduo.git      # - or just using the pack in third_party folder.
   $ cd muduo
   compile on OSX:  $ patch -p1 < MacOSX.diff
        edit muduo/CMakeLists.txt.
            find set(CXX_FLAGS  then add 2 new line:
                -D__GXX_EXPERIMENTAL_CXX0X__
                -std=c++11
            comment -march=native
            comment set(CMAKE_CXX_COMPILER "clang++")
            uncomment -DMUDUO_STD_STRING
   compile on linux:
        edit muduo/CMakeLists.txt.
            find set(CXX_FLAGS  then add 2 new line:
                -D__GXX_EXPERIMENTAL_CXX0X__
                -std=c++11
            uncomment -DMUDUO_STD_STRING
   modify muduo/base/LogStream.h   kSmallBuffer = 4000  ->  kSmallBuffer = 4000*4
   adding VERBOSE=1 to "make" in muduo/build.sh will show detail of compiling.
   $ CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . build.sh

5. modify the BOOST_LIB_PATH and BOOST_INC_PATH in allmake.sh
6. do compiling at project root folder: $ . allmake.sh

The third_party folder should be like below,
▾ third_party/
  ▾ g2log/
    ▸ build/
    ▸ src/
    ▸ test_example/
    ▸ test_performance/
    ▸ test_unit/
      CMakeLists.txt
  ▸ muduo/


Compile unit test
1. download gtest from https://googletest.googlecode.com/files/gtest-1.7.0.zip
     or just use the package in third_party folder.
2. unpack the gtest-1.7.0.zip. coping gtest-1.7.0 folder to asio_kcp/third_party/
3. $ cd gtest-1.7.0 && ./configure && make
4. download googlemock from https://googlemock.googlecode.com/files/gmock-1.7.0.zip
     or just use the package in third_party folder.
5. unpack the gmock-1.7.0.zip. coping gmock-1.7.0 folder to asio_kcp/third_party/
6. $ cd gmock-1.7.0 && ./configure && make
7. back to asio_kcp folder then $ . utest_make.sh
8. $ ./asio_kcp_utest/asio_kcp_utest

The third_party folder should be like below,
▾ third_party/
  ▸ g2log/
  ▸ gmock-1.7.0/
  ▸ gtest-1.7.0/
  ▸ muduo/



Run example test
 a. filter the verbose log from asio timer
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
 b. filter all asio log
    ./client_with_asio/client_with_asio 23425 127.0.0.1 12345 500 2>/dev/null


4. run bench mark test of kcp.
git checkout kcp_bench_mark_test
compile
the run.
    ./asio_kcp_server/asio_kcp_server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
    ./asio_kcp_client/asio_kcp_client 23445 120.26.200.117 12345 500 2>/dev/null



Centos 6.5 install help:
1. boost
  http://www.boost.org/doc/libs/1_58_0/more/getting_started/unix-variants.html
  tar jxvf boost_1_58_0.tar.bz2
  cd boost_1_58_0
  ./bootstrap.sh
  sudo ./b2 install --build-type=complete --layout=tagged

2. error when running: /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.15' not found
   strings /usr/lib64/libstdc++.so.6 | grep GLIBC   //  check the GLIBCXX_3.4.15. It is not in it.
   sudo find / -name "libstdc++.so*" 2>/dev/null  // find the latest libstdc++.so   It should be /usr/local/lib64/libstdc++.so.6.0.18
   sudo rm /usr/lib64/libstdc++.so.6
   sudo ln -s /usr/local/lib64/libstdc++.so.6.0.18 /usr/lib64/libstdc++.so.6
   sudo vim /etc/ld.so.conf   // add a new line: /usr/lib64
   sudo ldconfig // all ok



OSX install help
1. gcc 4.8
  install port if do not have one:  https://www.macports.org/install.php
  port search gcc   # show gcc version
  sudo /opt/local/bin/port install gcc48
  sudo /opt/local/bin/port select --set gcc mp-gcc48
  last step,  close old shell, open a new shell.
      https://apple.stackexchange.com/questions/118830/install-gcc-to-mac-via-terminal
      http://www.tuicool.com/articles/reqiY3

2. boost 1.58
  look at the centos step in https://stackoverflow.com/questions/104322/how-do-you-install-boost-on-macos


