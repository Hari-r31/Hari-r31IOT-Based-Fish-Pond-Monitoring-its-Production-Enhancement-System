float calibration_value = 20.50;
int phval = 0; 
unsigned long int avgval; 
int buffer_arr[10],temp;

float pHvalue()
{
  for(int i=0;i<10;i++) 
 { 
 buffer_arr[i]=analogRead(36);
 delay(30);
 }
 for(int i=0;i<9;i++)
 {
 for(int j=i+1;j<10;j++)
 {
 if(buffer_arr[i]>buffer_arr[j])
 {
 temp=buffer_arr[i];
 buffer_arr[i]=buffer_arr[j];
 buffer_arr[j]=temp;
 }
 }
 }
 avgval=0;
 for(int i=2;i<8;i++)
 avgval+=buffer_arr[i];
 float volt=(float)avgval*3.3/4096/6;
 float ph_act = -5.70 * volt + calibration_value;
 return ph_act/2;
 
}
