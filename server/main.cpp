#include <iostream>
#include <string>
#include <stdlib.h>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "../server_lib/server.hpp"
#include "g2logworker.h"
#include "g2log.h"
#include "asio_kcp_log.hpp"


#include <muduo/base/Logging.h>
#include <muduo/base/LogFile.h>
#include <muduo/base/ThreadPool.h>
#include <muduo/base/TimeZone.h>
#include <stdio.h>
int g_total = 0;
boost::scoped_ptr<muduo::LogFile> g_logFile;


void dummyOutput(const char* msg, int len)
{
    g_total += len;
    if (g_logFile)
        g_logFile->append(msg, len);
}

int main(int argc, char* argv[])
{
  try
  {
    // Check command line arguments.
    if (argc != 3)
    {
      std::cerr << "Usage: server <address> <port>\n";
      std::cerr << "  For IPv4, try:\n";
      std::cerr << "    server 0.0.0.0 80\n";
      std::cerr << "  For IPv6, try:\n";
      std::cerr << "    server 0::0 80\n";
      return 1;
    }

    //system("mkdir /var/log/asio_kcp");

    // g2log
    //
    std::string path_to_log_file("/var/log/asio_kcp_log/");
    g2LogWorker logger(argv[0], path_to_log_file);
    g2::initializeLogging(&logger);
    AK_LOG(INFO) << "Start";

    // muduo log
    //
    std::string basename = "/var/log/asio_kcp_log/ak_muduo_log";
    size_t rollSize = 500*1000*1000;
    bool threadSafe = true;
    int flushInterval = 3; // seconds
    int checkEveryN = 1024;
    g_logFile.reset(new ::muduo::LogFile(basename, rollSize, threadSafe, flushInterval, checkEveryN));
    muduo::Logger::setOutput(dummyOutput);
    LOG_INFO << "start";


    // Initialise the server.
    kcp_svr::server s(argv[1], argv[2]);

    // Run the server until stopped.
    s.run();
  }
  catch (std::exception& e)
  {
    std::cerr << "exception: " << e.what() << "\n";
  }

  return 0;
}
