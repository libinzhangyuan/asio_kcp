OLD_PWD="$( pwd )"

cd ./asio_kcp_client/ && make clean && \
    cd ../asio_kcp_server/ && make clean && \
    cd ../essential/ && make clean
cd ../

# restore old path.
cd $OLD_PWD
