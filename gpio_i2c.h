/*
 * @version: 1.0
 * @Author: kuankuan
 * @Date: 2021-03-12 18:10:13
 * @LastEditTime: 2021-03-12 18:34:52
 */

typedef struct
{
    int speed;
    int error_code;
    void (*lock)(int);
    void (*delay_us)();
    void (*scl_write)(int);
    void (*sda_write)(int);
    void (*sda_set_dir)(int);/* 0 output / 1 input*/
    int (*sda_read)(void);
}gpio_i2c_t;


