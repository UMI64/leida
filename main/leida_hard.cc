#include "leida_hard.hpp"
leida_hard::leida::leida(leida_hard *leida_hard_obj)
{
    target_buffer_lock = new thread_mutex_lock;
    this->leida_hard_obj = leida_hard_obj;
    power(true);
}
leida_hard::leida::~leida()
{
    power(false);
    delete target_buffer_lock;
}
void leida_hard::leida::power(bool sw)
{
    gpio_set_level(this->leida_hard_obj->leida_power_pin, !sw);
    if (sw)
    {
        target_num = 0;
        memset(targets_buffer, 0, 3 * sizeof(leida_target_t));
        read_thread = new thread_helper(std::bind(&leida::read_task, this, std::placeholders::_1));
    }
    else
    {
        delete read_thread;
        target_num = 0;
        memset(targets_buffer, 0, 3 * sizeof(leida_target_t));
    }
}
uint8_t leida_hard::leida::get_targets(leida_target_t *targets, uint8_t target_num)
{
    uint8_t __target_num;
    target_buffer_lock->lock();
    memcpy(targets, targets_buffer, (target_num > 3 ? 3 : target_num) * sizeof(leida_target_t));
    __target_num = target_num;
    target_buffer_lock->unlock();
    return __target_num;
}
void leida_hard::leida::read_task(void *param)
{
    uint8_t temp_buffer[32];
    uint8_t head_magic[4] = {0xAA, 0xFF, 0x03, 0x00};
    uint8_t temp_targets_buffer[3 * sizeof(leida_target_t)];
    uint8_t targets_buffer_used = 0;
    uint8_t recv_status = 0;
    while (true)
    {
        int length;
        length = uart_read_bytes(UART_NUM_1, temp_buffer, 32, 10);
        if (length)
        {
            uint8_t length_lused = 0;
            while (length_lused < length)
            {
                if (recv_status < sizeof(head_magic))
                {
                    if (temp_buffer[length_lused] == head_magic[recv_status])
                        recv_status++;
                    else
                        recv_status = 0;
                    length_lused++;
                    continue;
                }
                if (recv_status < sizeof(head_magic) + 3 * sizeof(leida_target_t))
                {
                    uint8_t target_length = length - length_lused;
                    uint8_t max_target_length = sizeof(temp_targets_buffer) - targets_buffer_used;
                    target_length = target_length > max_target_length ? max_target_length : target_length;
                    memcpy(temp_targets_buffer + targets_buffer_used, temp_buffer + length_lused, target_length);
                    length_lused += target_length;
                    recv_status += target_length;
                    targets_buffer_used += target_length;
                }
                if (recv_status >= sizeof(head_magic) + 3 * sizeof(leida_target_t))
                {
                    target_buffer_lock->lock();
                    target_num = 0;
                    memcpy(targets_buffer, temp_targets_buffer, sizeof(temp_targets_buffer));
                    for (int i = 0; i < 3; i++)
                    {
                        if (targets_buffer[i].xpos_mm != 0 && targets_buffer[i].ypos_mm != 0)
                        {
                            targets_buffer[i].xpos_mm = targets_buffer[i].xpos_mm & 0x8000 ? targets_buffer[i].xpos_mm & ~0x8000 : -(targets_buffer[i].xpos_mm & ~0x8000);
                            targets_buffer[i].ypos_mm = targets_buffer[i].ypos_mm & 0x8000 ? targets_buffer[i].ypos_mm & ~0x8000 : -(targets_buffer[i].ypos_mm & ~0x8000);
                            targets_buffer[i].speed_cms = targets_buffer[i].speed_cms & 0x8000 ? targets_buffer[i].speed_cms & ~0x8000 : -(targets_buffer[i].speed_cms & ~0x8000);
                            target_num++;
                        }
                    }
                    target_buffer_lock->unlock();
                    recv_status = 0;
                    targets_buffer_used = 0;
                }
            }
        }
    }
}
