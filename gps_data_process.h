
void GetTime_GPS()
{
	uchar max;
		hour=(time[0]-0x30)*10+time[1]-0x30;  
		min=(time[2]-0x30)*10+time[3]-0x30;	
		sec=(time[4]-0x30)*10+time[5]-0x30;
					
		day=(date[0]-0x30)*10+date[1]-0x30;
		mon=(date[2]-0x30)*10+date[3]-0x30;
		year=(date[4]-0x30)*10+date[5]-0x30;
			
			if(sec>59)
			{
			 	sec=0;
				min+=1;
				if(min>59)
				{
				 min=0;
				 hour+=1;
				}
				else;
			}
			else;
			
			hour+=8;			//转换为北京时间 ，东8区要+8
			if(hour>23)		//如果超过23则到了第二天
			{   
				hour-=24;	//
				day+=1;		//日+1
				switch(mon)		//获取每月最大天数
				{
				 	case 2:			 //2月
						if(year%4==0)  //判断是否闰月，只取2000-2099年，因此简单的%4运算
							max=29;		  //闰月29天
						else
							max=28;		  //正常28天
						break;
					case 4:	case 6:	 
					case 9: case 11:     //4，6，9，11月					
						max=30;			 //30天
						break;
					default:
						max=31;		  //其他月份1，3，5，7，8，10，12月31天
					break;		
				}
				if(day>max)
				{
					mon+=1;
					day=1;
					if(mon>12)
					{
					 	year+=1;
						mon=1;
					}
				}
			}		
}


void GetSpeed_GPS()
{
//$GPVTG,,T,,M,0.261,N,0.484,K,A*2E
//$GPRMC,134655.00,A,3858.01092,N,10622.87193,E,1.188,,270216,,,A*7F
//speed conver
//0.00---99.99==>*1.852
if(speed[1]=='.')//x.xxx
	
	speed_km=(speed[0]-0x30)*1000+(speed[2]-0x30)*100+(speed[3]-0x30)*10+(speed[4]-0x30);
else if	(speed[2]=='.')//xx.xx
{
	//speed_km=(speed[0]-0x30)*10000+(speed[1]-0x30)*1000+(speed[3]-0x30)*100+(speed[4]-0x30)*10+(speed[5]-0x30);
	speed_km=(speed[0]-0x30);
	speed_km=speed_km*10000;
	speed_km=speed_km+(speed[1]-0x30)*1000+(speed[3]-0x30)*100+(speed[4]-0x30)*10+(speed[5]-0x30);
}
else	speed_km=999999;

	speed_km=(speed_km*1.852+5)/10; //convert km/h
	if(speed_km/10000!=0)
		speed_km=speed_km+5;  //>100km/h  4she5ru process
	else;
	
	speed_km_h=speed_km/100;
	speed_km_l=speed_km-speed_km_h*100;			
}
/*test code
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,0.005,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,0.055,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,0.555,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,5.555,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,45.555,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,55.555,,270216,,,A*7E
$GPRMC,123638.00,A,957.98958,N,1622.87433,E,95.555,,270216,,,A*7E
*/

/*
void GetLatLng_GPS(void)
{			
	////纬度ddmm.mmmmm，度分格式（前导位数不足则补0）//经度dddmm.mmmmm，度分格式（前导位数不足则补0）
	//3957.98949,N,11622.88084,E
	lat_d=(Lat[0]-0x30)*10+(Lat[1]-0x30);
	lat_m=(Lat[2]-0x30)*10+(Lat[3]-0x30);
	lat_m_l=(Lat[5]-0x30)*10+(Lat[6]-0x30);//纬度ddmm.mmmmm
	
	lng_d=(Lng[0]-0x30)*100+(Lng[1]-0x30)*10+(Lng[2]-0x30);
	lng_m=(Lng[3]-0x30)*10+(Lng[4]-0x30);
	lng_m_l=(Lng[6]-0x30)*10+(Lng[7]-0x30);//经度dddmm.mmmmm
}
*/