//����tips������8����ʾģʽ��ʵ�ֶԸ��εĿ��ƣ��߶�ģʽʱ��С���㶼��ʾ


//**********8�����ȵ���***********************************
//uchar liangdu[]={0x19,0x29,0x39,0x49,0x59,0x69,0x79,0x09};//1-8�����ȵ���
//*******************************************************
//uchar shuma[]={0x00,0xF9,0xA4,0xB0,0x99,0x92,0x82,0xF8,0x80,0x90,0x88,0x83,0xC6,0xA1,0x86,0x8E};//������0-f
//************�����������ʾ0-F************************
// uchar display[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0xFF,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71};//������
/**************�����������ʾ0-F,��������-��**********************/
uchar display_TM1650[]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x77,0x7C,0x39,0x5E,0x79,0x71,0x00,0x40}; //�������ֶ���

//hgfedcba

/************ START�ź�*******************************/
void TM1650_START()
{
	SCL_TM1650=1;
	SDA_TM1650=1;
	NOP4;
	SDA_TM1650=0;
	NOP4;
	SCL_TM1650=0;
}
/******************** STOP�ź�************************/
void TM1650_STOP()
{
	SDA_TM1650=0;
	NOP4;
	SCL_TM1650=1;
	NOP4;
	SDA_TM1650=1;
	NOP4;
	SCL_TM1650=0;
	SDA_TM1650=0;
}
/****************д1���ֽڸ�TM1650********************/
void TM1650_write_8bit( uchar dat)
{
 	uchar i;
	SCL_TM1650=0;
	for(i=0;i<8;i++)
		{
		if(dat&0x80)
		{
			SDA_TM1650=1;
			NOP4;
			SCL_TM1650=1;
			NOP4;
			SCL_TM1650=0;	 
		}
		else
		{
			SDA_TM1650=0;
			NOP4;
			SCL_TM1650=1;
			NOP4;
			SCL_TM1650=0;
		}	
			dat<<=1;	 
		}
		SDA_TM1650=1;			//ACK�ź�
		NOP4;
		SCL_TM1650=1;
		NOP4;
		SCL_TM1650=0;
		NOP4;	 
}

/**********************��8bit**************************/
uchar TM1650_read_8bit()
{
	uchar dat,i;
	SDA_TM1650=1;
	dat=0;
	for(i=0;i<8;i++)
	{
	SCL_TM1650=1;                        //ʱ������
	NOP4;
	dat<<=1;
	if(SDA_TM1650)
	 dat++;
	SCL_TM1650=0;
	NOP4;
	}
	SDA_TM1650=0;			    //ACK�ź�
	NOP4;
	SCL_TM1650=1;
	NOP4;
	SCL_TM1650=0;
	NOP4;
	
	return dat ;

} 
/*******************����������************************/
uchar TM1650_read()
{
	uchar key;
	TM1650_START();
  TM1650_write_8bit(0x49);//������ָ��	
	key=TM1650_read_8bit();
	TM1650_STOP();
	return key;
} 
/*****************���������ź�***********************/
void TM1650_send(uchar date1,uchar date2)
{
 	TM1650_START();
	TM1650_write_8bit(date1);
	TM1650_write_8bit(date2);
	TM1650_STOP();
}

/*****************��ʾ����***********************/
void disp_TM1650_clr()
{
	TM1650_send(0x48,0); 
}	

void disp_TM1650_test()
{
	TM1650_send(0x48, (TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1������
	//TM1650_send(0x48,0x09);// ������ʾ��1������
	TM1650_send(0X68,0x49);  //GID1
	TM1650_send(0X6A,display_TM1650[TM1650_bright]);  //GID2
	TM1650_send(0X6C,display_TM1650[TM1650_bright]);  //GID3
	TM1650_send(0X6E,0x49);  //GID4
}
void disp_TM1650_mic_count()
{
	TM1650_send(0x48, (TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1������
	//TM1650_send(0x48,0x09);// ������ʾ��1������
	TM1650_send(0X68,display_TM1650[mic_count/10]);  //GID1
	TM1650_send(0X6A,display_TM1650[mic_count%10]);  //GID2
	TM1650_send(0X6C,display_TM1650[mic_count/10]);  //GID3
	TM1650_send(0X6E,display_TM1650[mic_count%10]);  //GID4
}

void disp_TM1650_m_s()
{
	TM1650_send(0x48,(TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1������
	TM1650_send(0X68,display_TM1650[min/10]);  //GID1
	TM1650_send(0X6A,display_TM1650[min%10]);  //GID2
	TM1650_send(0X6C,display_TM1650[sec/10]);  //GID3
	TM1650_send(0X6E,display_TM1650[sec%10]);  //GID4
}

void disp_TM1650_h_m()
{
	TM1650_send(0x48,(TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1������
	if(hour/10==0)
		TM1650_send(0X68,display_TM1650[16]);  //GID1
	else
		TM1650_send(0X68,display_TM1650[hour/10]);  //GID1
	TM1650_send(0X6A,display_TM1650[hour%10]);  //GID2
	TM1650_send(0X6C,display_TM1650[min/10]);  //GID3
	if(sec%2)
		TM1650_send(0X6E,display_TM1650[min%10]);  //GID4
	else
		TM1650_send(0X6E,display_TM1650[min%10]|0x80);  //GID4
}

void disp_TM1650_date()
{
	TM1650_send(0x48,(TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1����
	
	if(mon/10==0)
		TM1650_send(0X68,display_TM1650[16]);  //GID1
	else
	if(mon/10==0)
		TM1650_send(0X68,display_TM1650[16]);  //GID1
	else
		TM1650_send(0X68,display_TM1650[mon/10]);  //GID1
	
	TM1650_send(0X6A,display_TM1650[mon %10]);  //GID2
	TM1650_send(0X6C,display_TM1650[day/10]);  //GID3
	TM1650_send(0X6E,display_TM1650[day%10]);  //GID4
}

void disp_TM1650_year()
{
	TM1650_send(0x48,(TM1650_bright*16+0x01));                     // ������ʾģʽ��7����ʾ��1����
	TM1650_send(0X68,display_TM1650[2]);  //GID1
	TM1650_send(0X6A,display_TM1650[0]);  //GID2
	TM1650_send(0X6C,display_TM1650[year/10]);  //GID3
	TM1650_send(0X6E,display_TM1650[year%10]);  //GID4
}

//uchar speed_km_h,speed_km_l;//hΪ�����ٶ�ֵ��lΪС������ֵ
void disp_TM1650_speed()
{
	TM1650_send(0x48,(TM1650_bright*16+0x01));                     // ������ʾģʽ��8����ʾ��1������
	if(speed_km_h<100)
	{
		if(speed_km_h/10==0)
			TM1650_send(0X68,display_TM1650[16]);  //GID1
		else
			TM1650_send(0X68,display_TM1650[speed_km_h/10]);  //GID1
		TM1650_send(0X6A,display_TM1650[speed_km_h%10]|0x80);  //GID2
		TM1650_send(0X6C,display_TM1650[speed_km_l/10]);  //GID3
		TM1650_send(0X6E,display_TM1650[speed_km_l%10]);  //GID4
	}
	else //>100Km/h
	{
		TM1650_send(0X68,display_TM1650[speed_km_h/100]);  //GID1
		TM1650_send(0X6A,display_TM1650[(speed_km_h-(speed_km_h/100)*100)/10]);  //GID2
		TM1650_send(0X6C,display_TM1650[speed_km_h%10]|0x80);  //GID3
		TM1650_send(0X6E,display_TM1650[speed_km_l/10]);  //GID4
	}		
}


