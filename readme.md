### asio_kcp - a fast and reliable udp framework. 
#### This framework will be the fastest protocal for pvp game.

#### kcp - a A Fast and Reliable ARQ Protocol (Can use with UDP)
##### website of kcp: https://github.com/skywind3000/kcp
##### This project wrap kcp by boost asio. Making it easy to use.


### Feature
* Using in realtime pvp game that need miniest network delay. For example, the multiplayer first person shooting game.
* Reliable UDP protocal.
* Minimize delay.  And the worst delay should be good. 3 or more times better than enet when worst network lag happen.
* Has good performace in wifi and phone network(3G, 4G).
* Extra using 20% ~ 50% network flow for speed improvement.


### Bench mark
```
worst network lag happen:
asio: 10:51.21
291  295   269   268   231   195   249   230   225   204

enet: 10:51.21
1563   1520    1470    1482    1438    1454    1412    1637    1588    1540
```

##### You can see the bench mark test result at [here](https://github.com/libinzhangyuan/reliable_udp_bench_mark)
##### Or you can see the bench mark detail log at bench_mark folder.




### client_lib
#### Feature:
* Do not using boost. And do not using feature of c++11
* It's implement by original thread and socket.
* [client_lib_usage](./client_lib_usage.md)




### Recommend: use Dockerfile to compiling the server and client.
* Please read the doc:  [docker/docker_readme.md](./docker/docker_readme.md)


<br>
### Or create compiling env yourself<br>
* I using gcc 4.8
* Other version of gcc that support c++11 should be OK. You can compile asio_kcp as verifying.

* I using boost 1.58
    * Other version of boost should be OK. You can compile asio_kcp as verifying.

* Build g2log
    * download g2log from [official website](http://www.codeproject.com/Articles/288827/g-log-An-efficient-asynchronous-logger-using-Cplus#TOC_initialization)
    * -  or using the package in third_party folder.
    * cope g2log folder to third_party folder
    * $ cd third_party/g2log/ &&  mkdir build && cd build && cmake .. && make

* Build muduo
    * $ cd third_party/
    * $ git clone https://github.com/chenshuo/muduo.git
    * -  or using the package in third_party folder. third_party folder.
    * $ cd muduo
    * compile on OSX:  $ patch -p1 < MacOSX.diff
        * edit muduo/CMakeLists.txt.
            * find set(CXX_FLAGS  then add 2 new line:
                * -D__GXX_EXPERIMENTAL_CXX0X__
                * -std=c++11
            * comment -march=native
            * comment set(CMAKE_CXX_COMPILER "clang++")
            * comment -Werror
            * uncomment -DMUDUO_STD_STRING
    * compile on linux:
        * edit muduo/CMakeLists.txt.
            * find set(CXX_FLAGS  then add 2 new line:
                * -D__GXX_EXPERIMENTAL_CXX0X__
                * -std=c++11
            * uncomment -DMUDUO_STD_STRING
            * comment -Werror
    * modify muduo/base/LogStream.h   kSmallBuffer = 4000  ->  kSmallBuffer = 4000*4
    * adding VERBOSE=1 to "make" in muduo/build.sh will show detail of compiling.
    * $ CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . ./build.sh

* modify the BOOST_LIB_PATH and BOOST_INC_PATH in allmake.sh
* do compiling at project root folder: $ . allmake.sh

The third_party folder should like below,
```
▾ third_party/
  ▾ g2log/
    ▸ build/
    ▸ src/
    ▸ test_example/
    ▸ test_performance/
    ▸ test_unit/
      CMakeLists.txt
  ▸ muduo/
```

### Compile unit test
* download gtest from [website](https://googletest.googlecode.com/files/gtest-1.7.0.zip)
    * or just use the package in third_party folder.
* unpack the gtest-1.7.0.zip. coping gtest-1.7.0 folder to asio_kcp/third_party/
* $ cd gtest-1.7.0 && ./configure && make
* download googlemock from https://googlemock.googlecode.com/files/gmock-1.7.0.zip
    * or just use the package in third_party folder.
* unpack the gmock-1.7.0.zip. coping gmock-1.7.0 folder to asio_kcp/third_party/
* $ cd gmock-1.7.0 && ./configure && make
* back to asio_kcp folder then $ . utest_make.sh
* $ ./asio_kcp_utest/asio_kcp_utest

The third_party folder should like below,
```
▾ third_party/
  ▸ g2log/
  ▸ gmock-1.7.0/
  ▸ gtest-1.7.0/
  ▸ muduo/
```


### Run example test
##### filter the verbose log from asio timer
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
##### filter all asio log
    ./client_with_asio/client_with_asio 23425 127.0.0.1 12345 500 2>/dev/null




### run bench mark test of kcp.
* Server on your server
```
git checkout kcp_bench_mark_test
read the readme.md in this branch. and do compiling thing.
then run server on your server:
  ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
```
* run client on your client computer
```
git checkout kcp_bench_mark_test
read the readme.md in this branch. and do compiling thing.
./client_with_asio/client_with_asio 23445 120.26.200.117 12345 500 2>/dev/null
# Note: changing the ip and port to your server which is running the asio_kcp_server
```

### how to test 3G/4G
* if you want to test the 3G/4G. you can share the wifi on your phone by using wiless AP. Making your client computer connect to this wifi.
* run client on your client computer (Note: changing the ip and port to your server)




### Centos 6.5 install help:
#### boost  http://www.boost.org/doc/libs/1_58_0/more/getting_started/unix-variants.html
* $ yum -y install python-devel
* $ tar jxvf boost_1_58_0.tar.bz2
* $ cd boost_1_58_0
* $ ./bootstrap.sh
* $ sudo ./b2 install --build-type=complete --layout=tagged

#### error when running server<br>
###### /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.15' not found
* strings /usr/lib64/libstdc++.so.6 | grep GLIBC   //  check the GLIBCXX_3.4.15. It is not in it.
* sudo find / -name "libstdc++.so*" 2>/dev/null  // find the latest libstdc++.so   It should be /usr/local/lib64/libstdc++.so.6.0.18
* sudo rm /usr/lib64/libstdc++.so.6
* sudo cp /usr/local/lib64/libstdc++.so.6.0.18 /usr/lib64/
* sudo ln -s /usr/lib64/libstdc++.so.6.0.18 /usr/lib64/libstdc++.so.6
* sudo vim /etc/ld.so.conf   // add a new line: /usr/lib64
* sudo ldconfig // all ok



### OSX install help
#### gcc 4.8
* install port if do not have one:  https://www.macports.org/install.php
* port search gcc   # show gcc version
* sudo /opt/local/bin/port install gcc48
* sudo /opt/local/bin/port select --set gcc mp-gcc48
* last step,  close old shell, open a new shell.
###### https://apple.stackexchange.com/questions/118830/install-gcc-to-mac-via-terminal
###### http://www.tuicool.com/articles/reqiY3

#### boost 1.58
* https://stackoverflow.com/questions/104322/how-do-you-install-boost-on-macos


