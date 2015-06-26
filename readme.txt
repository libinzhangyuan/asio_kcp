centos 6.5
1. 先安装 gcc 4.8  : http://blog.csdn.net/dyllove98/article/details/8917485

2. 安装boost 1.58    http://www.boost.org/doc/libs/1_58_0/more/getting_started/unix-variants.html
  scp ~/work/code/boost_1_58_0.tar.bz2 star@g1:/data/
  ssh star@g1
  cd /data
  tar jxvf boost_1_58_0.tar.bz2
  cd boost_1_58_0
  ./bootstrap.sh
  sudo ./b2 install --build-type=complete --layout=tagged

3. ./bserver无法运行 报错 /usr/lib64/libstdc++.so.6: version `GLIBCXX_3.4.15' not found (required by ./bserver)
   -- http://www.169it.com/article/7300696743188626901.html
   strings /usr/lib64/libstdc++.so.6 | grep GLIBC   //  可以查看GLIBCXX_3.4.15不在里面
   sudo find / -name "libstdc++.so*" 2>/dev/null  // 查找到最新版本的libstdc++.so   /usr/local/lib64/libstdc++.so.6.0.18
   sudo rm /usr/lib64/libstdc++.so.6
   sudo ln -s /usr/local/lib64/libstdc++.so.6.0.18 /usr/lib64/libstdc++.so.6
   sudo vim /etc/ld.so.conf   // 将/usr/lib64目录新加一行，存入.
   sudo ldconfig // all ok

osx 10.10 部署:

1. 安装 gcc 4.8
  安装port如果没有:  https://www.macports.org/install.php
  port search gcc   # 显示可安装的gcc版本号
  sudo /opt/local/bin/port install gcc48
  sudo /opt/local/bin/port select --set gcc mp-gcc48
  最后一步，重新开一个窗口
      方法来源: https://apple.stackexchange.com/questions/118830/install-gcc-to-mac-via-terminal
                http://www.tuicool.com/articles/reqiY3

/opt/local/bin/gcc-mp-4.8

2. boost 1.58  https://stackoverflow.com/questions/104322/how-do-you-install-boost-on-macos
  见centos中的安装步骤



osx 额外的备注:
1. gdb安装
brew install https://raw.github.com/Homebrew/homebrew-dupes/master/gdb.rb

3. ddd安装
https://www.gnu.org/software/ddd/
or
brew install Caskroom/cask/xquartz
brew install ddd
导入证书:  http://blog.plotcup.com/a/129

4. zhangyuan机器，两种gcc切换. -- 如果需要编译fly, 或者需要编译ruby 的某个gem  时，需要切换gcc版本
sudo rm /opt/local/bin/gcc
sudo ln -s /opt/local/bin/gcc_48_for_fly /opt/local/bin/gcc
sudo rm /opt/local/bin/g++
sudo ln -s /opt/local/bin/g++_48_for_fly /opt/local/bin/g++

5. 调试命令
 a. 打日志, 过滤掉timer的日志
    ./asio_kcp_server/asio_kcp_server 0.0.0.0 12345 2>&1 | grep --line-buffered -v -e deadline_timer -e "ec=system:0$" -e "|$" >>bserver.txt
