OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH
`rm -f client_with_asio/client_with_asio 2>/dev/null ;\
    rm -f server/server 2>/dev/null ;\
    rm -f server_lib/asio_kcp_server.a 2>/dev/null;\
    rm -f asio_kcp_utest/asio_kcp_utest 2>/dev/null;\
    rm -f asio_kcp_client_utest/asio_kcp_client_utest 2>/dev/null;\
`

echo "" && echo "" && echo "[-------------------------------]" && echo "   essential" && echo "[-------------------------------]" && \
    cd ./essential/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   client_lib" && echo "[-------------------------------]" && \
    cd ../client_lib/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   server_lib" && echo "[-------------------------------]" && \
    cd ../server_lib/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   server" && echo "[-------------------------------]" && \
    cd ../server/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   client_with_asio" && echo "[-------------------------------]" && \
    cd ../client_with_asio/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_utest" && echo "[-------------------------------]" && \
    cd ../asio_kcp_utest/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   kcp_client_utest" && echo "[-------------------------------]" && \
    cd ../asio_kcp_client_utest/ && make && \
echo ""

# restore old path.
cd $OLD_PWD
