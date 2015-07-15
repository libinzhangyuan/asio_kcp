OLD_PWD="$( pwd )"

cd ./client_with_asio/ && make clean && \
    cd ../server/ && make clean && \
    cd ../server_lib/ && make clean && \
    cd ../asio_kcp_utest/ && make clean && \
    cd ../essential/ && make clean
cd ../

# restore old path.
cd $OLD_PWD
