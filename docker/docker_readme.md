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
# 1. prepare (out of the docker container)
cd /home/zhangyuan/work/asio_kcp/third_party/
unzip g2log.zip && unzip gmock-1.7.0.zip && unzip gtest-1.7.0.zip && unzip muduo.zip
cp ../docker/LogStream.h ./muduo/muduo/base/LogStream.h
cp ../docker/CMakeLists.txt ./muduo/CMakeLists.txt

# 2. run or start the container
sudo docker run -it --name asio_kcp -p 12345:12345/udp -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
sudo docker start -ia asio_kcp

# 3. build in docker
cd /home/work/asio_kcp/third_party && sh build.sh
cd /home/work/asio_kcp
sh quick_make.sh  # or  sh allmake.sh
```


###Run example test
##### run server in container
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
##### run test client in anothor container
    sudo docker run -it --name asio_kcp_client -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop
    sh quick_make.sh
    ./client_with_asio/client_with_asio 2342x 127.0.0.1 12345 500 2>/dev/null





# Run the benchmark test
### do at server
```
docker build at master branch
sudo docker build -t asio_kcp:develop .
sudo docker run -it --name asio_kcp_server -p 12345:12345/udp -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop

compile and run at kcp_bench_mark_test branch
git checkout kcp_bench_mark_test
compile
then run server on your server:
    cd /home/work/asio_kcp
    ./server/server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
```
### do at client
```
docker build at master branch
sudo docker build -t asio_kcp:develop .
sudo docker run -it --name asio_kcp_client -v /home/zhangyuan/work/asio_kcp:/home/work/asio_kcp asio_kcp:develop

compile and run at kcp_bench_mark_test branch
git checkout kcp_bench_mark_test
if you want to test the 3G/4G. you can share the wifi on your phone by using wiless AP. Making your client computer connect to this wifi.
run client on your client computer (Note: changing the ip and port to your server)
  cd /home/work/asio_kcp
  ./client_with_asio/client_with_asio 2344x 192.0.x.x 12345 500 2>/dev/null
       Note: changing the ip and port to your server which is running the asio_kcp_server
```
