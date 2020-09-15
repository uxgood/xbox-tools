#include <stdio.h>
#include <sys/io.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
// wdt for x3 plus /x3 pro /x5
#define outportb(port,data)		outb(data,port)
#define inportb(port)				inb(port)

//x3 plus register//
#define REG_X3PLUS				0x4E
#define VAL_X3PLUS				0x4F
#define LDS_X3PLUS				0x07
#define LDN_WDT_X3PLUS			0x08
#define WDT_CONTROL_X3PLUS	0x30
#define WDT_CONFIG_X3PLUS		0xF5
#define WDT_TIMEOUT_X3PLUS		0xF6
#define CHIPID_X3PLUS				0x20
#define TIMEOUT_30_X3PLUS		0x1E
#define TIMEOUT_0_X3PLUS		0x00
#define BIT_WDT_EN_X3PLUS		0
#define FLAG_UNLOCK_X3PLUS		0x87
#define FLAG_LOCK_X3PLUS		0xAA

//x3 pro register //
#define REG_X3PRO					0x2E
#define VAL_X3PRO					0x2F
#define LDS_X3PRO					0x07
#define LDN_WDT_X3PRO			0x07
#define WDT_CONTROL_X3PRO		0x71
#define WDT_CONFIG_X3PRO		0x72
#define WDT_TIMEOUT_X3PRO		0x73
#define CHIPID_X3PRO				0x20
#define BIT_WDT_EN_X3PRO		6
#define BIT_WDT_SEC_X3PRO		7
#define TIMEOUT_30_X3PRO		0x1E
#define TIMEOUT_0_X3PRO			0x00
#define FLAG_UNLOCK_X3PRO		0x87
#define FLAG_UNLOCK_X3PRO1		0x01
#define FLAG_UNLOCK_X3PRO2		0x55
#define FLAG_LOCK_X3PRO			0xAA

//x5 register //
#define REG_X5						0x4E
#define VAL_X5						0x4F
#define LDS_X5						0x07
#define LDN_WDT_X5				0x07
#define WDT_CONTROL_X5			0xF0
#define WDT_CONFIG_X5			0xF5
#define WDT_TIMEOUT_X5			0xF6
#define CHIPID_X5					0x20
#define SIO_REG_ENABLE			0x30
#define BIT_SIO_REG				0
#define BIT_WDOUT_EN_X5			7
#define BIT_WD_RST_EN_X5			0
#define BIT_WD_EN_X5				5
#define BIT_WDTMOUT_SYS_X5		6
#define TIMEOUT_30_X5			0x1E
#define TIMEOUT_0_X5				0x00
#define FLAG_UNLOCK_X5			0x87
#define FLAG_LOCK_X5				0xAA

//x7 register //
#define REG_X3						0x2E
#define VAL_X3						0x2F
#define LDS_X3						0x07
#define LDN_WDT_X3				0x08
#define WDT_CONTROL_X3			0x30
#define WDT_CONFIG_X3			0xF5
#define WDT_TIMEOUT_X3			0xF6
#define CHIPID_X3					0x20
#define TIMEOUT_30_X3			0x1E
#define TIMEOUT_0_X3				0x00
#define FLAG_UNLOCK_X3			0x87
#define FLAG_LOCK_X3				0xAA
#define FLAG_WDT_EN_X3			0x01
#define FLAG_WDT_SEC_X3			0x00

int superio_inb_x3plus(int reg);
void superio_outb_x3plus(int val, int reg);
void superio_enter_x3plus(void);
void superio_exit_x3plus(void);
void superio_select_x3plus(int ld);
void superio_set_bit_x3plus(int reg, int bit);
void superio_clear_bit_x3plus(int reg, int bit);
int get_chipid_x3plus(int reg);
int feed_wdt_x3plus();
void exit_wdt_x3plus(int sig);

int superio_inb_x3pro(int reg);
void superio_outb_x3pro(int val, int reg);
void superio_enter_x3pro(void);
void superio_exit_x3pro(void);
void superio_select_x3pro(int ldn);
void superio_set_bit_x3pro(int reg, int bit);
void superio_clear_bit_x3pro(int reg, int bit);
int get_chipid_x3pro(int reg);
int feed_wdt_x3pro();
void exit_wdt_x3pro(int sig);

int superio_inb_x5(int reg);
void superio_outb_x5(int val, int reg);
void superio_enter_x5(void);
void superio_exit_x5(void);
void superio_select_x5(int ldn);
void superio_set_bit_x5(int reg, int bit);
void superio_clear_bit_x5(int reg, int bit);
int get_chipid_x5(int reg);
int feed_wdt_x5();
void exit_wdt_x5(int sig);

int superio_inb_x3(int reg);
void superio_outb_x3(int val, int reg);
void superio_enter_x3(void);
void superio_exit_x3(void);
void superio_select_x3(int ldn);
int get_chipid_x3(int reg);
int feed_wdt_x3();
void exit_wdt_x3(int sig);

int main()
{
	if(iopl(3))
	{
		perror("iopl");
		printf("\nERROR: Fail to change I/O privilege by iopl().\n");
		return -1;
	}
	if ( get_chipid_x3plus(CHIPID_X3PLUS) == 0xC563 )
	{
		signal(SIGINT,exit_wdt_x3plus);
		while (1)
		{
			feed_wdt_x3plus();
			sleep(25);
		}
	}
	else if( get_chipid_x3pro(CHIPID_X3PRO) == 0x8712 )
	{
		signal(SIGINT,exit_wdt_x3pro);
		while (1)
		{
			feed_wdt_x3pro();
			sleep(25);
		}
	}
	else if( get_chipid_x5(CHIPID_X5) == 0x1106 )
	{
		signal(SIGINT,exit_wdt_x5);
		while (1)
		{
			feed_wdt_x5();
			sleep(25);
		}
	}
	else
	{
		signal(SIGINT,exit_wdt_x3);
		while (1)
		{
			feed_wdt_x3();
			sleep(25);
		}
	}

}

int superio_inb_x3plus(int reg)
{
	outportb(REG_X3PLUS, reg);
	return inb(VAL_X3PLUS);
}

void superio_outb_x3plus(int val, int reg)
{
	outportb(REG_X3PLUS,reg );
	outportb(VAL_X3PLUS, val);
}

void superio_enter_x3plus(void)
{
	outportb(REG_X3PLUS,FLAG_UNLOCK_X3PLUS);
	outportb(REG_X3PLUS,FLAG_UNLOCK_X3PLUS);
}

void superio_exit_x3plus(void)
{
	outportb(REG_X3PLUS,FLAG_LOCK_X3PLUS);
	outportb(VAL_X3PLUS,FLAG_LOCK_X3PLUS);
}

void superio_select_x3plus(int ld)
{
	outportb(REG_X3PLUS, LDS_X3PLUS);
	outportb(VAL_X3PLUS, ld);
}

void superio_set_bit_x3plus(int reg, int bit)
{
	int val;
	val = superio_inb_x3plus(reg);
	val |= (1<<bit) ;
	superio_outb_x3plus(val,reg);
}

void superio_clear_bit_x3plus(int reg, int bit)
{
	int val;
	val = superio_inb_x3plus(reg);
	val &= ~(1<<bit);
	superio_outb_x3plus(val,reg);
}

int get_chipid_x3plus(int reg)
{
	int val;
	superio_enter_x3plus();
	val  = superio_inb_x3plus(reg) << 8;
	val |= superio_inb_x3plus(reg + 1);
	superio_exit_x3plus();
	return val;
}

int feed_wdt_x3plus()
{
	superio_enter_x3plus();
	superio_select_x3plus(LDN_WDT_X3PLUS);

	superio_set_bit_x3plus(WDT_CONFIG_X3PLUS,BIT_WDT_EN_X3PLUS);
	superio_outb_x3plus(TIMEOUT_30_X3PLUS,WDT_TIMEOUT_X3PLUS);

	superio_exit_x3plus();
	return 0;
}

void exit_wdt_x3plus(int sig)
{
	feed_wdt_x3plus();
	superio_enter_x3plus();
	superio_select_x3plus(LDN_WDT_X3PLUS);

	superio_clear_bit_x3plus(WDT_CONFIG_X3PLUS,BIT_WDT_EN_X3PLUS);
	superio_outb_x3plus(TIMEOUT_0_X3PLUS,WDT_TIMEOUT_X3PLUS);

	superio_exit_x3plus();
	exit(0) ;
}

int superio_inb_x3pro(int reg)
{
	outportb(REG_X3PRO, reg);
	return inb(VAL_X3PRO);
}

void superio_outb_x3pro(int val, int reg)
{
	outportb(REG_X3PRO,reg );
	outportb(VAL_X3PRO, val);
}

void superio_enter_x3pro(void)
{
	outportb(REG_X3PRO,FLAG_UNLOCK_X3PRO);
	outportb(REG_X3PRO,FLAG_UNLOCK_X3PRO1 );
	outportb(REG_X3PRO,FLAG_UNLOCK_X3PRO2 );
	outportb(REG_X3PRO,FLAG_UNLOCK_X3PRO2 );
}

void superio_exit_x3pro(void)
{
	outportb(REG_X3PRO,FLAG_LOCK_X3PRO);
	outportb(VAL_X3PRO,FLAG_LOCK_X3PRO);
}

void superio_select_x3pro(int ldn)
{
	outportb(REG_X3PRO, LDS_X3PRO);
	outportb(VAL_X3PRO, ldn);
}

void superio_set_bit_x3pro(int reg, int bit)
{
	int val;
	val = superio_inb_x3pro(reg);
	val |= (1<<bit) ;
	superio_outb_x3pro(val,reg);
}

void superio_clear_bit_x3pro(int reg, int bit)
{
	int val;
	val = superio_inb_x3pro(reg);
	val &= ~(1<<bit);
	superio_outb_x3pro(val,reg);
}

int get_chipid_x3pro(int reg)
{
	int val;
	superio_enter_x3pro();
	val  = superio_inb_x3pro(reg) << 8;
	val |= superio_inb_x3pro(reg + 1);
	superio_exit_x3pro();
	return val;
}

int feed_wdt_x3pro()
{
	superio_enter_x3pro();
	superio_select_x3pro(LDN_WDT_X3PRO);

	superio_set_bit_x3pro(WDT_CONFIG_X3PRO,BIT_WDT_SEC_X3PRO);
	superio_set_bit_x3pro(WDT_CONFIG_X3PRO,BIT_WDT_EN_X3PRO);
	superio_outb_x3pro(TIMEOUT_30_X3PRO,WDT_TIMEOUT_X3PRO);

	superio_exit_x3pro();
	return 0;
}

void exit_wdt_x3pro(int sig)
{
	feed_wdt_x3pro();
	superio_enter_x3pro();
	superio_select_x3pro(LDN_WDT_X3PRO);

	superio_clear_bit_x3pro(WDT_CONFIG_X3PRO,BIT_WDT_EN_X3PRO);
	superio_outb_x3pro(TIMEOUT_0_X3PRO,WDT_TIMEOUT_X3PRO);

	superio_exit_x3pro();
	exit(0) ;
}

int superio_inb_x5(int reg)
{
	outportb(REG_X5, reg);
	return inb(VAL_X5);
}

void superio_outb_x5(int val, int reg)
{
	outportb(REG_X5,reg );
	outportb(VAL_X5, val);
}

void superio_enter_x5(void)
{
	outportb(REG_X5,FLAG_UNLOCK_X5);
	outportb(REG_X5,FLAG_UNLOCK_X5);
}

void superio_exit_x5(void)
{
	outportb(REG_X5,FLAG_LOCK_X5);
	outportb(VAL_X5,FLAG_LOCK_X5);
}

void superio_select_x5(int ldn)
{
	outportb(REG_X5, LDS_X5);
	outportb(VAL_X5, ldn);
}

void superio_set_bit_x5(int reg, int bit)
{
	int val;
	val = superio_inb_x5(reg);
	val |= (1<<bit) ;
	superio_outb_x5(val,reg);
}

void superio_clear_bit_x5(int reg, int bit)
{
	int val;
	val = superio_inb_x5(reg);
	val &= ~(1<<bit);
	superio_outb_x5(val,reg);
}

int get_chipid_x5(int reg)
{
	int val;
	superio_enter_x5();
	val  = superio_inb_x5(reg) << 8;
	val |= superio_inb_x5(reg + 1);
	superio_exit_x5();
	return val;
}

int feed_wdt_x5()
{
	superio_enter_x5();
	superio_select_x5(LDN_WDT_X5);
	superio_set_bit_x5(SIO_REG_ENABLE,BIT_SIO_REG);
	superio_set_bit_x5(WDT_CONTROL_X5,BIT_WDOUT_EN_X5);
	superio_set_bit_x5(WDT_CONTROL_X5,BIT_WD_RST_EN_X5);
	superio_set_bit_x5(WDT_CONFIG_X5,BIT_WD_EN_X5);
	superio_set_bit_x5(WDT_CONFIG_X5,BIT_WDTMOUT_SYS_X5);
	superio_outb_x5(TIMEOUT_30_X5,WDT_TIMEOUT_X5);

	superio_exit_x5();
	return 0;
}

void exit_wdt_x5(int sig)
{
	feed_wdt_x5();
	superio_enter_x5();
	superio_select_x5(LDN_WDT_X5);

	superio_clear_bit_x5(WDT_CONFIG_X5,BIT_WD_EN_X5);

	superio_exit_x5();
	exit(0) ;
}

int superio_inb_x3(int reg)
{
	outportb(REG_X3, reg);
	return inb(VAL_X3);
}

void superio_outb_x3(int val, int reg)
{
	outportb(REG_X3,reg );
	outportb(VAL_X3, val);
}

void superio_enter_x3(void)
{
	outportb(REG_X3,FLAG_UNLOCK_X3);
	outportb(REG_X3,FLAG_UNLOCK_X3);
}

void superio_exit_x3(void)
{
	outportb(REG_X3,FLAG_LOCK_X3);
	outportb(VAL_X3,FLAG_LOCK_X3);
}

void superio_select_x3(int ldn)
{
	outportb(REG_X3, LDS_X3);
	outportb(VAL_X3, ldn);
}

int get_chipid_x3(int reg)
{
	int val;
	superio_enter_x3();
	val  = superio_inb_x3(reg) << 8;
	val |= superio_inb_x3(reg + 1);
	superio_exit_x3();
	return val;
}

int feed_wdt_x3()
{
	superio_enter_x3();
	superio_select_x3(LDN_WDT_X3);

	superio_outb_x3(WDT_CONTROL_X3,FLAG_WDT_EN_X3);
	superio_outb_x3(WDT_CONFIG_X3,FLAG_WDT_SEC_X3);
	superio_outb_x3(TIMEOUT_30_X3,WDT_TIMEOUT_X3);

	superio_exit_x3();
	return 0;
}

void exit_wdt_x3(int sig)
{
	feed_wdt_x3();
	superio_enter_x3();
	superio_select_x3(LDN_WDT_X3);

	superio_outb_x3(TIMEOUT_0_X3,WDT_TIMEOUT_X3);

	superio_exit_x3();
	exit(0) ;
}
