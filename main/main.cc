#include "leida.hpp"
extern "C"
{
    void app_main(void)
    {
        leida leida_obj;
        while (1)
        {
            vTaskDelay(pdMS_TO_TICKS(1000));
            // ESP_LOGI("BED", "tempture: %fC", get_bed_tempture_digrees());
            // ESP_LOGI("BED", "current: %fA", get_bed_current_a());
            // gpio_set(3, cnt % 2);
            // for (uint8_t addr = 0; addr < 10; addr++)
            // ESP_LOGI("HUSB238", "addr%x: %x", addr, husb238_read(addr));
            // lv_timer_handler();
            // cnt++;
        }
    }
}