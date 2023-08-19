#pragma once
#include <functional>
#include <string>
#include <map>
#include <vector>
#include <string.h>
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/uart.h"

typedef enum leida_err_e
{
    leida_success = 0,
    leida_faile = -1,
    leida_faile_timeout = 0x107
} leida_err_e;

class leida_err
{
public:
    static std::string to_string(leida_err_e err)
    {
        switch (err)
        {
        case leida_success:
            return "leida_success";
        case leida_faile:
            return "leida_faile";
        case leida_faile_timeout:
            return "leida_faile_timeout";
        default:
            return "leida_undefine";
        }
    }
};

class thread_helper
{
private:
    TaskHandle_t pxCreatedTask;
    std::function<void(void *)> task_caller;
    void *task_param;
    static void caller(void *param)
    {
        ((thread_helper *)param)->task_caller(((thread_helper *)param)->task_param);
    }

public:
    thread_helper(std::function<void(void *)> caller, void *param = nullptr, std::string name = "", uint32_t stack_depth = 4096, uint8_t prio = 1)
    {
        task_caller = caller;
        task_param = param;
        xTaskCreate(this->caller, name.c_str(), stack_depth, this, prio, &pxCreatedTask);
    }
    thread_helper(std::function<void(void *)> caller, uint32_t stack_depth, void *param = nullptr, std::string name = "", uint8_t prio = 1)
    {
        task_caller = caller;
        task_param = param;
        xTaskCreate(this->caller, name.c_str(), stack_depth, this, prio, &pxCreatedTask);
    }
    ~thread_helper()
    {
        vTaskDelete(pxCreatedTask);
    }
    static void *malloc(size_t xSize)
    {
        return pvPortMalloc(xSize);
    }
    static void free(void *pointer)
    {
        vPortFree(pointer);
    }
    static void sleep(uint32_t sleep_time_ms)
    {
        vTaskDelay(pdMS_TO_TICKS(sleep_time_ms));
    }
};

class thread_mutex_lock
{
private:
    SemaphoreHandle_t semaphore;

public:
    thread_mutex_lock(void)
    {
        semaphore = xSemaphoreCreateMutex();
    }
    ~thread_mutex_lock(void)
    {
        vSemaphoreDelete(semaphore);
    }
    bool lock(void)
    {
        return xSemaphoreTake(semaphore, portMAX_DELAY);
    }
    bool lock(uint32_t delay_ms)
    {
        return xSemaphoreTake(semaphore, pdMS_TO_TICKS(delay_ms));
    }
    void unlock(void)
    {
        xSemaphoreGive(semaphore);
    }
};

class timer_helper
{
private:
    std::string name;
    esp_timer_handle_t lvgl_tick_timer;

public:
    timer_helper(std::string name)
    {
        this->name = name;
    }
    static timer_helper *create_timer(std::string name, void (*callback)(void *arg))
    {
        timer_helper *helper = new timer_helper(name);
        esp_timer_create_args_t lvgl_tick_timer_args = {};
        lvgl_tick_timer_args.callback = callback;
        lvgl_tick_timer_args.name = helper->name.c_str();
        ESP_ERROR_CHECK(esp_timer_create(&lvgl_tick_timer_args, &helper->lvgl_tick_timer));
        return helper;
    }
    static void destroy_timer(timer_helper *helper)
    {
        ESP_ERROR_CHECK(esp_timer_delete(helper->lvgl_tick_timer));
        delete helper;
    }
    static void start_timer(timer_helper *helper, uint64_t period)
    {
        ESP_ERROR_CHECK(esp_timer_start_periodic(helper->lvgl_tick_timer, period));
    }
    static void start_timer_once(timer_helper *helper, uint64_t period)
    {
        ESP_ERROR_CHECK(esp_timer_start_once(helper->lvgl_tick_timer, period));
    }
    static void stop_timer(timer_helper *helper)
    {
        ESP_ERROR_CHECK(esp_timer_stop(helper->lvgl_tick_timer));
    }
};