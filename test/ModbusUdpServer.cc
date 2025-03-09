#include <stdio.h>
#include <stdlib.h>
#include <modbus.h>
#include <pthread.h>
#include <unistd.h> // for sleep function
#include <time.h>   // for random number generation
#include <errno.h>

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
    int rc;

    // ����һ���µ�Modbus UDP������
    ctx = modbus_new_udp("0.0.0.0", 1502);
    if (ctx == NULL)
    {
        fprintf(stderr, "Unable to allocate libmodbus context\n");
        return -1;
    }

    // �󶨷�����
    if (modbus_udp_bind(ctx) == -1)
    {
        fprintf(stderr, "Unable to bind UDP socket: %s\n", modbus_strerror(errno));
        modbus_free(ctx);
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

    // ������̨�߳�
    pthread_t thread;
    pthread_create(&thread, NULL, update_registers, NULL);

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
    modbus_mapping_free(mb_mapping);
    modbus_free(ctx);

    return 0;
}