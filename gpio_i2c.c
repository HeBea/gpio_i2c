/*
 * @version: 1.0
 * @Author: kuankuan
 * @Date: 2021-03-12 18:10:04
 * @LastEditTime: 2021-03-12 18:53:23
 */

#if(INLINE_ENABLE)
    #define INLINE inline
#else
    #define INLINE   
#endif

#define input   1
#define output  0
#define high    1
#define low     0

#define SpeedToUs(speed)    ((int)(1000000U/speed/2))

static INLINE void i2c_start(gpio_i2c_t *i2c)
{
    int t = SpeedToUs(i2c->speed);
    i2c->sda_set_dir(output);
    i2c->sda_write(high);
    i2c->delay_us(t);
    GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    GPIO_CLR(i2c->SDA_Port, i2c->SDA_Pin);
    i2c->delay_us(t);
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
}


static INLINE void i2c_stop(GPIO_I2C_Typedef *i2c, int speed)
{
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    GPIO_CLR(i2c->SDA_Port, i2c->SDA_Pin);
    i2c->delay_us(t);
    GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    i2c->sda_write(high);
    i2c->delay_us(t);
}


static INLINE uint8_t i2c_wait_ack(GPIO_I2C_Typedef *i2c, int speed)
{
    i2c->sda_write(high);
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_INPUT);
    i2c->delay_us(t);
    GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    if(GPIO_READ(i2c->SDA_Port, i2c->SDA_Pin))
    {
        i2c_stop(i2c, speed);
        return 0;
    }
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
    return 1; 
}

static INLINE void i2c_send_ack(GPIO_I2C_Typedef *i2c, int speed)
{
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
    GPIO_CLR(i2c->SDA_Port, i2c->SDA_Pin);
    i2c->delay_us(t);
    GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);    
}

static INLINE void i2c_send_noack(GPIO_I2C_Typedef *i2c, int speed)
{
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
    i2c->sda_write(high);
    i2c->delay_us(t);
    GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
    i2c->delay_us(t);
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);    
}


static INLINE void i2c_send_data(GPIO_I2C_Typedef *i2c, uint8_t data, int speed)
{
    uint8_t i = 8;

    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
    while(i--)
    {
        GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
        I2C_DELAY_US(10);
        if(data & 0x80)
        {
            i2c->sda_write(high);
        }         
        else
        {
            GPIO_CLR(i2c->SDA_Port, i2c->SDA_Pin);
        }
        
        i2c->delay_us(t);
        data <<= 1;
        GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
        i2c->delay_us(t);
        GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
        i2c->delay_us(t);
    }    
}


static INLINE uint8_t i2c_receive_data(GPIO_I2C_Typedef *i2c, int speed)
{
    uint8_t i = 8;
    uint8_t data = 0;
    
    i2c->sda_write(high);
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_INPUT);
    while(i--)
    {
        data <<= 1;
        GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
        i2c->delay_us(t);
        GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
        i2c->delay_us(t);
        if(GPIO_READ(i2c->SDA_Port, i2c->SDA_Pin)) 
            data |= 0x01;
    }
    GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
    
    return(data);    
}

 
static INLINE uint8_t i2c_write_byte(GPIO_I2C_Typedef *i2c, uint8_t dev_addr, uint8_t addr, const uint8_t *pdata, int speed)
{    
    i2c_start(i2c, speed);
    i2c_send_data(i2c, dev_addr, speed);
    if(i2c_wait_ack(i2c, speed) == 0)
    {
        i2c_stop(i2c, speed);
        return 0;
    }
    i2c_send_data(i2c, addr, speed);
    if(i2c_wait_ack(i2c, speed) == 0)
    {
        i2c_stop(i2c, speed);
        return 0;
    }

    i2c_send_data(i2c, *pdata, speed);
    if(i2c_wait_ack(i2c, speed) == 0)
    {
        i2c_stop(i2c, speed);
        return 0;
    }
    
    i2c_stop(i2c, speed);
    
    return 1;    
}


uint8_t i2c_write_bytes(GPIO_I2C_Typedef *i2c, uint8_t dev_addr, uint8_t addr, const uint8_t *pdata, uint16_t num, int speed)
{
    uint8_t ret = 0;
    uint16_t i;
#if(DEBUG)
    u1_printf("i2c addr : %hhx, mem_addr %hhx write:", dev_addr, addr);
    for(int k = 0; k < num; k++)
    {
        u1_printf("%hhx ", pdata[k]);
    }	
    u1_printf("\r\n");
#endif
		
	OS_TaskSuspendAll();			/*avoid switch context*/
    for(i = 0; i < num; i++)
    {
        ret = i2c_write_byte(i2c, dev_addr, addr++, pdata++, speed);
        if(ret == 0)
        {
            break;
        }
        //I2C_DELAY_US(1500);
		//HAL_Delay(1);
    }
	OS_TaskResumeAll();				/*protect end*/
    return ret;
}

uint8_t i2c_read_bytes(GPIO_I2C_Typedef *i2c, uint8_t dev_addr, uint8_t addr, uint8_t *pdata, uint16_t num, int speed)
{
		uint8_t i, res = 0;
		
		OS_TaskSuspendAll();			/*avoid switch context*/
		do{
			i2c_start(i2c, speed);
			i2c_send_data(i2c, dev_addr, speed);
			if(i2c_wait_ack(i2c, speed) == 0)
			{
					i2c_stop(i2c, speed);
					break;
			}
			i2c_send_data(i2c, addr, speed);
			if(i2c_wait_ack(i2c, speed) == 0)
			{
					i2c_stop(i2c, speed);
					break;
			}
			
			i2c_start(i2c, speed);
			i2c_send_data(i2c, dev_addr | 0x01, speed);      //read mode bit0 is always 1
			if(i2c_wait_ack(i2c, speed) == 0)
			{
					i2c_stop(i2c, speed);
					break;
			}
			for(i = 0; i < num; i++)
			{
					*pdata++ = i2c_receive_data(i2c, speed);
					if(i < (num - 1))
							i2c_send_ack(i2c, speed);
			}
			i2c_send_noack(i2c, speed);
			i2c_stop(i2c, speed);
			
			res = 1;	//read success
			
		}while(0);
		/*i2c read fail*/
		
		OS_TaskResumeAll();		/*protect end*/
		return res;	
}

void I2cWriteBit(GPIO_I2C_Typedef *i2c, uint8_t slave_address, uint8_t reg_address, uint8_t bitNum, uint8_t data, int speed)
{
	uint8_t value;
	i2c_read_bytes(i2c, slave_address, reg_address, &value, 1, speed);
	value = (data != 0) ? (value | (1 << bitNum)) : (value & ~(1 << bitNum));
	i2c_write_bytes(i2c, slave_address, reg_address, &value, 1, speed);
}

void I2cWriteBits(GPIO_I2C_Typedef *i2c, uint8_t slave_address, uint8_t reg_address, uint8_t bitStart, uint8_t length, uint8_t data, int speed)
{
	uint8_t byte;
	i2c_read_bytes(i2c, slave_address, reg_address, &byte, 1, speed);
	uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
	data <<= (bitStart - length + 1); 										// shift data into correct position
	data &= mask; 															// zero all non-important bits in data
	byte &= ~(mask); 														// zero all important bits in existing byte
	byte |= data; 															// combine data with existing byte
	i2c_write_bytes(i2c, slave_address, reg_address, &byte, 1, speed);
}

void i2c_init(GPIO_I2C_Typedef *i2c, int speed)
{
	GPIO_SET_DIR(i2c->SCL_Port, i2c->SCL_Pin, GPIO_MODE_OUTPUT_OD);
	GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
	i2c_stop(i2c, speed);
}


void i2c_test(GPIO_I2C_Typedef *i2c, int speed)
{
	GPIO_SET_DIR(i2c->SCL_Port, i2c->SCL_Pin, GPIO_MODE_OUTPUT_OD);
    GPIO_SET_DIR(i2c->SDA_Port, i2c->SDA_Pin, GPIO_MODE_OUTPUT_OD);
	while(1)
	{
		GPIO_SET(i2c->SCL_Port, i2c->SCL_Pin);
		i2c->sda_write(high);
		i2c->delay_us(t);
		GPIO_CLR(i2c->SCL_Port, i2c->SCL_Pin);
		GPIO_CLR(i2c->SDA_Port, i2c->SDA_Pin);
		i2c->delay_us(t);
	}
}


void i2c_device_init(void)
{
	for(int i = 0; i < POWER_1500_NUM; i++)
	{
		ae1500_init(&power_1500[i]);
		osDelay(1);	
	}
    u1_printf("ae1500 init success\r\n");
	for(int i = 0; i <  FAN_CHIP_NUM; i++)	//
	{
		max31790_init(&fan_driver[i]);
		max31790_set_speed(&fan_driver[i]);
		osDelay(1);	
	}
    u1_printf("fan driver init success\r\n");
}
