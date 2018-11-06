#include "reg51.h"
#include "intrins.h"

//===========================================



/* define SFR */
sfr AUXR = 0x8e;                    //Auxiliary register

/*----ADC Register -=-*/
sfr ADC_CONTR = 0xC5;
sfr ADC_DATA = 0xC6;
sfr ADC_LOW2 = 0xBE;

/*-----I/O----------*/
//sfr P0M0  = 0x93;
//sfr P0M1  = 0x94;
sfr P1M0  = 0x91;
sfr P1M1  = 0x92;
sfr P2M0  = 0x95;
sfr P2M1  = 0x96;
sfr P3M0  = 0xB1;
sfr P3M1  = 0xB2;

//=============================================
typedef unsigned char BYTE;
typedef unsigned int WORD;
unsigned char TIME_BASE;
unsigned char TIME_1_sec;
unsigned char TIME_1_min;
unsigned char TIME_2_sec;
unsigned char TIME_2_min;
unsigned char MACT_ENDB;
unsigned char time_day;
unsigned char time_hour;
unsigned char time_min;
unsigned char TIME_MS;
unsigned char frequnce_of_heart;
unsigned char m,n;
unsigned char TIME_queue[]={0,0,0,0,0,0,0,0,0,0};
unsigned char time_number;
unsigned char time_bluetooth;
unsigned char button_A;
unsigned char cnt;
int time_numberx;
int time_sum;
int count,count1,count2,re;  

sbit LCD_RS=P3^2;    /*LCD寄存器选择信号*/
sbit LCD_R_W=P3^3;   /*LCD读写信号*/
sbit LCD_E=P3^4;     /*LCD片选信号*/
// 控制引脚定义，不同的连接必须修改的部分
sbit button1 = P1^4;
sbit button2 = P1^5;
sbit RS=P3^2;
sbit RW=P3^3; 
sbit Elcm=P3^4;  
unsigned char lcd_data[5];
unsigned int i,k,j;
unsigned char disp_bool;
unsigned char tempC;
unsigned char overflag;
unsigned char flag;
#define Uchar unsigned char
#define Uint unsigned int
#define uchar unsigned char
#define uint unsigned int
#define DataPort P2                // 数据端口
#define Busy    0x80
#define uchar unsigned char
#define uint unsigned int
#define ulong unsigned long
#define _Nop() _nop_()
char idata word_storeroom[]=
{'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'}; 
void Delay400Ms(void);
void Delay5Ms(void);
void WaitForEnable( void );
void LcdWriteData( char dataW );
void LcdWriteCommand( Uchar CMD,Uchar AttribC );
void LcdReset( void );
void delay(unsigned int t);
void DispOneChar(Uchar x,Uchar y,Uchar Wdata);
void ePutstr(Uchar x,Uchar y, Uchar code *ptr);
void Send_OneByte(char ch);
uint Adc(unsigned char n);

/*Declare SFR associated with the PCA */
sbit EPCAI      =   IE^6;
sfr CCON        =   0xD8;           //PCA control register
sbit CCF0       =   CCON^0;         //PCA module-0 interrupt flag
sbit CCF1       =   CCON^1;         //PCA module-1 interrupt flag
sbit CR         =   CCON^6;         //PCA timer run control bit
sbit CF         =   CCON^7;         //PCA timer overflow flag
sfr CMOD        =   0xD9;           //PCA mode register
sfr CL          =   0xE9;           //PCA base timer LOW
sfr CH          =   0xF9;           //PCA base timer HIGH
sfr CCAPM0      =   0xDA;           //PCA module-0 mode register
sfr CCAP0L      =   0xEA;           //PCA module-0 capture register LOW
sfr CCAP0H      =   0xFA;           //PCA module-0 capture register HIGH
sfr CCAPM1      =   0xDB;           //PCA module-1 mode register
sfr CCAP1L      =   0xEB;           //PCA module-1 capture register LOW
sfr CCAP1H      =   0xFB;           //PCA module-1 capture register HIGH
sfr CCAPM2      =   0xDC;           //PCA module-2 mode register
sfr CCAP2L      =   0xEC;           //PCA module-2 capture register LOW
sfr CCAP2H      =   0xFC;           //PCA module-2 capture register HIGH
sfr CCAPM3      =   0xDD;           //PCA module-3 mode register
sfr CCAP3L      =   0xED;           //PCA module-3 capture register LOW
sfr CCAP3H      =   0xFD;           //PCA module-3 capture register HIGH
sfr PCAPWM0     =   0xF2;
sfr PCAPWM1     =   0xF3;
sfr PCAPWM2     =   0xF4;
sfr PCAPWM3     =   0xF5;

sbit PCA_LED    =   P1^0;           //PCA test LED
//===========================================================================================
//===========================================================================================
/*=======================================================
显示字符串
=======================================================*/
void ePutstr(Uchar x,Uchar y, Uchar code *ptr) {
Uchar i,l=0;
    while (ptr[l] >31) {l++;};
    for (i=0;i<16;i++) { DispOneChar(x++,y,ptr[i]);
        if ( x == 16 ){
            x = 0; y ^= 1;
        }
    }
}

/*=======================================================
演示一行连续字符串，配合上位程序演示移动字串
=======================================================*/
void Display( Uchar dd ) {

Uchar i;

    for (i=0;i<16;i++) {
        DispOneChar(i,1,dd++);
        dd &= 0x7f;
        if (dd<32) dd=32;
    }
}

/*=======================================================
显示光标定位
=======================================================*/
void Disp_XY( char posx,char posy) {

uchar temp;

    temp = posx %40;// & 0x07;
    posy &= 0x01;
    if ( posy==1)  {temp |= 0x40;}
    temp |= 0x80;
    LcdWriteCommand(temp,1);
}

/*=======================================================
按指定位置显示数出一个字符
=======================================================*/
void DispOneChar(Uchar x,Uchar y,Uchar Wdata) {

    Disp_XY( x, y );                // 定位显示地址
    LcdWriteData( Wdata );            // 写字符
}

/*=======================================================
初始化程序, 必须按照产品资料介绍的初始化过程进行
=======================================================*/
void LcdReset( void ) {

       LcdWriteCommand( 0x38, 0);            // 显示模式设置(不检测忙信号)
        Delay5Ms();
    LcdWriteCommand( 0x38, 0);            // 共三次
        Delay5Ms();
    LcdWriteCommand( 0x38, 0);
        Delay5Ms();

    LcdWriteCommand( 0x38, 1);            // 显示模式设置(以后均检测忙信号)
    LcdWriteCommand( 0x08, 1);            // 显示关闭
    LcdWriteCommand( 0x01, 1);            // 显示清屏
    LcdWriteCommand( 0x06, 1);            // 显示光标移动设置
    LcdWriteCommand( 0x0c, 1);            // 显示开及光标设置
    LcdWriteCommand( 0x02, 1);            // 显示开及光标设置
}

/*=======================================================
写控制字符子程序: E=1 RS=0 RW=0
=======================================================*/
void LcdWriteCommand( Uchar CMD,Uchar AttribC ) {

    if (AttribC) WaitForEnable();                // 检测忙信号?

    RS = 0;    RW = 0; _nop_();

    DataPort = CMD; _nop_();
    // 送控制字子程序

    Elcm = 1;_nop_();_nop_();Elcm = 0;            // 操作允许脉冲信号
}

/*=======================================================
当前位置写字符子程序: E =1 RS=1 RW=0
=======================================================*/
void LcdWriteData( char dataW ) {

    WaitForEnable();
        // 检测忙信号

    RS = 1; RW = 0; _nop_();

    DataPort = dataW; _nop_();

    Elcm = 1; _nop_(); _nop_(); Elcm = 0;        // 操作允许脉冲信号

}

/*=======================================================
正常读写操作之前必须检测LCD控制器状态:    CS=1 RS=0 RW=1
DB7:    0  LCD控制器空闲; 1  LCD控制器忙
========================================================*/
void WaitForEnable( void ) {

    DataPort = 0xff;

    RS =0; RW = 1; _nop_();    Elcm = 1; _nop_(); _nop_();

    while( DataPort & Busy );

    Elcm = 0;
}


// 短延时
void Delay5Ms(void)
{
    Uint i = 5552;
    while(i--);
}

//长延时
void Delay400Ms(void)
{
    Uchar i = 5;
    Uint j;
    while(i--)
    {
        j=7269;
        while(j--);
    };
}

void delay(unsigned int t) //延时
{       
  	unsigned int i, j; 
  	for(i=0;i<t;i++)
	{ 
  		for(j=0;j<10;j++)
		{;} 
    } 
} 
//===========================================================================================
//===========================================================================================




//Time interrupt
void SystemInit(void){
TMOD = 0x21;//
TH0 = 0xfc; //1ms延时常数
TL0 = 0xff; //频率调节
ET0 = 1;
TR0 = 1;
TH1 = 0xfd; //波特率2400
TL1 = 0xfd;
TR1 = 1; //启动T1
SM0 = 0; //串口，工作方式一
SM1 = 1;
REN = 1; //允许接收
ES = 1; //串口中断允许
EA = 1; //开总中断

  TH0 = (65536 - 27650) / 256; // 定时50ms //晶振：11.0592MHZTL0 = (65536 - 45872) % 256;
  TL0=(65536-27650)%256;
  ET0 = 1;
  TR0 = 1;
  EA = 1;
  TIME_1_sec=0;
  TIME_1_min=0;
  TIME_2_sec=0;
  TIME_2_min=0;
  
      EA=1;//开总开关
	ES=1;//开串口中断
	//TMOD=0x22;//定义TM0D为00100010，为8位方式
	SCON=0x50;//01010000z
	TH1=0xFD;//定义波特率为9600
	TL1=0xFD;
	TR1=1;//开定时器中断
  }

void Timer0_IRQ(void) interrupt 1
{
	 TH0 = (655365 - 9217) / 256;//55300
	 TL0 = (655365 - 9217) % 256;/* 进入中断50ms +1次 */
	 TIME_BASE++;
	  if (cnt == 2 & flag == 1)
    {
        cnt = 0;                 
		ES=0;
		re = Adc(6);
		Send_OneByte(re/3);
		ES=1;
    }
	cnt=cnt+1;
 }
void PCA_isr() interrupt 6 using 2
{
   	CCF0 = 0;                       //Clear interrupt flag
    PCA_LED = !PCA_LED;             //toggle the test pin while CEX0(P3.7) have a falling edge
	//if(TI) TI=0;

	TIME_MS = TIME_BASE;//how many 50 ms

	TIME_BASE =0;
	
	if(time_number>=10){time_number=0;}

	TIME_queue[time_number++]=TIME_MS;	
}
void placeoneChar(unsigned char n){
	SBUF=n;//将缓冲中的数据写为n  23 - 17
	while(!TI);//正在写入数据中
	TI=0;//写入完毕后置TI为0
}
//=========================================
void Send_OneByte(char ch)
{
  SBUF=ch;
  while(!TI);
  TI=0;
}

void Uart_ini()
{
    EA=1;//开总开关
	ES=1;//开串口中断
	TMOD=0x22;//定义TM0D为00100010，为8位方式
	SCON=0x50;//01010000
	TH1=0xFD;//定义波特率为9600
	TL1=0xFD;
	TR1=1;//开定时器中断
}

uint Adc(unsigned char n)
{
	unsigned char i;	
	int result=0;

	ADC_CONTR = ADC_CONTR|0x80;
	delay(10);
	i = 0x01<<n;
    P1M0 = P1M0|i;
   	P1M1 = P1M1|i;
    delay(10); 
    ADC_CONTR = 0xE0|n;
    delay(10);
	ADC_DATA = 0;
	ADC_LOW2 = 0;
	ADC_CONTR = ADC_CONTR|0x08;
	delay(10);
    ADC_CONTR = ADC_CONTR&0xE7;
	result = ADC_DATA;
	result<<=2;
	result = result&0x03FC;
	result = result|(ADC_LOW2 & 0x03);
	return(result);
}

//=========================================
void wzh()
{
	unsigned int j;
	char *number1="Rate:";	
	unsigned char i;
	CCON = 0;                       //Initial PCA control register
                                    //PCA timer stop running
                                    //Clear CF flag
                                    //Clear all module interrupt flag
    CL = 0;                         //Reset PCA base timer
    CH = 0;
    CMOD = 0x00;                    //Set PCA timer clock source as Fosc/12
                                    //Disable PCA timer overflow interrupt
    CCAPM0 = 0x11;                  //PCA module-0 capture by a negative tigger on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x21;                  //PCA module-0 capture by a rising edge on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x31;                  //PCA module-0 capture by a transition (falling/rising edge) on CEX0(P3.7) and enable PCA interrupt

    CR = 1;                         //PCA timer start run
    EPCAI = 1;
    EA = 1;
	SystemInit(); LcdReset();
    while (1){
		
		
	time_numberx=0;time_sum=0;
	for(j=0;j<10;j++){
		if(TIME_queue[j]==0) continue;
		time_sum += TIME_queue[j];time_numberx++;
	}
	time_sum/=time_numberx;
	
	frequnce_of_heart = 6000 / time_sum; //
	
	time_day = frequnce_of_heart/10/10;
	time_hour = frequnce_of_heart/10%10;
	time_min = frequnce_of_heart%10;		
	
	//10->16 17->23
	
	time_bluetooth = frequnce_of_heart/10*16+frequnce_of_heart%10;
	if(frequnce_of_heart>99){time_bluetooth=153;} //
	
	
	for(i=0;i<5;i++)
	    {
	        DispOneChar(i,0,*(number1+i));
	    }
	
	
	DispOneChar(5,0,word_storeroom[time_day]);
	DispOneChar(6,0,word_storeroom[time_hour]);
	DispOneChar(7,0,word_storeroom[time_min]);
	Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
	    Delay400Ms();
		//蓝牙

		ES=0;//暂时关闭串口中断
		placeoneChar('R');	
		placeoneChar('a');
		placeoneChar('t');
		placeoneChar('e');
		placeoneChar(':');
		placeoneChar(word_storeroom[time_day]);
		placeoneChar(word_storeroom[time_hour]);
		placeoneChar(word_storeroom[time_min]);
		placeoneChar('\n');
		ES=1;//重新打开串口中断

	for(i=0;i<16;i++)
    {
        DispOneChar(i,0,' ');
        DispOneChar(i,1,' ');
    }
	


	}
	
	
}
void ljn()
{
	char *number1="Seen by computer";
	char *number2="  By HelloWord  ";
	SystemInit(); 
	LcdReset();
	Delay400Ms();
	n=0;
	cnt=0;
	CCON = 0;                       //Initial PCA control register
                                    //PCA timer stop running
                                    //Clear CF flag
                                    //Clear all module interrupt flag
    CL = 0;                         //Reset PCA base timer
    CH = 0;
    CMOD = 0x00;                    //Set PCA timer clock source as Fosc/12
                                    //Disable PCA timer overflow interrupt
    CCAPM0 = 0x11;                  //PCA module-0 capture by a negative tigger on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x21;                  //PCA module-0 capture by a rising edge on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x31;                  //PCA module-0 capture by a transition (falling/rising edge) on CEX0(P3.7) and enable PCA interrupt

    CR = 1;                         //PCA timer start run
    EPCAI = 1;
    EA = 1;
//	P1M0 = 0xff;
//	P1M1 = 0x00;
//  P3M0 = 0xfd;
//	P3M1 = 0x01;
	for(i=0;i<16;i++)
	    {
	        DispOneChar(i,0,*(number1+i));
	    }
	for(i=0;i<16;i++)
	    {
	        DispOneChar(i,1,*(number2+i));
	    }
    while (1);
	
	
}
void main()
{
    char *number1="  Heart Health  ";
	char *number2="  By HelloWord  ";
	Uart_ini();
	LcdReset();
	Delay400Ms();
	n=0;
	flag = 0;
	CCON = 0;                       //Initial PCA control register
                                    //PCA timer stop running
                                    //Clear CF flag
                                    //Clear all module interrupt flag
    CL = 0;                         //Reset PCA base timer
    CH = 0;
    CMOD = 0x00;                    //Set PCA timer clock source as Fosc/12
                                    //Disable PCA timer overflow interrupt
    CCAPM0 = 0x11;                  //PCA module-0 capture by a negative tigger on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x21;                  //PCA module-0 capture by a rising edge on CEX0(P3.7) and enable PCA interrupt
//  CCAPM0 = 0x31;                  //PCA module-0 capture by a transition (falling/rising edge) on CEX0(P3.7) and enable PCA interrupt

    CR = 1;                         //PCA timer start run
    EPCAI = 1;
    EA = 1;
//	P1M0 = 0xff;
//	P1M1 = 0x00;
//  P3M0 = 0xfd;
//	P3M1 = 0x01;
	for(i=0;i<16;i++)
	    {
	        DispOneChar(i,0,*(number1+i));
	    }
	for(i=0;i<16;i++)
	    {
	        DispOneChar(i,1,*(number2+i));
	    }
	
	
	
	
	while(1){
		if(button1==0){
			Delay400Ms();
			if(button1==0){
				flag=1;
			}
		}
		if(button2==0){
			Delay400Ms();
			if(button2==0){
				flag=2;
			}
		}
		
		if(flag==1)	ljn();
		else if(flag==2) wzh();

	}
}

