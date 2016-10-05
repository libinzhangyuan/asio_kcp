How to build asio_kcp in docker container. 

# Start the docker.
## install docker
 https://www.docker.com/products/overview

## prepare if you are in china. For GFW.
```
vim /etc/default/docker
  DOCKER_OPTS="--dns 114.114.114.114 --dns 8.8.8.8 --dns 8.8.4.4"
```

## download boost_1_58_0.tar.bz2 && gcc-4.8.0.tar.bz2 to folder asio_kcp/docker
```
cd asio_kcp/docker
wget http://sourceforge.net/projects/boost/files/boost/1.58.0/boost_1_58_0.tar.bz2
wget http://ftp.gnu.org/gnu/gcc/gcc-4.8.0/gcc-4.8.0.tar.bz2
```

## create images using DockerfileSS
```
cd asio_kcp/docker
sudo docker build -t asio_kcp:develop .
```

## create container. please change the source code path in command. (the /home/zhangyuan/work/asio_kcp part)
```
sudo docker run -it --name asio_kcp -p 12345:12345/udp -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
  exit
```


<br><br><br>


# Compile in asio_kcp docker container
* how
```
share the source code folder to container. Then compile the shared code foler in container. <br>
changing the coding outside the container. And compiling or testing the code in container.
```
* command
```
sudo docker run -it --name asio_kcp -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
cd /home/work/asio_kcp/third_party/g2log
mkdir build ; cd build && cmake .. && make
cd /home/work/asio_kcp/third_party/muduo
CC=gcc CXX=g++ BUILD_DIR=./build BUILD_TYPE=release BUILD_NO_EXAMPLES=1 . ./build.sh
cd /home/work/asio_kcp
sh quick_make.sh  # or  sh allmake.sh
```


###Run example test
##### filter the verbose log from asio timer
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
##### filter all asio log  (in anothor container)
    sudo docker run -it --name asio_kcp_client --net=container:asio_kcp_server -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
    ./client_with_asio/client_with_asio 23425 127.0.0.1 12345 500 2>/dev/null








# Run the benchmark test
### run server
```
git checkout kcp_bench_mark_test
sudo docker run -it --name asio_kcp_server -p 12345:12345/udp -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
compile
then run server on your server:
    cd /home/work/asio_kcp
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
```
### run client
```
git checkout kcp_bench_mark_test
sudo docker run -it --name asio_kcp_client --net=container:asio_kcp_server -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
if you want to test the 3G/4G. you can share the wifi on your phone by using wiless AP. Making your client computer connect to this wifi.
run client on your client computer (Note: changing the ip and port to your server)
  cd /home/work/asio_kcp
  ./client_with_asio/client_with_asio 23445 127.0.0.1 12345 500 2>/dev/null
       Note: changing the ip and port to your server which is running the asio_kcp_server
```
