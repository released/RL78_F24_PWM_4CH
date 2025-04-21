/*_____ I N C L U D E S ____________________________________________________*/
#include <stdio.h>
#include <string.h>

#include "r_smc_entry.h"
#include "platform.h"

#include "misc_config.h"
#include "custom_func.h"

/*_____ D E C L A R A T I O N S ____________________________________________*/

volatile struct flag_32bit flag_PROJ_CTL;
#define FLAG_PROJ_TIMER_PERIOD_1000MS                 	(flag_PROJ_CTL.bit0)
#define FLAG_PROJ_TRIG_BTN1                       	    (flag_PROJ_CTL.bit1)
#define FLAG_PROJ_TRIG_BTN2                 	        (flag_PROJ_CTL.bit2)
#define FLAG_PROJ_REVERSE3                    		    (flag_PROJ_CTL.bit3)
#define FLAG_PROJ_REVERSE4                              (flag_PROJ_CTL.bit4)
#define FLAG_PROJ_REVERSE5                              (flag_PROJ_CTL.bit5)
#define FLAG_PROJ_REVERSE6                              (flag_PROJ_CTL.bit6)
#define FLAG_PROJ_REVERSE7                              (flag_PROJ_CTL.bit7)


#define FLAG_PROJ_TRIG_1                                (flag_PROJ_CTL.bit8)
#define FLAG_PROJ_TRIG_2                                (flag_PROJ_CTL.bit9)
#define FLAG_PROJ_TRIG_3                                (flag_PROJ_CTL.bit10)
#define FLAG_PROJ_TRIG_4                                (flag_PROJ_CTL.bit11)
#define FLAG_PROJ_TRIG_5                                (flag_PROJ_CTL.bit12)
#define FLAG_PROJ_PWM_DUTY_INC                          (flag_PROJ_CTL.bit13)
#define FLAG_PROJ_PWM_DUTY_DEC                          (flag_PROJ_CTL.bit14)
#define FLAG_PROJ_REVERSE15                             (flag_PROJ_CTL.bit15)

/*_____ D E F I N I T I O N S ______________________________________________*/

volatile unsigned long counter_tick = 0;
volatile unsigned long btn_counter_tick = 0;

#define BTN_PRESSED_LONG                                (2500)

volatile unsigned int pwm_clk_src = 0;
volatile unsigned int pwm_duty[16] = {0};
volatile unsigned char pwm_ch_idx = 0;

/*_____ M A C R O S ________________________________________________________*/

/*_____ F U N C T I O N S __________________________________________________*/

unsigned long btn_get_tick(void)
{
	return (btn_counter_tick);
}

void btn_set_tick(unsigned long t)
{
	btn_counter_tick = t;
}

void btn_tick_counter(void)
{
	btn_counter_tick++;
    if (btn_get_tick() >= 60000)
    {
        btn_set_tick(0);
    }
}

unsigned long get_tick(void)
{
	return (counter_tick);
}

void set_tick(unsigned long t)
{
	counter_tick = t;
}

void tick_counter(void)
{
	counter_tick++;
    if (get_tick() >= 60000)
    {
        set_tick(0);
    }
}

void delay_ms(unsigned long ms)
{
	#if 1
    unsigned long tickstart = get_tick();
    unsigned long wait = ms;
	unsigned long tmp = 0;
	
    while (1)
    {
		if (get_tick() > tickstart)	// tickstart = 59000 , tick_counter = 60000
		{
			tmp = get_tick() - tickstart;
		}
		else // tickstart = 59000 , tick_counter = 2048
		{
			tmp = 60000 -  tickstart + get_tick();
		}		
		
		if (tmp > wait)
			break;
    }
	
	#else
	TIMER_Delay(TIMER0, 1000*ms);
	#endif
}


/*
    TAU1 PWM : 10K
        - SLAVE 1 : P12 
        - SLAVE 2 : P11
        - SLAVE 3 : P10
        - SLAVE 4 : P31
*/
unsigned int get_TAU1_pwm_ch_clk_src(void)
{
	return (pwm_clk_src);
}

void set_TAU1_pwm_ch_clk_src(unsigned int src)
{
	pwm_clk_src = src;
}

unsigned int get_TAU1_pwm_ch_duty(unsigned char ch)
{
	return (pwm_duty[ch]);
}

void set_TAU1_pwm_ch_duty(unsigned char ch ,unsigned int duty)
{
    pwm_duty[ch] = duty;
}

/*copy from R_Config_TAU1_0_Create*/
void generate_TAU1_pwm_ch_idx(unsigned char idx)
{
    switch(idx)
    {
        case 1:
            // slave 1
            TOM1 |= _0002_TAU_CH1_SLAVE_OUTPUT;
            TOL1 &= (uint16_t)~_0002_TAU_CH1_OUTPUT_LEVEL_L;
            TO1 &= (uint16_t)~_0002_TAU_CH1_OUTPUT_VALUE_1;
            PWMDLY2 &= _FFF3_TAU_CH1_OUTPUT_CLEAR;
            PWMDLY2 |= _0000_TAU_CH1_OUTPUT_NO_DELAY;
            TOE1 |= _0002_TAU_CH1_OUTPUT_ENABLE;        
            break;
        case 2:
            // slave 2
            TOM1 |= _0004_TAU_CH2_SLAVE_OUTPUT;
            TOL1 &= (uint16_t)~_0004_TAU_CH2_OUTPUT_LEVEL_L;
            TO1 &= (uint16_t)~_0004_TAU_CH2_OUTPUT_VALUE_1;
            PWMDLY2 &= _FFCF_TAU_CH2_OUTPUT_CLEAR;
            PWMDLY2 |= _0000_TAU_CH2_OUTPUT_NO_DELAY;
            TOE1 |= _0004_TAU_CH2_OUTPUT_ENABLE;
            break;
        case 3:
            // slave 3
            TOM1 |= _0008_TAU_CH3_SLAVE_OUTPUT;
            TOL1 &= (uint16_t)~_0008_TAU_CH3_OUTPUT_LEVEL_L;
            TO1 &= (uint16_t)~_0008_TAU_CH3_OUTPUT_VALUE_1;
            PWMDLY2 &= _FF3F_TAU_CH3_OUTPUT_CLEAR;
            PWMDLY2 |= _0000_TAU_CH3_OUTPUT_NO_DELAY;
            TOE1 |= _0008_TAU_CH3_OUTPUT_ENABLE;
            break;
        case 4:
            // slave 4
            TOM1 |= _0010_TAU_CH4_SLAVE_OUTPUT;
            TOL1 &= (uint16_t)~_0010_TAU_CH4_OUTPUT_LEVEL_L;
            TO1 &= (uint16_t)~_0010_TAU_CH4_OUTPUT_VALUE_1;
            PWMDLY2 &= _FCFF_TAU_CH4_OUTPUT_CLEAR;
            PWMDLY2 |= _0000_TAU_CH4_OUTPUT_NO_DELAY;
            TOE1 |= _0010_TAU_CH4_OUTPUT_ENABLE;
            break;
    }
}


void PWM_Channel_Duty_Decrease(unsigned char idx)
{
    int tmp = 0;
    int pwm_master = 0;
    unsigned int duty_hex = 0;

    tmp = get_TAU1_pwm_ch_duty(idx);
    pwm_master = get_TAU1_pwm_ch_clk_src()+1;

    duty_hex = (pwm_master) / 100 * 5;   // - 5 %
    tmp = (tmp <= 0) ? (0) : (tmp - duty_hex ) ;  
    set_TAU1_pwm_ch_duty(idx,tmp);

    #if 1   // debug
    printf("[%d]-duty:0x%02X,",idx,pwm_master);
    printf("0x%02X,",duty_hex );
    printf("0x%02X,",tmp );
    printf("(%2.2f)\r\n",(float) tmp*100/(pwm_master) );
    #endif

    switch(idx)
    {
        case 1: 
            TDR11 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 2:
            TDR12 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 3:
            TDR13 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 4:
            TDR14 = get_TAU1_pwm_ch_duty(idx);
            break;
    }
    generate_TAU1_pwm_ch_idx(idx);
}

void PWM_Channel_Duty_Increase(unsigned char idx)
{
    int tmp = 0;
    int pwm_master = 0;
    unsigned int duty_hex = 0;

    tmp = get_TAU1_pwm_ch_duty(idx);
    pwm_master = get_TAU1_pwm_ch_clk_src()+1;

    duty_hex = (pwm_master) / 100 * 5;   // + 5 %
    tmp = (tmp >= (pwm_master)) ? ((pwm_master)) : (tmp + duty_hex );   
    set_TAU1_pwm_ch_duty(idx,tmp);

    #if 1   // debug
    printf("[%d]+duty:0x%02X,",idx,pwm_master);
    printf("0x%02X,",duty_hex );
    printf("0x%02X,",tmp );
    printf("(%2.2f)\r\n",(float) tmp*100/(pwm_master) );
    #endif

    switch(idx)
    {
        case 1:
            TDR11 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 2: 
            TDR12 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 3: 
            TDR13 = get_TAU1_pwm_ch_duty(idx);
            break;
        case 4: 
            TDR14 = get_TAU1_pwm_ch_duty(idx);
            break;
    }
    generate_TAU1_pwm_ch_idx(idx);
}

void PWM_Process_Adjust(void)
{


    /*
        duty * 16
        --------  = target hex 
            10
    */

    if (FLAG_PROJ_PWM_DUTY_INC)
    {
        FLAG_PROJ_PWM_DUTY_INC = 0;
        PWM_Channel_Duty_Increase(pwm_ch_idx);
    }

    if (FLAG_PROJ_PWM_DUTY_DEC)
    {
        FLAG_PROJ_PWM_DUTY_DEC = 0;
        PWM_Channel_Duty_Decrease(pwm_ch_idx);
    }
}

void Timer_1ms_IRQ(void)
{
    tick_counter();

    if ((get_tick() % 1000) == 0)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 1;
    }

    if ((get_tick() % 50) == 0)
    {

    }	

    Button_Process_long_counter();
}


/*
    F24 target board
    LED1 connected to P66, LED2 connected to P67
*/
void LED_Toggle(void)
{
    // PIN_WRITE(6,6) = ~PIN_READ(6,6);
    // PIN_WRITE(6,7) = ~PIN_READ(6,7);
    P6_bit.no6 = ~P6_bit.no6;
    P6_bit.no7 = ~P6_bit.no7;
}

void loop(void)
{
	// static unsigned long LOG1 = 0;

    if (FLAG_PROJ_TIMER_PERIOD_1000MS)
    {
        FLAG_PROJ_TIMER_PERIOD_1000MS = 0;

        // printf("log(timer):%4d\r\n",LOG1++);
        LED_Toggle();             
    }

    Button_Process_in_polling();

    PWM_Process_Adjust();
}


// F24 EVB , P137/INTP0 , set both edge 
void Button_Process_long_counter(void)
{
    if (FLAG_PROJ_TRIG_BTN2)
    {
        btn_tick_counter();
    }
    else
    {
        btn_set_tick(0);
    }
}

void Button_Process_in_polling(void)
{
    static unsigned char cnt = 0;

    if (FLAG_PROJ_TRIG_BTN1)
    {
        FLAG_PROJ_TRIG_BTN1 = 0;
        printf("BTN pressed(%d)\r\n",cnt);

        if (cnt == 0)   //set both edge  , BTN pressed
        {
            FLAG_PROJ_TRIG_BTN2 = 1;
        }
        else if (cnt == 1)  //set both edge  , BTN released
        {
            FLAG_PROJ_TRIG_BTN2 = 0;
        }

        cnt = (cnt >= 1) ? (0) : (cnt+1) ;
    }

    if ((FLAG_PROJ_TRIG_BTN2 == 1) && 
        (btn_get_tick() > BTN_PRESSED_LONG))
    {         
        printf("BTN pressed LONG\r\n");
        btn_set_tick(0);
        FLAG_PROJ_TRIG_BTN2 = 0;
    }
}

// F24 EVB , P137/INTP0
void Button_Process_in_IRQ(void)    
{
    FLAG_PROJ_TRIG_BTN1 = 1;
}

void UARTx_Process(unsigned char rxbuf)
{    
    if (rxbuf > 0x7F)
    {
        printf("invalid command\r\n");
    }
    else
    {
        printf("press:%c(0x%02X)\r\n" , rxbuf,rxbuf);   // %c :  C99 libraries.
        switch(rxbuf)
        {
            case '1':
                FLAG_PROJ_TRIG_1 = 1;
                pwm_ch_idx = 1;
                break;
            case '2':
                FLAG_PROJ_TRIG_2 = 1;
                pwm_ch_idx = 2;
                break;
            case '3':
                FLAG_PROJ_TRIG_3 = 1;
                pwm_ch_idx = 3;
                break;
            case '4':
                FLAG_PROJ_TRIG_4 = 1;
                pwm_ch_idx = 4;
                break;

            case 'a':
            case 'A':
                FLAG_PROJ_PWM_DUTY_INC = 1;
                break;
            case 'd':
            case 'D':
                FLAG_PROJ_PWM_DUTY_DEC = 1;
                break;


            case 'X':
            case 'x':
                RL78_soft_reset(7);
                break;
            case 'Z':
            case 'z':
                RL78_soft_reset(1);
                break;
        }
    }
}

/*
    Reset Control Flag Register (RESF) 
    BIT7 : TRAP
    BIT6 : 0
    BIT5 : 0
    BIT4 : WDCLRF
    BIT3 : 0
    BIT2 : 0
    BIT1 : IAWRF
    BIT0 : LVIRF
*/
void check_reset_source(void)
{
    /*
        Internal reset request by execution of illegal instruction
        0  Internal reset request is not generated, or the RESF register is cleared. 
        1  Internal reset request is generated. 
    */
    uint8_t src = RESF;
    printf("Reset Source <0x%08X>\r\n", src);

    #if 1   //DEBUG , list reset source
    if (src & BIT0)
    {
        printf("0)voltage detector (LVD)\r\n");       
    }
    if (src & BIT1)
    {
        printf("1)illegal-memory access\r\n");       
    }
    if (src & BIT2)
    {
        printf("2)EMPTY\r\n");       
    }
    if (src & BIT3)
    {
        printf("3)EMPTY\r\n");       
    }
    if (src & BIT4)
    {
        printf("4)watchdog timer (WDT) or clock monitor\r\n");       
    }
    if (src & BIT5)
    {
        printf("5)EMPTY\r\n");       
    }
    if (src & BIT6)
    {
        printf("6)EMPTY\r\n");       
    }
    if (src & BIT7)
    {
        printf("7)execution of illegal instruction\r\n");       
    }
    #endif

}

/*
    7:Internal reset by execution of illegal instruction
    1:Internal reset by illegal-memory access
*/
//perform sofware reset
void _reset_by_illegal_instruction(void)
{
    static const unsigned char illegal_Instruction = 0xFF;
    void (*dummy) (void) = (void (*)(void))&illegal_Instruction;
    dummy();
}
void _reset_by_illegal_memory_access(void)
{
    #if 1
    const unsigned char ILLEGAL_ACCESS_ON = 0x80;
    IAWCTL |= ILLEGAL_ACCESS_ON;            // switch IAWEN on (default off)
    *(__far volatile char *)0x00000 = 0x00; //write illegal address 0x00000(RESET VECTOR)
    #else
    signed char __far* a;                   // Create a far-Pointer
    IAWCTL |= _80_CGC_ILLEGAL_ACCESS_ON;    // switch IAWEN on (default off)
    a = (signed char __far*) 0x0000;        // Point to 0x000000 (FLASH-ROM area)
    *a = 0;
    #endif
}

void RL78_soft_reset(unsigned char flag)
{
    switch(flag)
    {
        case 7: // do not use under debug mode
            _reset_by_illegal_instruction();        
            break;
        case 1:
            _reset_by_illegal_memory_access();
            break;
    }
}

// retarget printf
int __far putchar(int c)
{
    // F24 , UART0
    STMK0 = 1U;    /* disable INTST0 interrupt */
    SDR00L = (unsigned char)c;
    while(STIF0 == 0)
    {

    }
    STIF0 = 0U;    /* clear INTST0 interrupt flag */
    return c;
}

void hardware_init(void)
{
    // const unsigned char indicator[] = "hardware_init";
    BSP_EI();
    R_Config_UART0_Start();         // UART , P15 , P16
    R_Config_TAU0_1_Start();        // TIMER
    R_Config_INTC_INTP0_Start();    // BUTTON , P137 

    /*
        P12 : TO11
        P11 : TO12
        P10 : TO13
        P31 : TO14
    */
    R_Config_TAU1_0_Start();
    
    // check_reset_source();
    printf("%s finish\r\n\r\n",__func__);
}
