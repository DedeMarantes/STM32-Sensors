/*
 * bme280.c
 *
 *  Created on: Jun 6, 2024
 *      Author: andre
 */
#include "bme280.h"
float tfine; //Variável Global Tfine Para Utilização no BME280;
void bme280_reset(I2C_HandleTypeDef *hi2c) {
	 //reset dispositivo
	  uint8_t dreset[2];
	  dreset[0] = 0xE0;//endereço registrador reset
	  dreset[1] = 0xB6;//valor para resetar
	  HAL_I2C_Master_Transmit(hi2c, BME280_ADDR, dreset, sizeof(dreset), 1000);
}

void bme280_read_id(I2C_HandleTypeDef *hi2c){
	  uint8_t id_reg = 0xD0;//endereço registrador
	  uint8_t id_bme280;//buffer de leitura do id
	  HAL_I2C_Master_Transmit(hi2c, BME280_ADDR, &id_reg, 1, 1000);
	  HAL_I2C_Master_Receive(hi2c, BME280_ADDR, &id_bme280, 1, 1000);
}

void bme280_basic_config(I2C_HandleTypeDef *hi2c) {
	  uint8_t config_ctrl[4];
	  config_ctrl[0] = 0xF2;//endereço registrador com ctrl_h
	  config_ctrl[1] = 0x01;//dado escrito no registrador no end 0xF2
	  config_ctrl[2] = 0xF4;//endereço registrador com ctrl_c e ctrl_p
	  config_ctrl[3] = 0x27;//dados do registrador 0xF4
	  HAL_I2C_Master_Transmit(hi2c, BME280_ADDR, config_ctrl, 4, 1000);
}

int bme280_get_temp(I2C_HandleTypeDef *hi2c) {
	//Função Para Leitura de Temperatura com Base no BME280.
	//Lendo Registradores e Calibração de Temperatura conforme datasheet
	uint8_t digi_t1_adress=0x88; //Início da Leitura;
	uint8_t digi_t1[2]; //0x88,0x89 - [7:0]/[15:8]
	HAL_I2C_Master_Transmit(hi2c, 0xEC, &digi_t1_adress, 1, 100);
	HAL_I2C_Master_Receive(hi2c, 0xEC, digi_t1, 2, 100);
	//seguindo datasheet de como fazer leitura
	uint8_t digi_t2_adress=0x8A; //Início da Leitura;
	uint8_t digi_t2[2]; //0x8A,0x8B - [7:0]/[15:8]
	HAL_I2C_Master_Transmit(hi2c, 0xEC, &digi_t2_adress, 1, 100);
	HAL_I2C_Master_Receive(hi2c, 0xEC, digi_t2, 2, 100);
	uint8_t digi_t3_adress=0x8C; //Início da Leitura;
	uint8_t digi_t3[2]; //0x8C,0x8D - [7:0]/[15:8]
	HAL_I2C_Master_Transmit(hi2c, 0xEC, &digi_t3_adress, 1, 100);
	HAL_I2C_Master_Receive(hi2c, 0xEC, digi_t3, 2, 100);
	//Recebe Valores de Calibração:
	uint16_t digit1=(digi_t1[1] << 8) | digi_t1[0]; //Primeiro Valor.
	//Shift 8 porque está juntando esses 2 valores de 8 bits para um só de 16 bits
	//a operação or junta os dois
	int16_t digit2=(digi_t2[1] << 8) | digi_t2[0];//Segundo Valor.
	int16_t digit3=(digi_t3[1] << 8) | digi_t3[0];//Terceiro Valor.
	//Rebimento dos Bytes de dados totais, porém só irá armazenar a temperatura.
	//Leitura conforme datasheet dos bytes de dados (Pág.25);
	uint8_t cb=0xF7; //Endereço de início.
	HAL_I2C_Master_Transmit(hi2c, 0xEC, &cb, 1, 100);
	uint8_t datar[8];//0xF7 0xF8 ... 0XFD 0xFE -> 8 bytes com endereços
	//segundo datasheet 0xFA 0xFB 0xFC são os endereços relacionados a temperatura
	HAL_I2C_Master_Receive(hi2c, 0xEC, datar, 8, 100);
	//calculo temperatura seguindo planilha para formula
	uint32_t temp[3];
	temp[0]=datar[3]; //0xFA -> Temperatura, MSB da Temperatura;
	temp[1]=datar[4]; //0xFB -> Temperatura, LSB da Temperatura;
	temp[2]=datar[5]; //0xFC -> Temperatura, XSLB da Temperatura;
	//Ajusta para Posicionamento de 20 bits; 0xFC só tem 4 bits de dados usados
	int32_t utc=(temp[0]<<12)+(temp[1]<<4)+temp[2]; //Reposiciona 20 Bits em 'utc'
	//Algoritmo de Cálculo da Temperatura:,
	//seguindo formulas que passaram na planilha
	float ut=utc;
	float dig_t1=digit1;
	float dig_t2=digit2;
	float dig_t3=digit3;
	float var1, var2, tempbme280;
	int16_t tempbme280i;
	var1=((ut)/16384-(dig_t1)/1024)*(dig_t2);
	var2=(((ut)/131072-(dig_t1)/8192)*((ut)/131072-(dig_t1)/8192))*(dig_t3);
	tfine=var1+var2;
	tempbme280=(tfine/5120);
	tempbme280i=(tempbme280*100);
	HAL_Delay(10); //Aguarda 10ms;
	return tempbme280i;
}

