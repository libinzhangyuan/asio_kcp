FROM centos:6
MAINTAINER zhangyuan <libinzhangyuan@gmail.com>


RUN yum -y update && yum clean all &&\
    yum install -y git &&\
    yum -y install wget &&\
    yum -y groupinstall "Development Tools" &&\
    yum -y install cmake &&\
    yum -y install bzip2-devel&&\
    yum -y install python-devel


# vim
#
ADD .vimrc /root/
RUN yum install -y vim-enhanced.x86_64 &&\
    git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim &&\
    vim +PluginInstall +qall
ADD .vimrc2 /root/.vimrc



RUN mkdir /home/install

# gcc
#
ADD gcc-4.8.0.tar.bz2 /home/install/
RUN cd /home/install/gcc-4.8.0 &&\
    ./contrib/download_prerequisites &&\
    mkdir ../gcc-build-4.8.0 &&\
    cd /home/install/gcc-build-4.8.0 &&\
    ../gcc-4.8.0/configure --enable-checking=release --enable-languages=c,c++ --disable-multilib &&\
    make -j4 && make install &&\
    rm -Rf /home/install/gcc-build-4.8.0 &&\
    rm -f /usr/lib64/libstdc++.so.6 &&\
    cp /usr/local/lib64/libstdc++.so.6.0.18 /usr/lib64/ &&\
    ln -s /usr/lib64/libstdc++.so.6.0.18 /usr/lib64/libstdc++.so.6 &&\
    ldconfig -v


# boost
#
ADD boost_1_58_0.tar.bz2 /home/install/
RUN cd /home/install/boost_1_58_0 &&\
    ./bootstrap.sh &&\
    ./b2 install --build-type=complete --layout=tagged --without-graph --without-graph_parallel --without-math --without-mpi --without-python --without-serialization --without-wave &&\
    rm -Rf /home/install/boost_1_58_0


# asio_kcp
#
RUN cd /home/install &&\
    wget -O asio_kcp.zip https://codeload.github.com/libinzhangyuan/asio_kcp/zip/master &&\
    unzip asio_kcp.zip && mv asio_kcp-master asio_kcp

RUN cd /home/install/asio_kcp/third_party &&\
    unzip g2log.zip &&\
    cd /home/install/asio_kcp/third_party/g2log &&\
    mkdir build && cd build &&\
    cmake .. && make

RUN cd /home/install/asio_kcp/third_party &&\
    unzip muduo.zip
ADD LogStream.h /home/install/asio_kcp/third_party/muduo/muduo/base/LogStream.h
ADD CMakeLists.txt /home/install/asio_kcp/third_party/muduo/CMakeLists.txt
RUN cd /home/install/asio_kcp/third_party/muduo &&\
    CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . ./build.sh

RUN rm -Rf /home/install/asio_kcp && rm -Rf /home/install

RUN mkdir /var/log/asio_kcp_log

WORKDIR /home/work/asio_kcp
