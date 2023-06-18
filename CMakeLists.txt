cmake_minimum_required(VERSION 3.25)
project(homework)

set(CMAKE_CXX_STANDARD 14)

add_executable(homework
        20230410/BaiTap1/tcp_client.c
        20230410/BaiTap2/tcp_server.c
        20230410/BaiTap3/sv_client.c
        20230410/BaiTap4/sv_server.c
        20230417/BaiTap01.01/info_client.c
        20230417/BaiTap01.01/info_server.c
        20230417/BaiTap01.02/client.c
        20230417/BaiTap01.02/server.c
        20230417/BaiTap01.03/udp_file_receiver.c
        20230417/BaiTap01.03/udp_file_sender.c
        practice/non_blocking/non_blocking.c 20230522/chat_client.c 20230522/chat_server.c)
