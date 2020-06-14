/*
 * simple_calc.c
 *
 * Created: 4/5/2020 11:07:15 AM
 *  Author: safifi
 */ 


#include "LCD.h"
#include "keypad.h"

int8_t get_number_and_next_operand(uint32_t *num);
void print_result1(uint32_t n1 ,uint32_t n2 ,int8_t op);
void print_result2(uint32_t n1 ,uint32_t n2,uint32_t ,int8_t op,int8_t);
void wait_press_key();

uint8_t state = 0;
/*
0 -------- clear state 
1 -------- first number and first operand
1 -------- second number and result
2 -------- second number and second operand
3 -------- third number and result 
*/
int main(void)
{
	LCD_init();
	keypad_init();
	uint32_t num1,num2,num3;
	
	while(1)
    {
		state = 0; // this is the clear state and to go to state 1 you should insert 1st num and 1st operator
		//-------------------------------
		/*  transition from state 0 to state 1 actions */
		/*
				if operator1 = 'c' this mean the user press on clear button to clear everything on the LCD and state will be 0
				if operator1 = 0   this mean wrong entry and you should press clear button to start again for state 0
				if operator1 = + - * / this mean the state will change from 0 to 1
		*/
		int8_t operator1 = get_number_and_next_operand(&num1); 
		if(operator1 == 0 ) { // wrong entry and you should press on clear
			wait_press_key();
			continue;
		}
		else if(operator1 == 'C') continue; // clear LCD and state from state 0
		//-------------------------------

		//-------------------------------
		/*  transition from state 1 to state 2 actions */
		/*
				if operator2 = 'c' this mean the user press on clear button to clear everything on the LCD and state will be 0
				if operator2 = 0   this mean wrong entry and you should press clear button to start again for state 0
				if operator2 = + - * / this mean the state will change from 1 to 2
				if operator2 = '=' this mean the state will change from 1 to 3 and result will be appear and to enter new operation you should
								   press clear again to go to state 0
		*/
		int8_t operator2 = get_number_and_next_operand(&num2);
		if(operator2 == 0 ) { // wrong entry and you should press on clear
			wait_press_key();
			continue;
		}
		else if(operator1 == 'C') continue; // clear LCD and state from state 0
		
		if (state == 3)
		{
			print_result1(num1,num2,operator1);
			wait_press_key();
			continue;
		}
		//--------------------------------

		//-------------------------------
		/*  transition from state 2 to state 3 actions */
		/*
				if operator3 = 'c' this mean the user press on clear button to clear everything on the LCD and state will be 0
				if operator3 = 0   this mean wrong entry and you should press clear button to start again for state 0
				if operator3 = '=' this mean the state will change from 2 to 3 and result will be appear and to enter new operation you should
								   press clear again to go to state 0
		*/

		int8_t operator3 = get_number_and_next_operand(&num3);
		if(operator3 == 0 ) { // wrong entry and you should press on clear
			wait_press_key();
			continue;
		}
		else if(operator3 == 'C') continue; // clear LCD and state from state 0
		
		if (state == 3) 
		{
			print_result2(num1,num2,num3,operator1,operator2); // 3 number and 2 operators
			wait_press_key();
			continue;
		}
		//--------------------------------
							
    }
}
void wait_press_key(){
	int8_t key = keypad_read();
	while(key != 'C') key = keypad_read();  // wait until clear send
	while(keypad_read() == key) ; // to prevent multiple print of same push button
	LCD_write_command(1); // clear	
}
void print_result1(uint32_t n1 ,uint32_t n2 ,int8_t op){ //normal operation between 2 number
	LCD_write_command(0xc0);
	switch(op){
		case '+': LCD_write_num(n1+n2); break;
		case '-': LCD_write_num(n1-n2); break;
		case '/': LCD_write_num(n1/n2); break;
		case '*': LCD_write_num(n1*n2); break;
	}
}

void print_result2(uint32_t n1 ,uint32_t n2 ,uint32_t n3 ,int8_t op1 ,int8_t op2){
	LCD_write_command(0xc0);
	uint32_t temp = 0;
	if(op1 == '*' || op1 == '/'){
		switch(op1){
			case '/': temp = n1/n2; break;
			case '*': temp = n1*n2; break;
		}
		switch(op2){
			case '+': LCD_write_num(temp + n3); break;
			case '-': LCD_write_num(temp - n3); break;
			case '/': LCD_write_num(temp / n3); break;
			case '*': LCD_write_num(temp * n3); break;
		}
		return;
	}
	else if(op2 == '*' || op2 == '/'){
		switch(op2){
			case '/': temp = n2/n3; break;
			case '*': temp = n2*n3; break;
		}
		switch(op1){
			case '+': LCD_write_num(n1 + temp); break;
			case '-': LCD_write_num(n1 - temp); break;
			case '/': LCD_write_num(n1 / temp); break;
			case '*': LCD_write_num(n1 * temp); break;
		}
		return;		
	}
	
	else if(op1 == '+')	temp = n1 + n2;
	else if(op1 == '-')	temp = n1 - n2;
	
	if (op2 == '+') LCD_write_num(temp + n3);
	else LCD_write_num(temp - n3);	
			
}
int8_t get_number_and_next_operand(uint32_t *num){
	uint8_t txt[4];
	uint16_t i = 1;
	*num = 0;
	do{
		int8_t key = -1;
		while(key == -1) key = keypad_read();
		while(keypad_read() == key) ; // to prevent multiple print of same push button
		
		if (key >= '0' && key <= '9'){
			*num = (key-48) + *num * i;
			LCD_write_char(key);
			i = 10 * i;
		}
		else if (key == '+' || key == '-' || key == '*' || key == '/' ){ // valid in state 0 and 1
			LCD_write_char(key);
			if (state == 2 || i == 1)
			{
				state = 0;
				LCD_write_command(0xc);
				LCD_write_string("wrong entry");
				return 0;
			}
			state++; // GO TO next state and return operation
			return key;
		}
		else if( key == '=' ){ // valid in state 1 and 2
			LCD_write_char(key);
			if ((state == 1 || state == 2) && i != 1 )
			{
				state = 3;
				return key;
			}
			else{
				state = 0;
				LCD_write_command(0xc);
				LCD_write_string("wrong entry");
				return 0;				
			}				
		}
		else if (key == 'C'){  // if press clear, clear LCD and start from state 0
			LCD_write_command(1); // clear
			state = 0;
			return key;
		}
	}while(i <= 1000);
}