OLD_PWD="$( pwd )"

BOOST_LIB_PATH=/usr/local/lib
export BOOST_LIB_PATH
BOOST_INC_PATH=/usr/local/include
export BOOST_INC_PATH

`rm client_with_asio/client_with_asio 2>/dev/null ; rm server/server 2>/dev/null ; rm asio_kcp.a/asio_kcp.a 2>/dev/null`

echo "" && echo "" && echo "[-------------------------------]" && echo "   essential" && echo "[-------------------------------]" && \
    cd ./essential/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp" && echo "[-------------------------------]" && \
    cd ../server_lib/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   server" && echo "[-------------------------------]" && \
    cd ../server/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   client_with_asio" && echo "[-------------------------------]" && \
    cd ../client_with_asio/ && make && \
echo "" && echo "" && echo "[-------------------------------]" && echo "   asio_kcp_utest" && echo "[-------------------------------]" && \
    cd ../asio_kcp_utest/ && make && \
echo ""

# restore old path.
cd $OLD_PWD
