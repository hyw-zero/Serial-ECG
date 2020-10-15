#include "dataprocess.h"

float temprature_process(unsigned char *buff)
{
    int adc;
    float temp_adc,V_TAO,T;
    adc=(int)(buff[0]<<8)+(buff[1]);
    double a=-1.064200E-09;
    double b=-5.759725E-06;
    double c=-1.789883E-01;
    double d=2.048570E+02;
    temp_adc=(float)adc*(3.3/4096);
    adc=temp_adc;
    temp_adc-=adc;
    V_TAO=temp_adc*1000;
    T=a*V_TAO*V_TAO*V_TAO+b*V_TAO*V_TAO+c*V_TAO+d;
    return T;
}
long step_process(unsigned char *buff)
{
    return (long)((buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+buff[3]);
}

int peak_buff[300]={0};
int peak_max,peak_max2=0;
int peak_index1=0;
int peak_index2=0;
int peak_index1_valid=0;
int peak_index2_valid=0;
int peak_temp=0;
float hear_beat=0;


float heart_rate_process(unsigned char *buff)
{
#if 1
    unsigned int adc=(unsigned int)((buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+buff[3]);
    unsigned int dc=(unsigned int)((buff[4]<<24)+(buff[5]<<16)+(buff[6]<<8)+buff[7]);

    int temp;
    temp=adc;
    temp<<=8;
    temp>>=8;
    int temp1;
    temp1=dc;
    temp1<<=8;
    temp1>>=8;
    peak_buff[peak_temp]=(int)(temp-temp1);

    int index_r,index_l,index_m=0;
    index_r = peak_temp;
    index_l = peak_temp - 2;
    index_m = peak_temp - 1;

    float thresh=5000;

    if((((peak_buff[index_m]-peak_buff[index_r])> thresh) && ((peak_buff[index_m]-peak_buff[index_l])> thresh))
        ||(((peak_buff[index_m]-peak_buff[index_r])<- thresh) && ((peak_buff[index_m]-peak_buff[index_l])<-thresh) )
      )
    {

            if(!peak_index1_valid)
            {
                peak_index1 = index_m;
                peak_index1_valid = 1;
                qDebug()<<"find index1111 success:"<<peak_index1;
            }
            else
            {
                peak_index2 = index_m;
                peak_index2_valid = 1;
                qDebug()<<"find index1111 success:"<<peak_index2;
            }

            if(peak_index1_valid && peak_index2_valid)
            {
                float hear_beat_temp=(float)(60.0*100.0/abs((peak_index2-peak_index1)));
                qDebug()<<"hear_beat_temp:"<<hear_beat_temp;
                if((hear_beat_temp<180 ) && (hear_beat_temp > 50) )
                {
                    peak_index1=0;
                    peak_index2=0;
                    peak_index1_valid=0;
                    peak_index2_valid=0;
                    peak_temp = 0;
                    hear_beat = hear_beat_temp;
                    return hear_beat;
                }

            }

    }
    if(peak_temp == 299)
    {
        peak_index1=0;
        peak_index2=0;
        peak_index1_valid=0;
        peak_index2_valid=0;
        peak_temp = 0;
    }


    peak_temp++;

    return hear_beat;

#endif
#if 0
    unsigned int adc=(unsigned int)((buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+buff[3]);
    unsigned int dc=(unsigned int)((buff[4]<<24)+(buff[5]<<16)+(buff[6]<<8)+buff[7]);

    int temp;
    temp=adc;
    temp<<=8;
    temp>>=8;
    int temp1;
    temp1=dc;
    temp1<<=8;
    temp1>>=8;
    peak_buff[peak_temp]=(int)abs(temp-temp1);
    peak_temp++;

    int thres1 = 2900;

  //  qDebug()<<"peak_buff "<<peak_buff[peak_temp-1]<<"  thres1"<<thres1;

    int p1,p2;
    int pnum1,pnum2=0;
    peak_index1_valid=0;
    peak_index2_valid=0;
    if(peak_temp >= 199) {

        for(int x=0;x<peak_temp-10;x++){
            if( peak_buff[x] > thres1)
            {
                qDebug()<<"into thres1";
                p1 = x;
                p2 = x +10;
                pnum1=0;
                peak_max = peak_buff[x];
                for(int i=p1;i<p2;i++)
                {
                    if(peak_max < peak_buff[i])
                    {
                        peak_max=peak_buff[i];
                        if(!peak_index1_valid)
                        {
                            peak_index1=i;
                        }
                        else
                        {
                            peak_index2=i;
                        }


                    }
                    if(peak_buff[i]>thres1)
                    {
                        pnum1++;
                        if(pnum1>5)
                        {
                            if(!peak_index1_valid)
                            {
                                peak_index1_valid=1;
                                 qDebug()<<"find index1111 success:"<<peak_index1;
                                 break;
                            }
                            else
                            {
                                peak_index2_valid=1;
                                qDebug()<<"find index222 success:"<<peak_index2;
                                break;
                            }

                         }
                    }
                }
                x=p2;
            }
            if(peak_index1_valid && peak_index2_valid)
            {
                hear_beat=(float)(60.0*100.0/abs((peak_index2-peak_index1)));
                qDebug()<<"hear_beat:"<<hear_beat;
                peak_index1=0;
                peak_index2=0;
                peak_index1_valid=0;
                peak_index2_valid=0;
                return hear_beat;
            }

        }
        peak_temp = 0;
    }


    return hear_beat;
#endif
#if 0
    int i=0;
    //Todo
    if(peak_temp == 199) {

        for(i=0;i<99;i++)
        {
            if(peak_max <= peak_buff[i])
            {
                peak_index1=i;
            }
            if(peak_max2 <= peak_buff[i+100])
            {
                peak_index2=i+100;
            }
        }

        hear_beat=(float)(60.0*100.0/(peak_index2-peak_index1));

        peak_temp = 0;
    }
    peak_buff[peak_temp]=(int)((buff[0]<<24)+(buff[1]<<16)+(buff[2]<<8)+buff[3]);
    peak_temp++;

    return hear_beat;
#endif
}
