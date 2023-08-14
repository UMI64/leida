#include "leida.hpp"
leida::leida()
{
    leida_hard_obj = new leida_hard;
    wifi_obj = new leida_wifi;
    web_obj = new leida_web;
    main_thread = new thread_helper(std::bind(&leida::main_thread_task, this, std::placeholders::_1));
}
leida::~leida()
{
    delete main_thread;
    delete web_obj;
    delete wifi_obj;
    delete leida_hard_obj;
}
void leida::main_thread_task(void *param)
{
    leida_hard::leida::leida_target_t targets[3];
    leida_hard_obj->led(true);
    leida_hard_obj->leida_obj->power(true);
    ESP_LOGI("leida", "on");
    while (true)
    {
        thread_helper::sleep(400);
        leida_hard_obj->leida_obj->get_targets(targets, 3);
        // ESP_LOGI("leida", "T1 x:[%d] y:[%d] speed:[%d] reslusion:[%u]", targets[0].xpos_mm, targets[0].ypos_mm, targets[0].speed_cms, targets[0].reslusion_mm);
        // ESP_LOGI("leida", "T2 x:[%d] y:[%d] speed:[%d] reslusion:[%u]", targets[1].xpos_mm, targets[1].ypos_mm, targets[1].speed_cms, targets[1].reslusion_mm);
        // ESP_LOGI("leida", "T3 x:[%d] y:[%d] speed:[%d] reslusion:[%u]", targets[2].xpos_mm, targets[2].ypos_mm, targets[2].speed_cms, targets[2].reslusion_mm);
    }
}