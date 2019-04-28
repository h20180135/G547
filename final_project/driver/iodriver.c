#include <linux/init.h>
#include <linux/module.h>
#include <asm/io.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mm.h>


MODULE_AUTHOR("Aakrati Jain and Vidushi Khajuria");
MODULE_LICENSE("GPL");

static  int GpioPin ;
struct GpioRegisters
{
    uint32_t GPFSEL[6];
    uint32_t Reserved1;
    uint32_t GPSET[2];
    uint32_t Reserved2;
    uint32_t GPCLR[2];
    uint32_t GPLEV[2];
    uint32_t Reserved3;
};

struct GpioRegisters *Registers;



/*
To configure the GPIO Pin as input or output  
*/

static void SetGPIOFunction(int GPIO, int functionCode)
{
    int registerIndex = GPIO / 10;
    int bit = (GPIO % 10) * 3;

    unsigned oldValue = Registers->GPFSEL[registerIndex];
    unsigned mask = 0b111 << bit;
    printk("Changing function of GPIO%d from %x to %x\n", GPIO, (oldValue >> bit) & 0b111, functionCode);
    Registers->GPFSEL[registerIndex] = (oldValue & ~mask) | ((functionCode << bit) & mask);
}


 /* To write to the GPIO Pin */
 
static void SetGPIOOutputValue(int GPIO, long outputValue)
{
    if (outputValue==1)
        Registers->GPSET[GPIO / 32] = (1 << (GPIO % 32));
    else
        Registers->GPCLR[GPIO / 32] = (1 << (GPIO % 32));
}



/* To read the GPIO Pin state  */

static int readGPIOinputValue(int GPIO)
{  
    uint32_t bit = 1 << (GPIO % 32);
    uint32_t mem = Registers-> GPLEV[GPIO/32];
    if(mem & bit)
	return 1;
	return 0;
}



/* Callback for write */

static ssize_t write_value_callback(struct device* dev,
    struct device_attribute* attr,
    const char* buf,
    size_t count)
{
    long out = 0;
    if (kstrtol(buf, 0, &out) < 0)
        return -EINVAL;
    if (out < 0)	
    	return - EINVAL;
    SetGPIOOutputValue( GpioPin, out);
    printk ("setting output of gpio %d to %lu \n",GpioPin,out);
    
    return count;
}


/*Callback for read */

static ssize_t  read_value_callback(struct device* dev,struct device_attribute* attr,char* buf)
{  
	int read_value;
    read_value=readGPIOinputValue(GpioPin); 
    printk("read  value of gpio %d is %d \n",GpioPin,read_value);
    return sprintf(buf,"\n");
}


/*Callback for mode select of input or output */

static ssize_t mode_select_callback(struct device* dev,struct device_attribute* attr,const char* buf,size_t count)
{
    long config = 0;
    if (kstrtol(buf, 0, &config) < 0)
        return -EINVAL;
    if (config < 0)	//Safety check
    	return - EINVAL;
    	if (config==0)
    	{
			printk("setting mode of gpio %d as input",GpioPin);
    	SetGPIOFunction(GpioPin, 0b000);}
    	else if(config ==1)
    	{
			        printk("setting mode of gpio %d as output",GpioPin);

    	SetGPIOFunction(GpioPin, 0b001);  
    }
    else
    printk("can't be configured \n");
    return count;
}

/*Callback for pin select of input or output */

static ssize_t pin_select_callback(struct device* dev,struct device_attribute* attr,const char* buf,size_t count)
{
    long set = 0;
    if (kstrtol(buf, 0, &set) < 0)
        return -EINVAL;
    if (set < 0)	//Safety check
    	return - EINVAL;
    	if (set<=28)
    	GpioPin=set;
    	printk("selecting gpio %d \n",GpioPin);
    return count;
}

static DEVICE_ATTR(pin, S_IRWXU | S_IRWXG, NULL, pin_select_callback);
static DEVICE_ATTR(mode, S_IRWXU | S_IRWXG, NULL, mode_select_callback);
static DEVICE_ATTR(value, S_IRWXU | S_IRWXG, read_value_callback, write_value_callback);

static struct class *s_pDeviceClass;
static struct class *s_pDeviceClass1;
static struct class *s_pDeviceClass2;
static struct device *s_pDeviceObject;
static struct device *s_pDeviceObject1;
static struct device *s_pDeviceObject2;
#define PERIPH_BASE 0x3f000000
#define GPIO_BASE (PERIPH_BASE + 0x200000)

static int __init LedBlinkModule_init(void)
{
    int result;
    int result1;
    int result2;
    Registers = (struct GpioRegisters *)ioremap(GPIO_BASE, sizeof(struct GpioRegisters));

    s_pDeviceClass = class_create(THIS_MODULE, "read_or_write");
    BUG_ON(IS_ERR(s_pDeviceClass));
    s_pDeviceClass1 = class_create(THIS_MODULE, "mode_select");
    BUG_ON(IS_ERR(s_pDeviceClass1));
    s_pDeviceClass2 = class_create(THIS_MODULE, "pin_select");
    BUG_ON(IS_ERR(s_pDeviceClass2));

    s_pDeviceObject = device_create(s_pDeviceClass, NULL, 0, NULL, "read_or_write");
    BUG_ON(IS_ERR(s_pDeviceObject));
    s_pDeviceObject1 = device_create(s_pDeviceClass1, NULL, 0, NULL, "mode_select");
    BUG_ON(IS_ERR(s_pDeviceObject1));
	s_pDeviceObject2 = device_create(s_pDeviceClass2, NULL, 0, NULL, "pin_select");
    BUG_ON(IS_ERR(s_pDeviceObject2));

    
    result = device_create_file(s_pDeviceObject, &dev_attr_value);
    BUG_ON(result < 0);
    result1 = device_create_file(s_pDeviceObject1, &dev_attr_mode);
    BUG_ON(result1 < 0);
    result2 = device_create_file(s_pDeviceObject2, &dev_attr_pin);
    BUG_ON(result2 < 0);

    return 0;
}

static void __exit LedBlinkModule_exit(void)
{
    iounmap(Registers);
    device_remove_file(s_pDeviceObject, &dev_attr_value);
    device_remove_file(s_pDeviceObject1, &dev_attr_mode);
    device_remove_file(s_pDeviceObject2, &dev_attr_pin);
    device_destroy(s_pDeviceClass, 0);
    device_destroy(s_pDeviceClass1, 0);
    device_destroy(s_pDeviceClass2, 0);
    class_destroy(s_pDeviceClass);
     class_destroy(s_pDeviceClass1);
     class_destroy(s_pDeviceClass2);
}

module_init(LedBlinkModule_init);
module_exit(LedBlinkModule_exit);





