#pragma once
#include <leida_def.hpp>
class leida_hard
{
private:
    QueueHandle_t uart_queue;
    void gpio_init(void)
    {
        gpio_config_t io_conf = {};
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_INPUT_OUTPUT;
        io_conf.pin_bit_mask = 1ULL << led_pin;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&io_conf));
        io_conf.intr_type = GPIO_INTR_DISABLE;
        io_conf.mode = GPIO_MODE_OUTPUT_OD;
        io_conf.pin_bit_mask = 1ULL << leida_power_pin;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
        io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
        ESP_ERROR_CHECK(gpio_config(&io_conf));
    }
    void iic_init(void)
    {
        i2c_config_t i2c_conf = {};
        i2c_conf.mode = I2C_MODE_MASTER;
        i2c_conf.sda_io_num = sda_pin;
        i2c_conf.scl_io_num = scl_pin;
        i2c_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
        i2c_conf.master.clk_speed = 200 * 1000;
        ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &i2c_conf));
        ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
    }
    void uart_init(void)
    {
        uart_config_t uart_config = {};
        uart_config.baud_rate = 256000;
        uart_config.data_bits = UART_DATA_8_BITS;
        uart_config.parity = UART_PARITY_DISABLE;
        uart_config.stop_bits = UART_STOP_BITS_1;
        uart_config.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS;
        uart_config.rx_flow_ctrl_thresh = 122;
        ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
        ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, GPIO_NUM_21, GPIO_NUM_20, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
        const int uart_buffer_size = 1024 * 2;
        ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, uart_buffer_size, uart_buffer_size, 10, &uart_queue, 0));
    }
    void nvs_init(void)
    {
        esp_err_t ret = nvs_flash_init();
        if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
        {
            ESP_ERROR_CHECK(nvs_flash_erase());
            ret = nvs_flash_init();
        }
        ESP_ERROR_CHECK(ret);
    }

public:
    class bmp280
    {
    private:
        leida_hard *leida_hard_obj;
        uint8_t bmp280_addr = 0x8;
        leida_err_e read(uint8_t addr, uint8_t *data)
        {
            i2c_cmd_handle_t cmd_link1 = i2c_cmd_link_create();
            i2c_cmd_handle_t cmd_link2 = i2c_cmd_link_create();
            i2c_master_start(cmd_link1);
            i2c_master_write_byte(cmd_link1, (bmp280_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link1, addr, true);
            i2c_master_stop(cmd_link1);

            i2c_master_start(cmd_link2);
            i2c_master_write_byte(cmd_link2, (bmp280_addr << 1) | I2C_MASTER_READ, true);
            i2c_master_read_byte(cmd_link2, data, I2C_MASTER_NACK);
            i2c_master_stop(cmd_link2);

            leida_err_e err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link1, pdMS_TO_TICKS(1000));
            if (err == leida_success)
                err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link2, pdMS_TO_TICKS(1000));

            i2c_cmd_link_delete(cmd_link1);
            i2c_cmd_link_delete(cmd_link2);
            return err;
        }
        leida_err_e write(uint8_t addr, uint8_t data)
        {
            i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
            i2c_master_start(cmd_link);
            i2c_master_write_byte(cmd_link, (bmp280_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link, addr, true);
            i2c_master_write_byte(cmd_link, data, true);
            i2c_master_stop(cmd_link);
            esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd_link, pdMS_TO_TICKS(100));
            i2c_cmd_link_delete(cmd_link);
            return (leida_err_e)err;
        }

    public:
        bmp280(leida_hard *leida_hard_obj)
        {
            this->leida_hard_obj = leida_hard_obj;
        }
        ~bmp280()
        {
        }
    };
    class bh1750
    {
    private:
        leida_hard *leida_hard_obj;
        uint8_t bh1750_addr = 0x8;
        leida_err_e read(uint8_t addr, uint8_t *data)
        {
            i2c_cmd_handle_t cmd_link1 = i2c_cmd_link_create();
            i2c_cmd_handle_t cmd_link2 = i2c_cmd_link_create();
            i2c_master_start(cmd_link1);
            i2c_master_write_byte(cmd_link1, (bh1750_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link1, addr, true);
            i2c_master_stop(cmd_link1);

            i2c_master_start(cmd_link2);
            i2c_master_write_byte(cmd_link2, (bh1750_addr << 1) | I2C_MASTER_READ, true);
            i2c_master_read_byte(cmd_link2, data, I2C_MASTER_NACK);
            i2c_master_stop(cmd_link2);

            leida_err_e err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link1, pdMS_TO_TICKS(1000));
            if (err == leida_success)
                err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link2, pdMS_TO_TICKS(1000));

            i2c_cmd_link_delete(cmd_link1);
            i2c_cmd_link_delete(cmd_link2);
            return err;
        }
        leida_err_e write(uint8_t addr, uint8_t data)
        {
            i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
            i2c_master_start(cmd_link);
            i2c_master_write_byte(cmd_link, (bh1750_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link, addr, true);
            i2c_master_write_byte(cmd_link, data, true);
            i2c_master_stop(cmd_link);
            esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd_link, pdMS_TO_TICKS(100));
            i2c_cmd_link_delete(cmd_link);
            return (leida_err_e)err;
        }

    public:
        bh1750(leida_hard *leida_hard_obj)
        {
            this->leida_hard_obj = leida_hard_obj;
        }
        ~bh1750()
        {
        }
    };
    class aht20
    {
    private:
        leida_hard *leida_hard_obj;
        uint8_t aht20_addr = 0x8;
        leida_err_e read(uint8_t addr, uint8_t *data)
        {
            i2c_cmd_handle_t cmd_link1 = i2c_cmd_link_create();
            i2c_cmd_handle_t cmd_link2 = i2c_cmd_link_create();
            i2c_master_start(cmd_link1);
            i2c_master_write_byte(cmd_link1, (aht20_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link1, addr, true);
            i2c_master_stop(cmd_link1);

            i2c_master_start(cmd_link2);
            i2c_master_write_byte(cmd_link2, (aht20_addr << 1) | I2C_MASTER_READ, true);
            i2c_master_read_byte(cmd_link2, data, I2C_MASTER_NACK);
            i2c_master_stop(cmd_link2);

            leida_err_e err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link1, pdMS_TO_TICKS(1000));
            if (err == leida_success)
                err = (leida_err_e)i2c_master_cmd_begin(I2C_NUM_0, cmd_link2, pdMS_TO_TICKS(1000));

            i2c_cmd_link_delete(cmd_link1);
            i2c_cmd_link_delete(cmd_link2);
            return err;
        }
        leida_err_e write(uint8_t addr, uint8_t data)
        {
            i2c_cmd_handle_t cmd_link = i2c_cmd_link_create();
            i2c_master_start(cmd_link);
            i2c_master_write_byte(cmd_link, (aht20_addr << 1) | I2C_MASTER_WRITE, true);
            i2c_master_write_byte(cmd_link, addr, true);
            i2c_master_write_byte(cmd_link, data, true);
            i2c_master_stop(cmd_link);
            esp_err_t err = i2c_master_cmd_begin(I2C_NUM_0, cmd_link, pdMS_TO_TICKS(100));
            i2c_cmd_link_delete(cmd_link);
            return (leida_err_e)err;
        }

    public:
        aht20(leida_hard *leida_hard_obj)
        {
            this->leida_hard_obj = leida_hard_obj;
        }
        ~aht20()
        {
        }
    };
    class leida
    {
    public:
        typedef struct leida_target_t
        {
            int16_t xpos_mm;
            int16_t ypos_mm;
            int16_t speed_cms;
            uint16_t reslusion_mm;
        } leida_target_t;

        leida(leida_hard *leida_hard_obj);
        ~leida();
        void power(bool sw);
        uint8_t get_targets(leida_target_t *targets, uint8_t target_num);

    private:
        thread_mutex_lock *target_buffer_lock;
        leida_target_t targets_buffer[3];
        uint8_t target_num;
        thread_helper *read_thread;
        leida_hard *leida_hard_obj;
        void read_task(void *param);
    };
    gpio_num_t pir_pin = GPIO_NUM_13;
    gpio_num_t leida_power_pin = GPIO_NUM_12;
    gpio_num_t led_pin = GPIO_NUM_2;
    gpio_num_t sda_pin = GPIO_NUM_6;
    gpio_num_t scl_pin = GPIO_NUM_7;
    bmp280 *bmp280_obj = NULL;
    bh1750 *bh1750_obj = NULL;
    aht20 *aht20_obj = NULL;
    leida *leida_obj = NULL;
    leida_hard()
    {
        nvs_init();
        gpio_init();
        iic_init();
        uart_init();
        leida_obj = new leida(this);
        aht20_obj = new aht20(this);
        bh1750_obj = new bh1750(this);
        bmp280_obj = new bmp280(this);
        led(true);
    }
    ~leida_hard()
    {
        delete bmp280_obj;
        delete bh1750_obj;
        delete aht20_obj;
        delete leida_obj;
        led(false);
    }

    void led(bool sw)
    {
        gpio_set_level(led_pin, !sw);
    }
};