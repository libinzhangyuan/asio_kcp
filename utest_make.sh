OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH

`rm server_lib/asio_kcp_server.a 2>/dev/null;\
    rm asio_kcp_utest/asio_kcp_utest 2>/dev/null;\
`

echo "" && echo "" && echo "[-------------------------------]" && echo "   essential" && echo "[-------------------------------]" && \
    cd ./essential/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp" && echo "[-------------------------------]" && \
    cd ../server_lib/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_utest" && echo "[-------------------------------]" && \
    cd ../asio_kcp_utest/ && make && \
echo "" && \
    ./asio_kcp_utest

# restore old path.
cd $OLD_PWD
