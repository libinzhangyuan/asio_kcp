#include <unistd.h>
#include <iostream>
#include <strings.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>

#include "gtest/gtest.h"

std::string get_server_full_path_name(void)
{
    char current_absolute_path[1024 * 10] = "";
    //获取当前目录绝对路径
    if (NULL == realpath("../", current_absolute_path))
    {
        printf("***Error***");
        exit(-1);
    }
    strcat(current_absolute_path, "/");
    printf("current absolute path:%s\n", current_absolute_path);

    return std::string(current_absolute_path) + "asio_kcp/server/server";
}

int main(int argc, char* argv[])
{
    // ignore the child terminate signal
    signal(SIGCHLD, SIG_IGN);

    const std::string& server_full_name = get_server_full_path_name();
    pid_t child_pid = 0;
    // fork a child process for create a server.
    pid_t pid = fork();
    if (pid < 0)
    {
        std::cout << "fork return error :" << pid << std::endl;
    }
    if (pid == 0) // child exec server
    {
        std::cout << "exec server in fork progress server_full_name: " << server_full_name << std::endl;
        int fd = open("/dev/null", O_WRONLY);
        dup2(fd, 1);
        dup2(fd, 2);
        int ret_exec = execl(server_full_name.c_str(), "server", "0.0.0.0", "32323", NULL);
        if (ret_exec < 0)
            std::cerr << "execl error with errno: " << errno << " " << strerror(errno) << std::endl;
        return 0;
    }


    // parent
    //
    child_pid = pid;

    // waiting server start
    sleep(2);

    testing::InitGoogleTest(&argc, argv);
    int ret = RUN_ALL_TESTS();
    std::cerr << "RUN_ALL_TESTS return: " << ret << std::endl;


    sleep(2);

    // send signal to stop server.
    //
    int ret_kill = ::kill(child_pid, SIGINT);
    if (ret_kill < 0)
        std::cerr << "kill error with errno: " << errno << " " << strerror(errno) << std::endl;

    sleep(1);

    return 0;
}
