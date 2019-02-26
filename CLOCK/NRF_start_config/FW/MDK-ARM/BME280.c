
#include "BME280.h"
#include "stm32f1xx_hal.h"
#include "stm32f1xx.h"
#include "stm32f1xx_it.h"
#include "main.h"

#define ON 1
#define OFF 0
#define DEBUG_INFORMATION ON

extern UART_HandleTypeDef huart1;
extern I2C_HandleTypeDef hi2c1;
float calulete_temperature(uint32_t adc_T);
extern float cTemp;
extern int32_t T;

/////////////////////////////////

void init_BME280(void)
{
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_ID_ADDRES, (uint16_t) 1, &buff_id, (uint16_t)1, 1000);
		if((STATUS_BME280!=HAL_OK)|(buff_id!=0x60))
		{
					while(1)
					{
						    if(STATUS_BME280!=HAL_OK)
								{
											sprintf(str_BME280,"ERROR CONNECT TO BME280 !!! STATUS!=HAL_OK\r\n");      // convert   in  str 
											size=sizeof(str_BME280);
											HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 	

											HAL_Delay(1000);		
								}	
								if(buff_id!=0x60)
								{
											sprintf(str_BME280,"BME280 not found !!!\r\n");      // convert   in  str 
											size=sizeof(str_BME280);
											HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 	

											HAL_Delay(1000);		
								}									
					}
		}
		else
		{
				sprintf(str_BME280,"CONNECT TO BME280 \r\n");      // convert   in  str 
				size=sizeof(str_BME280);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 
			
		}
		
		// Soft reset value
		//#define BME280_SOFTRESET_VALUE 0xB6  		// Soft reset value
//		uint16_t resert_BME280=182;
//		
//		STATUS_BME280=HAL_I2C_Mem_Write(&hi2c1, BME280_ID<<1, (uint16_t)BME280_RESET,1, (uint8_t*)&BME280_SOFTRESET_VALUE, 2, 1000);
		
		
		// 3. Read calibrate registers
		uint8_t buf_calibration_data[24]={0};
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CALIB_00, (uint16_t) 1, buf_calibration_data, (uint16_t)24, 1000);
	
		dig_T1_LSB=buf_calibration_data[0];
		dig_T1_MSB=(unsigned short)buf_calibration_data[1];
		dig_T1=dig_T1_MSB<<8|dig_T1_LSB;
		
		dig_T2_LSB=buf_calibration_data[2];
		dig_T2_MSB=(short)buf_calibration_data[3];
		dig_T2=dig_T2_MSB<<8|dig_T2_LSB;
		
		dig_T3_LSB=buf_calibration_data[4];
	  dig_T3_MSB=(short)buf_calibration_data[5];
		dig_T3=dig_T3_MSB<<8|dig_T3_LSB;
		
		dig_P1=(unsigned short)buf_calibration_data[7]<<8|(unsigned short)buf_calibration_data[6];
		dig_P2=(signed short)buf_calibration_data[9]<<8|(signed short)buf_calibration_data[8];
		dig_P3=(signed short)buf_calibration_data[11]<<8|(signed short)buf_calibration_data[10];
		dig_P4=(signed short)buf_calibration_data[13]<<8|(signed short)buf_calibration_data[12];
		dig_P5=(signed short)buf_calibration_data[15]<<8|(signed short)buf_calibration_data[14];
		dig_P6=(signed short)buf_calibration_data[17]<<8|(signed short)buf_calibration_data[16];
		dig_P7=(signed short)buf_calibration_data[19]<<8|(signed short)buf_calibration_data[18];
		dig_P8=(signed short)buf_calibration_data[21]<<8|(signed short)buf_calibration_data[20];
		dig_P9=(signed short)buf_calibration_data[23]<<8|(signed short)buf_calibration_data[22];
		dig_H1=(unsigned short)buf_calibration_data[24];
		
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CALIB_26, (uint16_t) 1, buf_calibration_data, (uint16_t)10, 1000);
		
		dig_H2=(signed short)buf_calibration_data[1]<<8|(signed short)buf_calibration_data[0];
		dig_H3=(unsigned short)buf_calibration_data[2];
		dig_H4=(signed short)buf_calibration_data[4]<<3|(signed short)buf_calibration_data[5];
		dig_H5=(signed short)buf_calibration_data[6]<<11|(signed short)buf_calibration_data[7];
		dig_H6=(signed short)buf_calibration_data[8];
		
		// Print All callibration data
		
		#if DEBUG_INFORMATION
				sprintf(str_BME280,"CALIBRATE DATA====================\r\n");      // convert   in  str 
				size=sizeof(str_BME280);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 
				
				char str1[60]="";
				sprintf(str1,"dig_T1:%d, dig_T2:%d, dig_T3:%d\r\n", dig_T1, dig_T2, dig_T3);      // convert   in  str 
				size=sizeof(str1);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str1, size, 0xFFF); 
				
				char str2[60]="";
				sprintf(str2,"dig_P1:%d, dig_P2:%d, dig_P3:%d\r\n", dig_P1, dig_P2, dig_P3);      // convert   in  str 
				size=sizeof(str2);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str2, size, 0xFFF); 
				
				char str3[60]="";
				sprintf(str3,"dig_P4:%d, dig_P5:%d, dig_P6:%d\r\n", dig_P4, dig_P5, dig_P6);      // convert   in  str 
				size=sizeof(str3);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str3, size, 0xFFF); 
				
				char str4[60]="";
				sprintf(str4,"dig_P7:%d, dig_P8:%d, dig_P9:%d\r\n", dig_P7, dig_P8, dig_P9);      // convert   in  str 
				size=sizeof(str4);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str4, size, 0xFFF); 
				
				char str5[60]="";
				sprintf(str5,"dig_H1:%d, dig_H2:%d, dig_H3:%d\r\n", dig_H1, dig_H2, dig_H3);      // convert   in  str 
				size=sizeof(str5);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str5, size, 0xFFF); 
						
				char str6[60]="";		
				sprintf(str6,"dig_H4:%d, dig_H5:%d, dig_H6:%d\r\n", dig_H4, dig_H5, dig_H6);      // convert   in  str 
				size=sizeof(str6);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str6, size, 0xFFF); 
				
				char str7[60]="";
				sprintf(str7,"END CALIBRATE DATA====================\r\n\r\n");      // convert   in  str 
				size=sizeof(str7);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str7, size, 0xFFF); 
		#endif
		
		// 4. Configure BME280 
		// 1. Configure BME280_CONFIG register
		uint8_t CONFIG_DATA=0x20;  //0x20;  // 0x88;
		uint8_t BME280_CONFIG_DATA_FROM=0;
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CONFIG, (uint16_t) 1, &BME280_CONFIG_DATA_FROM, (uint16_t)1, 1000);
		BME280_CONFIG_DATA_FROM=BME280_CONFIG_DATA_FROM&0x03;
		BME280_CONFIG_DATA_FROM=BME280_CONFIG_DATA_FROM|CONFIG_DATA;
		STATUS_BME280=HAL_I2C_Mem_Write(&hi2c1, BME280_ID<<1, (uint16_t)BME280_CONFIG,1, &BME280_CONFIG_DATA_FROM, 1, 1000);
		
		// Read CONFIG register
		uint8_t BME280_CONFIG_read=0;
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CONFIG, (uint16_t) 1, &BME280_CONFIG_read, (uint16_t)1, 1000);
		
		// 2. Configure BME280_CTRL_MEAS register
		uint8_t BME280_CTRL_MEAS_read=0;
		uint8_t CTRL_MEAS_DATA=0x27; 	//0x4A;  //0x48
		uint8_t BME280_CTRL_MEAS_DATA_FROM=0;
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CTRL_MEAS, (uint16_t) 1, &BME280_CTRL_MEAS_DATA_FROM, (uint16_t)1, 1000);
		BME280_CTRL_MEAS_DATA_FROM=BME280_CTRL_MEAS_DATA_FROM|CTRL_MEAS_DATA;
		STATUS_BME280=HAL_I2C_Mem_Write(&hi2c1, BME280_ID<<1, (uint16_t)BME280_CTRL_MEAS,1, &BME280_CTRL_MEAS_DATA_FROM, 1, 1000);
		
		// Read BME280_CTRL_MEAS register
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CTRL_MEAS, (uint16_t) 1, &BME280_CTRL_MEAS_read, (uint16_t)1, 1000);	
		
		// 3. Configure BME280_CTRL_HUM register
		uint8_t CTRL_HUM=0x02;
		uint8_t BME280_CTRL_HUM_DATA_FROM=0;
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CTRL_HUM, (uint16_t) 1, &BME280_CTRL_HUM_DATA_FROM, (uint16_t)1, 1000);
		BME280_CTRL_HUM_DATA_FROM=BME280_CTRL_HUM_DATA_FROM&0x07;
		BME280_CTRL_HUM_DATA_FROM=BME280_CTRL_HUM_DATA_FROM|CTRL_HUM;
		STATUS_BME280=HAL_I2C_Mem_Write(&hi2c1, BME280_ID<<1, (uint16_t)BME280_CTRL_HUM,1, &BME280_CTRL_HUM_DATA_FROM, 1, 1000);
		
		// Print All register
		uint8_t BME280_CTRL_HUM_read=0;
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_CTRL_HUM, (uint16_t) 1, &BME280_CTRL_HUM_read, (uint16_t)1, 1000);
		
		#if DEBUG_INFORMATION
				sprintf(str_BME280,"CONFIG=0x%x,  CTRL_MEAS=0x%x, CTRL_HUM=0x%x\r\n",BME280_CONFIG_read,  BME280_CTRL_MEAS_read, BME280_CTRL_HUM_read );      // convert   in  str 
				size=sizeof(str_BME280);
				HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 
    #endif
		
		// 
		if(STATUS_BME280!=HAL_OK)
		{
					sprintf(str_BME280,"ERROR CONNECT TO BME280 !!! STATUS!=HAL_OK\r\n");      // convert   in  str 
					size=sizeof(str_BME280);
					HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 	

					HAL_Delay(1000);	
		}	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
void read_data_BME280(void)
{
	 // HAL_Delay(1000);
		// 4. Read measure data from BME280
		uint8_t buf_data_from_BME280[8]={0};
		STATUS_BME280=HAL_I2C_Mem_Read(&hi2c1, BME280_ID<<1,(uint16_t)BME280_PRESS_MSB, (uint16_t) 1, buf_data_from_BME280, (uint16_t)8, 1000);
	  
		uint32_t adc_T=(uint32_t)buf_data_from_BME280[3]<<12|buf_data_from_BME280[4]<<4|buf_data_from_BME280[5]>>4;
    uint32_t adc_P=(uint32_t)buf_data_from_BME280[0]<<12|(uint32_t)buf_data_from_BME280[1]<<4|(uint32_t)buf_data_from_BME280[2]>>4;		
		uint32_t adc_H=(uint32_t)buf_data_from_BME280[6]<<8|(uint32_t)buf_data_from_BME280[7];
	
		sprintf(str_BME280,"adc_T= %d  adc_P= %d, adc_H= %d \r\n", adc_T, adc_P, adc_H);      // convert   in  str 
		size=sizeof(str_BME280);
		HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 	
		
    cTemp=calulete_temperature(adc_T);   

		// Print 
		for(uint8_t i=0; i<=60; i++)
		{
				str_BME280[i]=0;
		}
		sprintf(str_BME280,"T=%.2f\r\n", cTemp);      // convert   in  str 
		size=sizeof(str_BME280);
		HAL_UART_Transmit(&huart1 , (uint8_t *)str_BME280, size, 0xFFF); 	
			
}

////////////////////////////////////////////////////////////////////////////////////////////////////////
float calulete_temperature(uint32_t adc_T)
{
	// 5. Convert TEMPERATURE/
		// From datasheet
		int32_t var1=((((adc_T>>3)-((int32_t)dig_T1<<1)))*((int32_t)dig_T2))>>11;
		int32_t var2=(((((adc_T>>4)-((int32_t)dig_T1))*((adc_T>>4)-((int32_t)dig_T1)))>>12)*((int32_t)dig_T3))>>14;

		int32_t t_fine=var1+var2;
		T=(t_fine*5+128)>>8;
		cTemp = (((float)T)/100.0f);
	
	  return  cTemp;
}













