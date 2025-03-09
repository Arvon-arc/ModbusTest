#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>
#include <unistd.h> // for sleep function

int main(void)
{
    modbus_t *ctx;
    uint16_t tab_reg[32];
    uint16_t value = 0; // 初始值
    int rc;
    int i;

    // 创建一个新的Modbus TCP上下文
    ctx = modbus_new_tcp("172.16.96.88", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    // 连接到服务器
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    while (1)
    {
        // 递增写入保持寄存器3
        rc = modbus_write_register(ctx, 4, value);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to write register: %s\n", modbus_strerror(errno));
            modbus_close(ctx);
            modbus_free(ctx);
            return -1;
        }

        // 读取保持寄存器
        rc = modbus_read_registers(ctx, 0, 10, tab_reg);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to read registers: %s\n", modbus_strerror(errno));
            modbus_close(ctx);
            modbus_free(ctx);
            return -1;
        }

        // 打印读取到的寄存器值（小端格式）
        for (i = 0; i < rc; i++)
        {
            uint16_t value_le = tab_reg[i];
            uint16_t value_be = (value_le >> 8) | (value_le << 8); // 转换为小端格式
            printf("reg[%d]=%d (LE: %d)\n", i, value_be, value_le);
        }

        // 递增值
        value++;

        // 每3秒写入一次
        sleep(3);
    }

    // 关闭连接
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}