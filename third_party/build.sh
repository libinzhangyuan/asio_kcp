cd /home/work/asio_kcp/third_party/g2log && mkdir build ; cd build && cmake .. && make
cd /home/work/asio_kcp/third_party/gmock-1.7.0 && ./configure && make

cd /home/work/asio_kcp/third_party/muduo
CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . ./build.sh
cd /home/work/asio_kcp/third_party/gtest-1.7.0 && ./configure && make
cd /home/work/asio_kcp/third_party/gmock-1.7.0 && ./configure && make
cd /home/work/asio_kcp
