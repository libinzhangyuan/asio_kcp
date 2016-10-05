FROM centos:6
MAINTAINER zhangyuan <libinzhangyuan@gmail.com>


RUN yum -y update && yum clean all
RUN yum install -y git
RUN yum -y install wget
RUN yum -y groupinstall "Development Tools"
RUN yum -y install cmake
RUN yum -y install bzip2-devel
RUN yum -y install python-devel


# vim
#
RUN yum install -y vim-enhanced.x86_64
ADD .vimrc /root/
RUN git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim
RUN vim +PluginInstall +qall
ADD .vimrc2 /root/.vimrc

RUN mkdir /home/install

# gcc
#
WORKDIR /home/install
ADD gcc-4.8.0.tar.bz2 /home/install/
#RUN wget http://ftp.gnu.org/gnu/gcc/gcc-4.8.0/gcc-4.8.0.tar.bz2
#RUN tar -jxvf gcc-4.8.0.tar.bz2
WORKDIR /home/install/gcc-4.8.0
RUN ./contrib/download_prerequisites
RUN mkdir ../gcc-build-4.8.0
WORKDIR /home/install/gcc-build-4.8.0
RUN ../gcc-4.8.0/configure --enable-checking=release --enable-languages=c,c++ --disable-multilib
RUN make -j4
RUN make install
RUN rm -Rf /home/install/gcc-build-4.8.0

RUN rm -f /usr/lib64/libstdc++.so.6
RUN cp /usr/local/lib64/libstdc++.so.6.0.18 /usr/lib64/
RUN ln -s /usr/lib64/libstdc++.so.6.0.18 /usr/lib64/libstdc++.so.6
RUN ldconfig -v



# boost
#
WORKDIR /home/install
ADD boost_1_58_0.tar.bz2 /home/install/
WORKDIR /home/install/boost_1_58_0
RUN ./bootstrap.sh
RUN ./b2 install --build-type=complete --layout=tagged --without-graph --without-graph_parallel --without-math --without-mpi --without-python --without-serialization --without-wave
RUN rm -Rf /home/install/boost_1_58_0


# asio_kcp
#
WORKDIR /home/install
RUN wget -O asio_kcp.zip https://codeload.github.com/libinzhangyuan/asio_kcp/zip/master
RUN unzip asio_kcp.zip && mv asio_kcp-master asio_kcp

WORKDIR /home/install/asio_kcp/third_party
RUN unzip g2log.zip
WORKDIR /home/install/asio_kcp/third_party/g2log
RUN mkdir build && cd build && cmake .. && make

WORKDIR /home/install/asio_kcp/third_party
RUN unzip muduo.zip
ADD LogStream.h /home/install/asio_kcp/third_party/muduo/muduo/base/LogStream.h
ADD CMakeLists.txt /home/install/asio_kcp/third_party/muduo/CMakeLists.txt
WORKDIR /home/install/asio_kcp/third_party/muduo
RUN CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . ./build.sh

RUN rm -Rf /home/install/asio_kcp
RUN rm -Rf /home/install

RUN mkdir /var/log/asio_kcp_log

WORKDIR /home/work/asio_kcp
