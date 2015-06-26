OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH

echo "" && echo "" && echo "[-------------------------------]" && echo "   essential" && echo "[-------------------------------]" && \
    cd ./essential/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_client" && echo "[-------------------------------]" && \
    cd ../asio_kcp_client/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_server" && echo "[-------------------------------]" && \
    cd ../asio_kcp_server/ && make

# restore old path.
cd $OLD_PWD
