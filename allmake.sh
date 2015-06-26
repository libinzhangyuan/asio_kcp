OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH

`rm asio_kcp_client/asio_kcp_client 2>/dev/null ; rm asio_kcp_server/asio_kcp_server 2>/dev/null`

echo "" && echo "" && echo "[-------------------------------]" && echo "   essential" && echo "[-------------------------------]" && \
    cd ./essential/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_client" && echo "[-------------------------------]" && \
    cd ../asio_kcp_client/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_server" && echo "[-------------------------------]" && \
    cd ../asio_kcp_server/ && make

# restore old path.
cd $OLD_PWD
