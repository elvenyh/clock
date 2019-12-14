//////////////////////////////////////////////////////////////////////////////////	 
//  文 件 名   : main.c
//  版 本 号   : v1.0
//  作    者   : yh
//  生成日期   : 
//  最近修改   : 
//  功能描述   : 基于GPS的时钟，速度计
//调试记录：
//带GPS模块224MA（5V）  不带GPS模块196MA
//******************************************************************************/

#define NOP _nop_()
#define nop _nop_()
#define NOP4 _nop_();_nop_();_nop_();_nop_()
#define		ulong	unsigned long
#define		uint	unsigned int
#define		uchar	unsigned char

#include "intrins.h"
#include "STC15L2K61S2.H"

#define ADC_POWER   0x80            //ADC电源控制位
#define ADC_FLAG    0x10            //ADC完成标志
#define ADC_START   0x08            //ADC起始控制位
#define ADC_SPEEDLL 0x00            //540个时钟
#define ADC_SPEEDL  0x20            //360个时钟
#define ADC_SPEEDH  0x40            //180个时钟
#define ADC_SPEEDHH 0x60            //90个时钟


//#include "REG51.h"
//#include "oled.h"
//#include "bmp.h"
//#include "18b20.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

//#define Time_Zone_8  8 //东八区，天朝北京，小菲马尼拉 

#define S1_S0 0x40              //P_SW1.6
#define S1_S1 0x80              //P_SW1.7

#define FOSC 11059200L      //System frequency
#define BAUD 9600           //UART baudrate

/*Define UART parity mode*/
#define NONE_PARITY     0   //None parity
#define ODD_PARITY      1   //Odd parity
#define EVEN_PARITY     2   //Even parity
#define MARK_PARITY     3   //Mark parity
#define SPACE_PARITY    4   //Space parity

//#define PARITYBIT EVEN_PARITY   //Testing even parity
#define PARITYBIT NONE_PARITY

bit busy;

//GPS数据
uchar date[6]; //日期
uchar time[6]; //时间
//串口中断需要的变量
uchar seg_count; //逗号计数器
uchar cmd_count; //前导位数计数器
uchar data_count; //data counter
uchar lock=0; //是否定位 (sbuf_tmp=='A') 
/*
uchar Lat[10];//={0x30+1,0x30+2,0x30+3,0x30+4,0x4D,0x30+5,0x30+6,0x30+7,0x30+8,0x30+9};//="3957.99358"; //纬度ddmm.mmmmm，度分格式（前导位数不足则补0）
bit Lat_sign;  //1==纬度N（北纬）或0==S（南纬）
uchar Lng[11];//={0x30+5,0x30+6,0x30+7,0x30+8,0x30+9,0x43,0x30+8,0x30+7,0x30+1,0x30+2,0x30+3};//="11622.87419"; //经度dddmm.mmmmm，度分格式（前导位数不足则补0）
bit Lng_sign;//1==经度E（东经）或0==W（西经）
uchar lat_d,lat_m,lat_m_l,lng_d,lng_m,lng_m_l;
*/
//uchar speed[6]={0x30+0,46,0x30+2,0x30+6,0x30+1,0x30+7};//速度信息  最大99.999节
uchar speed[6]={0x30+9,0x30+9,46,0x30+8,0x30+6,0x30+1};//速度信息  最大99.999节

bit frame_flag;

unsigned long speed_km;
uchar speed_km_h,speed_km_l;
//unsigned int speed_km_l;

uchar GRPMC[6]={0x24,0x47,0x50,0x52,0x4D,0x43};    //GPS type $GPRMC,

uchar year=0,mon=0,day=0,hour=0,min=0,sec=0;

//brightness  1(low)-->2-->3-->4-->5-->6-->7-->0(high)

/*
修改数据输出频率
10Hz: B5 62 06 08 06 00 64 00 01 00 01 00 7A 12
5HZ:  B5 62 06 08 06 00 C8 00 01 00 01 00  DE 6A
1HZ:  B5 62 06 08 06 00 E8 03 01 00 01 00  01 39
0.1Hz:B5 62 06 08 06 00 10 27 01 00 01 00 4D DD
*/

//////////////////////////////////////////////////////////////////////////////////////////////////////

//ds3231+24c64
sbit SCL=P1^7; //串行时钟
sbit SDA=P1^6; //串行数据

sbit key_1 =P1^1; 
//sbit key_2 =P1^3; 

sbit r_light =P1^3;  //reg for light AD

//sbit mic_dig_out  =P1^6; //mic phone

//tm1650
sbit SCL_TM1650=P1^5;//时钟线
sbit SDA_TM1650=P1^4;//数据线

uchar TM1650_bright=0;//0/1/..../7 8级亮度/一级亮度/...../七级亮度
uchar mic_count=0;

#include "DS3231+AT24C32.h"
#include "gps_data_process.h"
//#include "display_595.h"
#include "display_tm1650.h"


/*
void GetTime_3231();//get time form ds3231
void GetTime_GPS(); //get tiem form gps
void Calibration_Time()//write gps time to ds3231
void GetSpeed_GPS(); //get speed form gps
void GetLatLng_GPS();//get Lat&Lng form gps


void Delay500ms()		//@11.0592MHz
void Delay10ms()		//@11.0592MHz

*/

void InitADC();
void SendData(BYTE dat);
BYTE GetADCResult(BYTE ch);

////////////////////////////////////////////////////////////////////////////////////////////////////////
void UartInit(void)		//9600bps@11.0592MHz
{
    P0M0 = 0x00;
    P0M1 = 0x00;
    P1M0 = 0x00;
    P1M1 = 0x00;
    P2M0 = 0x00;
    P2M1 = 0x00;
    P3M0 = 0x00;
    P3M1 = 0x00;
    P4M0 = 0x00;
    P4M1 = 0x00;
    P5M0 = 0x00;
    P5M1 = 0x00;
    P6M0 = 0x00;
    P6M1 = 0x00;
    P7M0 = 0x00;
    P7M1 = 0x00;
    
    ACC = P_SW1;
    ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=0
    P_SW1 = ACC;                //(P3.0/RxD, P3.1/TxD)
    
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=1 S1_S1=0
//  ACC |= S1_S0;               //(P3.6/RxD_2, P3.7/TxD_2)
//  P_SW1 = ACC;  
//  
//  ACC = P_SW1;
//  ACC &= ~(S1_S0 | S1_S1);    //S1_S0=0 S1_S1=1
//  ACC |= S1_S1;               //(P1.6/RxD_3, P1.7/TxD_3)
//  P_SW1 = ACC;  

#if (PARITYBIT == NONE_PARITY)
    SCON = 0x50;                //8位可变波特率
#elif (PARITYBIT == ODD_PARITY) || (PARITYBIT == EVEN_PARITY) || (PARITYBIT == MARK_PARITY)
    SCON = 0xda;                //9位可变波特率,校验位初始为1
#elif (PARITYBIT == SPACE_PARITY)
    SCON = 0xd2;                //9位可变波特率,校验位初始为0
#endif

    T2L = (65536 - (FOSC/4/BAUD));   //设置波特率重装值
    T2H = (65536 - (FOSC/4/BAUD))>>8;
    AUXR = 0x14;                //T2为1T模式, 并启动定时器2
    AUXR |= 0x01;               //选择定时器2为串口1的波特率发生器
    ES = 1;                     //使能串口1中断
    EA = 1;

}

/*----------------------------
发送串口数据
----------------------------*/
/*
void SendData(BYTE dat)
{
    while (busy);               //等待前面的数据发送完成
    ACC = dat;                  //获取校验位P (PSW.0)
    if (P)                      //根据P来设置校验位
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 0;                //设置校验位为0
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 1;                //设置校验位为1
#endif
    }
    else
    {
#if (PARITYBIT == ODD_PARITY)
        TB8 = 1;                //设置校验位为1
#elif (PARITYBIT == EVEN_PARITY)
        TB8 = 0;                //设置校验位为0
#endif
    }
    busy = 1;
    SBUF = ACC;                 //写数据到UART数据寄存器
}
*/


/*----------------------------
发送串口数据
----------------------------*/
void SendData(BYTE dat)
{
    while (!TI);                    //等待前一个数据发送完成
    TI = 0;                         //清除发送标志
    SBUF = dat;                     //发送当前数据
}

/*----------------------------
发送字符串
----------------------------*/
void SendString(char *s)
{
    while (*s)                  //检测字符串结束标志
    {
        SendData(*s++);         //发送当前字符
    }
}
//串口接收中断
void Uart() interrupt 4 using 1
{
    uchar sbuf_tmp;
	  ES = 0;    //关闭串口中断
    if(RI)
		{
			RI=0;
      sbuf_tmp=SBUF;
        //数据转发
        //SBUF=tmp;
        //if(TI)TI=0;
		  //$GPRMC,121010.00,A,3957.99282,N,11622.87611,E,0.317,,160116,,,A*78		
			//接收GPS信息
			if(cmd_count<6)
			{
				if(sbuf_tmp == GRPMC[cmd_count])//比较$GPRMC
					cmd_count++;
				else
					cmd_count=0;
			}
			else //receive gps data
			{
				
				if(sbuf_tmp==',')//,计数器
				{
					seg_count++;
					data_count=0;
				}
				else
				{
					if(seg_count==1)  //time hh  mm  ss
					{
						if(data_count<6) //save time
							time[data_count++]=sbuf_tmp;
						else;
					}
					else if(seg_count==2)  //ok  
					{
					  if (sbuf_tmp=='A') 
							lock=1;
             else
							lock=0;
					}
					/*
					else if(seg_count==3) //纬度 Latitude 简写Lat
					{
					if(lock) //lock ok
						if(data_count<10) //save Latitude
							Lat[data_count++]=sbuf_tmp;
						else;
					else;
					}
					else if(seg_count==4) //纬度N（北纬）或S（南纬）
					{
					if(lock) //lock ok
					{
						if(sbuf_tmp=='N')
							Lat_sign=1;
						else 
							Lat_sign=0;
					}
					else;
					}
					else if(seg_count==5) //经度dddmm.mmmm，度分格式（前导位数不足则补0）
					{
					if(lock) //lock ok 
						if(data_count<11) //save Longitude
							Lng[data_count++]=sbuf_tmp;//经度 Longitude 简写Lng
						else;
					else;
					}
					else if(seg_count==6) //经度E（东经）或W（西经）
					{
					if(lock) //lock ok
					{
						if(sbuf_tmp=='E')
							Lng_sign=1;
						else 
							Lng_sign=0;
					}
					else;
					}
					*/
					else if(seg_count==7) //速度，节，Knots
					{
					if(lock) //lock ok
					{
						if(data_count<6) //save speed
							speed[data_count++]=sbuf_tmp;
						else;
					}
					else;
					}
					
					else if(seg_count==9) //dd mm yy
						date[data_count++]=sbuf_tmp;
					else if(seg_count>10) //reveive over
					{
						cmd_count=0;
						data_count=0;
						seg_count=0;
						frame_flag=~frame_flag;
          }
					else;
				}
			}//end receive data
		} //end RI process
		ES = 1;  //使能串口中断
} //end process		

void Delay500ms()		//@11.0592MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 22;
	j = 3;
	k = 227;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}

/*----------------------------
发送ADC结果到PC
----------------------------*/
void ShowResult(BYTE ch)
{
    SendData(ch);                   //显示通道号
    SendData(GetADCResult(ch));     //显示ADC高8位结果

//    SendData(ADC_LOW2);           //显示低2位结果
} 

/*----------------------------
读取ADC结果
----------------------------*/
BYTE GetADCResult(BYTE ch)
{
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL | ch | ADC_START;
    _nop_();                        //等待4个NOP
    _nop_();
    _nop_();
    _nop_();
    while (!(ADC_CONTR & ADC_FLAG));//等待ADC转换完成
    ADC_CONTR &= ~ADC_FLAG;         //Close ADC

    return ADC_RES;                 //返回ADC结果
}

/*----------------------------
初始化ADC
----------------------------*/
void InitADC()
{
    P1ASF = 0xff;                   //设置P10口为AD口
    ADC_RES = 0;                    //清除结果寄存器
    ADC_CONTR = ADC_POWER | ADC_SPEEDLL;
    Delay500ms();                   //ADC上电并延时
}

//////////////////////////////////////////////////////////////////////////////////////
void main(void)
 {uchar t,key_value_1=1;
	static bit key_1_sta;//,mic_dig_out_sta;
	TM1650_bright=0;//brightness high
	UartInit();			 //串口初始化
	//InitADC();                      //初始化ADC
	
	for(t=0;t<18;t++)
	{
		//TM1650_bright=t;//brightness high
		//disp_TM1650_test();Delay500ms();
		/*
        ShowResult(0); Delay500ms();             //显示通道0
        ShowResult(1); Delay500ms();             //显示通道1
        ShowResult(2); Delay500ms();             //显示通道2
        ShowResult(3); Delay500ms();             //显示通道3
        ShowResult(4); Delay500ms();             //显示通道4
        ShowResult(5); Delay500ms();             //显示通道5
        ShowResult(6); Delay500ms();             //显示通道6
        ShowResult(7); Delay500ms();             //显示通道7
		*/
		TM1650_send(0x48,0x00);
    }
/*		
	for(t=0;t<8;t++)
	{
		TM1650_bright=t;//brightness high
		disp_TM1650_test();Delay500ms();
		TM1650_send(0x48,0x00);
	}
*/

		
	while(1) 
	{
			if(lock) //lock ok
			{
				GetTime_GPS(); //get tiem form gps
				Calibration_Time();//write gps time to ds3231
				GetSpeed_GPS(); 
				//mic_dig_out_sta=mic_dig_out;
			}
			else
			{
				//mic_dig_out_sta=mic_dig_out;
				NOP;
			}
			
				key_1_sta=key_1;//mic_dig_out_sta=mic_dig_out;
				GetTime_3231();//get time form ds3231
				if(key_value_1==0)			disp_TM1650_m_s();
				else if(key_value_1==1)	disp_TM1650_h_m();
				else if(key_value_1==2)	disp_TM1650_date();
				else if(key_value_1==3)	disp_TM1650_year(); 
				else if(key_value_1==4)	disp_TM1650_test();
				else if(key_value_1==5)	{GetSpeed_GPS();disp_TM1650_speed();}
				//else if(key_value_1==6)	disp_TM1650_speed();
				else disp_TM1650_h_m();
			
	/*
				if(mic_dig_out==1 && mic_dig_out_sta==0) //release
				{
					if(mic_count<100)
						mic_count++;
					else
						mic_count=0;
				}
	*/			
				//brightness  1(low)-->2-->3-->4-->5-->6-->7-->0(high)
				if(hour<6 || hour>22)
					TM1650_bright=1;//brightness lowest
				else
					TM1650_bright=5;//brightness
				
				if(key_1==1 && key_1_sta==0) //release
				{
					if(key_value_1<6)
						key_value_1++;
					else 
						key_value_1=0;
				}
	}	
}		
	
	
