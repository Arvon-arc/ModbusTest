#include <stdio.h>
#include <stdlib.h>
#include "../lib/modbus/modbus-tcp.h"
#include <pthread.h>
#include <errno.h>
#include <unistd.h> // for sleep function
#include <time.h>   // for random number generation

// ȫ�ֱ���
uint16_t holding_registers[10];

// ��̨�̺߳��������ڸ��±��ּĴ�����ֵ
void *update_registers(void *arg)
{
    srand(time(NULL)); // ��ʼ�������������
    while (1)
    {
        for (int i = 0; i < 10; ++i)
        {
            holding_registers[i] = rand() % 100 + 1; // ����1��100֮��������
        }
        sleep(1); // ÿ�����һ��
    }
    return NULL;
}

int main(void)
{
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    int s = -1;
    int rc;

    // ����һ���µ�Modbus TCP������
    ctx = modbus_new_tcp("0.0.0.0", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    // ����һ���µ�Modbusӳ��
    mb_mapping = modbus_mapping_new(0, 0, 100, 100);
    if (mb_mapping == NULL)
    {
        fprintf(stderr, "Failed to allocate the mapping: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    // �󶨷�����
    s = modbus_tcp_listen(ctx, 1);
    if (s == -1)
    {
        fprintf(stderr, "Unable to listen TCP connection: %s\n", modbus_strerror(errno));
        modbus_mapping_free(mb_mapping);
        modbus_free(ctx);
        return -1;
    }

    // ������̨�߳�
    pthread_t thread;
    pthread_create(&thread, NULL, update_registers, NULL);

    // ���ܿͻ�������
    modbus_tcp_accept(ctx, &s);

    for (;;)
    {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

        // ��ȫ�ֱ�����ֵ���Ƶ����ּĴ���
        for (int i = 0; i < 10; ++i)
        {
            mb_mapping->tab_registers[i] = holding_registers[i];
        }

        // �ȴ�����
        rc = modbus_receive(ctx, query);
        if (rc == -1)
        {
            fprintf(stderr, "Connection closed: %s\n", modbus_strerror(errno));
            break;
        }

        // ��������
        rc = modbus_reply(ctx, query, rc, mb_mapping);
        if (rc == -1)
        {
            fprintf(stderr, "Failed to send reply: %s\n", modbus_strerror(errno));
            break;
        }
    }

    // �ͷ���Դ
    close(s);
    modbus_mapping_free(mb_mapping);
    modbus_free(ctx);

    return 0;
}