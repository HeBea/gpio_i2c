/*
 * @version: 1.0
 * @Author: kuankuan
 * @Date: 2021-03-12 18:10:13
 * @LastEditTime: 2021-03-12 18:12:14
 */

typedef struct
{
    int speed;
    int error_code;
    void (*lock)(int);
    void (*scl_set)(int);
    
}gpio_i2c_t;


