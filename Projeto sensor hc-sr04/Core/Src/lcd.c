/*
 * lcd.c
 *
 *  Created on: Jun 10, 2024
 *      Author: andre
 */
#include "lcd.h"
/*
PO -> RS
P1 -> RW
P2 -> En
P3 -> BL
P4 -> DB4
P5 -> DB5
P6 -> DB6
P7 -> DB7
*/
extern I2C_HandleTypeDef hi2c1;

void lcd_send_cmd(char cmd) {
	char data_upper, data_lower;
	uint8_t data_transmit[4];
	data_upper = (cmd & 0xF0);//pegar 4 bits mais significativos de cmd
	data_lower = ((cmd << 4) & 0xF0);//pegar 4 bits menos significativos
	//cmd HHHHLLLL separar para data_h = HHHH0000 e data_l = LLLL0000
	//comandos enviados dado 8 bits, separar em 2 partes, tem que ser 4 mais significativo
	//os 4 ultimos bits de cada byte são RS,RW,EN,BL
	data_transmit[0] = data_upper | 0x0C; //BL = 1 En = 1 RW = 0 RS = 0
	data_transmit[1] = data_upper | 0x08; //BL = 1 En = 0 RW = 0 RS = 0
	data_transmit[2] = data_lower | 0x0C;
	data_transmit[3] = data_lower | 0x08;
	HAL_I2C_Master_Transmit(&hi2c1, MODULE_ADDR, data_transmit, 4, 1000);
}

void lcd_send_data(char data){
	char data_upper, data_lower;
	uint8_t data_transmit[4];
	data_upper = (data & 0xF0);//pegar 4 bits mais significativos de cmd
	data_lower = ((data << 4) & 0xF0);//pegar 4 bits menos significativos
	//cmd HHHHLLLL separar para data_h = HHHH0000 e data_l = LLLL0000
	//comandos enviados dado 8 bits, separar em 2 partes, tem que ser 4 mais significativo
	//os 4 ultimos bits de cada byte são RS,RW,EN,BL
	data_transmit[0] = data_upper | 0x0D; //BL = 1 En = 1 RW = 0 RS = 1
	data_transmit[1] = data_upper | 0x09; //BL = 1 En = 0 RW = 0 RS = 1
	data_transmit[2] = data_lower | 0x0D;
	data_transmit[3] = data_lower | 0x09;
	HAL_I2C_Master_Transmit(&hi2c1, MODULE_ADDR, data_transmit, 4, 1000);
}

void lcd_init() {
	//segundo datasheet dar 3 vezes 0x30 e depois 0x20 para modo 4 bits
	HAL_Delay(50);//espera >40ms
	lcd_send_cmd(0x30);
	HAL_Delay(5);
	lcd_send_cmd(0x30);
	HAL_Delay(1);
	lcd_send_cmd(0x30);
	HAL_Delay(5);
	lcd_send_cmd(0x20);//modo 4 bits comando
	HAL_Delay(10);
	//inicialização
	lcd_send_cmd(FUNCTION_SET);
	HAL_Delay(1);
	lcd_send_cmd(DISPLAY_OFF);
	HAL_Delay(1);
	lcd_send_cmd(CLR_DISPLAY);
	HAL_Delay(2);
	lcd_send_cmd(DISPLAY_ON);
	HAL_Delay(1);
	lcd_send_cmd(INC_CURSOR);
}

void lcd_send_str(char *str) {
	//checa se valor apontado é o caracter nulo \0 se não for manda para o lcd
	while(*str) {
		//manda para o lcd e incrementa o ponteiro para a próxima posíção
		lcd_send_data(*str++);
	}
}

void lcd_clear() {
	lcd_send_cmd(CLR_DISPLAY);
	HAL_Delay(2);
}

void lcd_shift_left(){
	lcd_send_cmd(SHIFT_LEFT);
	HAL_Delay(2);
}
void lcd_shift_right(){
	lcd_send_cmd(SHIFT_RIGHT);
	HAL_Delay(2);
}
void lcd_cursor_blink(uint8_t blink){
	if(blink == 1) {
		lcd_send_cmd(CURSOR_BLINK);
	} else if(blink == 0) {
		lcd_send_cmd(CURSOR_NO_BLINK);
	}
	HAL_Delay(2);
}
void lcd_cursor_left() {
	lcd_send_cmd(CURSOR_LEFT);
	HAL_Delay(2);
}
void lcd_cursor_right() {
	lcd_send_cmd(CURSOR_RIGHT);
	HAL_Delay(2);
}

void lcd_set_cursor(uint8_t row, uint8_t col) {
	uint8_t row_calc = row - 1;
	//Calcula valor de enrdereço memoria DDRAM
	//segundo data sheet 0x0 começo primeira linha e 0x40 segunda linha
	//entao se linha = 1 vai ficar 0 no calculo e vai ser 0x0
	//0x80 segundo datasheet o comando set DDRAM Address começa com 1 no bit mais significativo
    uint8_t maskData = 0x80 | (col + row_calc * 0x40);
    lcd_send_cmd(maskData);
    HAL_Delay(2);
}

