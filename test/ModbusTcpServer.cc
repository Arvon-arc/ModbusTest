#include <stdio.h>
#include <stdlib.h>
#include "../lib/modbus/modbus-tcp.h"
#include <pthread.h>
#include <errno.h>
#include <unistd.h> // for sleep function
#include <time.h>   // for random number generation

// 全局变量
uint16_t holding_registers[10];

// 后台线程函数，用于更新保持寄存器的值
void *update_registers(void *arg)
{
    srand(time(NULL)); // 初始化随机数生成器
    while (1)
    {
        for (int i = 0; i < 10; ++i)
        {
            holding_registers[i] = rand() % 100 + 1; // 生成1到100之间的随机数
        }
        sleep(1); // 每秒更新一次
    }
    return NULL;
}

int main(void)
{
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int s = -1;
    int rc;

    // 创建一个新的Modbus TCP上下文
    ctx = modbus_new_tcp("0.0.0.0", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    // 创建一个新的Modbus映射
    mb_mapping = modbus_mapping_new(0, 0, 100, 100);
    if (mb_mapping == NULL)
    {
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    // 绑定服务器
    s = modbus_tcp_listen(ctx, 1);
    if (s == -1)
    {
        fprintf(stderr, "Unable to listen TCP connection: %s\n", modbus_strerror(errno));
        modbus_mapping_free(mb_mapping);
        modbus_free(ctx);
        return -1;
    }

    // 启动后台线程
    pthread_t thread;
    pthread_create(&thread, NULL, update_registers, NULL);

    // 接受客户端连接
    modbus_tcp_accept(ctx, &s);

    for (;;)
    {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        // 将全局变量的值复制到保持寄存器
        for (int i = 0; i < 10; ++i)
        {
            mb_mapping->tab_registers[i] = holding_registers[i];
        }

        // 等待请求
        rc = modbus_receive(ctx, query);
        if (rc == -1)
        {
            fprintf(stderr, "Connection closed: %s\n", modbus_strerror(errno));
            break;
        }

        // 处理请求
        rc = modbus_reply(ctx, query, rc, mb_mapping);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to send reply: %s\n", modbus_strerror(errno));
            break;
        }
    }

    // 释放资源
    close(s);
    modbus_mapping_free(mb_mapping);
    modbus_free(ctx);

    return 0;
}