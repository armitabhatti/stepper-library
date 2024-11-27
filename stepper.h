/*
 * stepper.h
 *
 *  Created on: Nov 5, 2024
 *      Author: armitabhatti
 */

//use this header file to integrate the stepper motor into stm project
//for the 28BYJ-48 stepper motor with the ULN2003AN motor driver
//the stepper motor has 4 coils corresponding to 4 pins, blue, pink, yellow, orange in that order

#ifndef INC_STEPPER_H_
#define INC_STEPPER_H_


//first, define output pins for the pins connecting to each coil
#define blue_1 GPIO_PIN_5 //PB5, D4
#define pink_2 GPIO_PIN_9 //PA9, D8
#define yellow_3 GPIO_PIN_0 //PC1, A5
#define orange_4 GPIO_PIN_1 //PC0, A4

//define port numbers for each pin above
#define port_b GPIOB
#define port_p GPIOA
#define port_yr GPIOC



//---------------------------------------------initialize private variables------------------------------------------------------------------

int current_step = 0; // will keep track of position in array and varies from 0-7
int motor_speed = 1200; // 1200 microsecond delay between each step

//in one step 5.625 degrees is turned, so 64 steps required for one full rotation of internal motor
//64 rotations of internal motor required for 1 rotation of external motor, 64x64=4096
int steps_per_rev = 4096;

//step sequence starts at 1000  1100  0100  0110  0010  0011  0001  1001 which is going from 1, 1&2, 2, 1&3, 3, 3&4, 4, 4&1
//represents sequence for half-step mode
uint8_t step_sequence[8]= {0x08, 0x0C, 0x04, 0x06, 0x02, 0x03, 0x01, 0x09};

//-------------------------------------------------------------------------------------------------------------------------------------------


//---------------------------------------------initialize private functions------------------------------------------------------------------

void set_pins (int step); //takes in the step in the sequence and sets the output pins based on current step
void motor_steps (int direction); // takes in integer representing clockwise or counterclockwise direction and sets the output pins respectively

//-------------------------------------------------------------------------------------------------------------------------------------------





//---------------------------------------------define private functions------------------------------------------------------------------
void set_pins (int step) {
	//checks if the respective output pin is supposed to be high at the given step sequence
	//if it's high then set the pin high, otherwise set it low
	HAL_GPIO_WritePin (port_b, blue_1, (step_sequence[step] & 0x01) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_p, pink_2, (step_sequence[step] & 0x02) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_yr, yellow_3, (step_sequence[step] & 0x04) ? GPIO_PIN_SET : GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_yr, orange_4, (step_sequence[step] & 0x08) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}


void motor_steps (int direction){

	//iterates from 0 until 1 quarter of a full turn to get 90 degrees
	//for one full rotation: replace i < steps_per_rev/4 with i < steps_per_rev
	for (int i= 0; i < steps_per_rev/4 ; i++ ){
		//call set_pins function which writes the appropriate pin high based on the index of the array step_sequence
		set_pins(current_step);

		//clockwise direction
		if (direction == 1){
			current_step++; //increase current step by 1 each iteration, so that step-sequence array goes in the forward order
			 if (current_step > 7) { //if current step reaches 7, reset back to 0
				current_step = 0;
				}
		}
		//anti-clockwise direction
		else if (direction == -1){
			current_step--; //decrease current step by 1 each iteration, so that step-sequence array goes in the backwards order
			if (current_step < 0 ) { //if current step reaches 0, reset to 7
				current_step = 7;
			}
		}
		HAL_Delay(motor_speed/1000); //motor speed in microseconds, so divide by 1000 to get milliseconds for delay function
	}

	//set all coils back to 0 to avoid drawing unnecessary current
	HAL_GPIO_WritePin (port_b, blue_1, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_p, pink_2, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_yr, yellow_3, GPIO_PIN_RESET);
	HAL_GPIO_WritePin (port_yr, orange_4, GPIO_PIN_RESET);


}

//-------------------------------------------------------------------------------------------------------------------------------------------



#endif /* INC_STEPPER_H_ */
