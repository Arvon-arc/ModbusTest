#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <errno.h>
#include <unistd.h> // for sleep function

int main(void)
{
    modbus_t *ctx;
    uint16_t tab_reg[32];
    uint16_t value = 0; // ��ʼֵ
    int rc;
    int i;

    // ����һ���µ�Modbus TCP������
    ctx = modbus_new_tcp("172.16.96.88", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    // ���ӵ�������
    if (modbus_connect(ctx) == -1)
    {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    while (1)
    {
        // ����д�뱣�ּĴ���3
        rc = modbus_write_register(ctx, 4, value);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to write register: %s\n", modbus_strerror(errno));
            modbus_close(ctx);
            modbus_free(ctx);
            return -1;
        }

        // ��ȡ���ּĴ���
        rc = modbus_read_registers(ctx, 0, 10, tab_reg);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to read registers: %s\n", modbus_strerror(errno));
            modbus_close(ctx);
            modbus_free(ctx);
            return -1;
        }

        // ��ӡ��ȡ���ļĴ���ֵ��С�˸�ʽ��
        for (i = 0; i < rc; i++)
        {
            uint16_t value_le = tab_reg[i];
            uint16_t value_be = (value_le >> 8) | (value_le << 8); // ת��ΪС�˸�ʽ
            printf("reg[%d]=%d (LE: %d)\n", i, value_be, value_le);
        }

        // ����ֵ
        value++;

        // ÿ3��д��һ��
        sleep(3);
    }

    // �ر�����
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}