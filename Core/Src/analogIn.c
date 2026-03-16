#include "analogIn.h"

#define ANALOG_IN_SEQUENCER_LENGTH					8

extern uint16_t ADC_VAL[4];

AnalogInStruct analogInA;
volatile AnalogInCHStruct analogInAChannels[8];
AnalogInStruct analogInB;
volatile AnalogInCHStruct analogInBChannels[8];

uint8_t analogInAChSeqIndex;
uint8_t analogInACHSequencer[ANALOG_IN_SEQUENCER_LENGTH];
uint8_t analogInBChSeqIndex;
uint8_t analogInBCHSequencer[ANALOG_IN_SEQUENCER_LENGTH];
uint8_t channelBitFlags = 0;

uint16_t preScalerIndex[8] = {8-1,8-1,8-1,4-1,4-1,4-1,4-1,2-1};
uint16_t counterPeriodIndex[8] = {8000-1, 4000-1, 1600-1, 1600-1, 800-1, 320-1, 160-1, 160-1};



/**
  * @brief	This function initializes the Analog IN Channels:
  * 		- Initialize structures
  * 		- Initialize ADA4254 Amplifier
  * 		- Calibrate ADC offset
  * @param	None
  * @return	None
  */
void AnalogInInit() {
	//Initialize external hardware/IC
	//ADA4254Init(ANALOG_IN_BLOCK_A);
	//ADA4254Init(ANALOG_IN_BLOCK_B);

	//Init Analog In Block A structs
	analogInA.config = (AnalogInConfigStruct){Sample_125k, Scale_1V};
	analogInA.offsetCall = 0;

	//Init Analog In Block B structs
	analogInB.config = (AnalogInConfigStruct){Sample_125k, Scale_1V};
	analogInB.offsetCall = 0;

	//Initialize structures
	uint8_t i = 0;
	for(i = 0; i < 8; i++) {
		//Init Analog In Block A Channel structs
		analogInAChannels[i].channel = i + 1;
		analogInAChannels[i].config = (AnalogInCHConfigStruct){Mode_Off, 0x01, Resolution_12Bit, Gain_1_8};
		analogInAChannels[i].bufferReadIndex = 0;
		analogInAChannels[i].bufferWriteIndex = 0;

		//Init Analog In Block B Channel structs
		analogInBChannels[i].channel = i + 1;
		analogInBChannels[i].config = (AnalogInCHConfigStruct){Mode_Off, 0x01, Resolution_12Bit, Gain_1_8};
		analogInBChannels[i].bufferReadIndex = 0;
		analogInBChannels[i].bufferWriteIndex = 0;
	}

	//Initialize sequencer
	for(i = 0; i < ANALOG_IN_SEQUENCER_LENGTH; i++) {
		analogInACHSequencer[i] = 0;
		analogInBCHSequencer[i] = 0;
	}
	analogInAChSeqIndex = 0;
	analogInBChSeqIndex = 0;

	//Calibrate ADCs
	AnalogInCalibration(ANALOG_IN_BLOCK_A);
	AnalogInCalibration(ANALOG_IN_BLOCK_B);


}

/**
  * @brief	This function calibrates the Analog In Block, the ADC offset
  * @param	anBlocks: Which analog block, A or B
  * @return	None
  */
void AnalogInCalibration(uint8_t anBlock) {
	if(anBlock == ANALOG_IN_BLOCK_A) {
		//Set up for calibration
		//LL_ADC_DisableIT_EOC(ADC1);
		//LL_ADC_DisableIT_EOS(ADC1);
		//ADA4254WriteRegister(anBlock, INPUT_MUX, SW_D12_MASK);		//Set Internal Mux connections: Set to D12, short analog IN

		uint32_t callSum = 0;

		uint8_t i;
		//for(i = 0; i < 64; i++) {
		//	while(LL_ADC_IsActiveFlag_EOC(ADC1) == 0x00);
		//	uint16_t value = LL_ADC_REG_ReadConversionData32(ADC1);
		//	callSum += value;
		//	LL_ADC_ClearFlag_EOC(ADC1);
		//}

		analogInA.offsetCall = 0000; //2048 - (callSum >> 6);

		//Reset settings
		//ADA4254WriteRegister(anBlock, INPUT_MUX, (SW_A1_MASK + SW_B2_MASK));
		//LL_ADC_EnableIT_EOS(ADC1);
	}
	else if(anBlock == ANALOG_IN_BLOCK_B) {
		//Set up for calibration
		//LL_ADC_DisableIT_EOC(ADC2);
		//LL_ADC_DisableIT_EOS(ADC2);
		//ADA4254WriteRegister(anBlock, INPUT_MUX, SW_D12_MASK);		//Set Internal Mux connections: Set to D12, short analog IN

		uint32_t callSum = 0;

		uint8_t i;
		//for(i = 0; i < 64; i++) {
		//	while(LL_ADC_IsActiveFlag_EOC(ADC2) == 0x00);
		//	uint16_t value = LL_ADC_REG_ReadConversionData32(ADC2);
		//	callSum += value;
		//	LL_ADC_ClearFlag_EOC(ADC2);
		//}

		analogInB.offsetCall = 2048 - (callSum >> 6);

		//Reset settings
		//ADA4254WriteRegister(anBlock, INPUT_MUX, (SW_A1_MASK + SW_B2_MASK));
		//LL_ADC_EnableIT_EOS(ADC2);
	}
}


/**
  * @brief	This function sets an Analog IN Block
  * @param	anBlock: Which analog block, A or B
  * @param	config: The configuration for the analog IN block, AnalogInConfigStruct
  * @return	None
  */
void AnalogInConfig(uint8_t anBlock, AnalogInConfigStruct config) {
	if(anBlock == ANALOG_IN_BLOCK_A) {
		analogInA.config = config;

		//Set Sample Rate
//		uint32_t sampleFreq = (4 << analogInA.config.rate) - 1;
		uint32_t sampleFreq = (1 << analogInA.config.rate) - 1;
		if(sampleFreq < 1) {
			return;
		}
		TIM1SetARR(sampleFreq);

		//Set Scaling, ADA4254 output gain
	}
	else if(anBlock == ANALOG_IN_BLOCK_B) {
		analogInB.config = config;

		//Set Sample Rate
//		uint32_t sampleFreq = (4 << analogInA.config.rate) - 1;
		uint32_t sampleFreq = (1 << analogInB.config.rate) - 1;
		if(sampleFreq < 1) {
			return;
		}
		TIM3SetARR(sampleFreq);

		//Set Scaling, ADA4254 output gain
	}
}

void ADC_Calibrate(ADC_HandleTypeDef* hadc, TIM_HandleTypeDef *htim) {
  // 1. Ensure ADC is disabled
  HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
  HAL_ADC_Stop(hadc);
  //HAL_SYSCFG_VREFBUF_VoltageScalingConfig(SYSCFG_VREFBUF_VOLTAGE_SCALE0);
  //HAL_SYSCFG_EnableVREFBUF();

  // 2. Start calibration
  //HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED);
  HAL_ADCEx_Calibration_Start(hadc, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED );

  // 3. Wait for completion
  //while(HAL_ADCEx_Calibration_GetValue(hadc,ADC_DIFFERENTIAL_ENDED) != HAL_OK);
  HAL_ADC_Start(hadc);
  HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);


}

/**
  * @brief	This function sets an Analog IN Channel
  * @param	anBlock: Which analog block, A or B
  * @param	channel: Which analog channel, 1 to 8
  * @param	config: The set configuration for the channel, AnalogInCHConfigStruct
  * @return	None
  */
void AnalogInConfigChannel(uint8_t anBlock, uint8_t channel, AnalogInCHConfigStruct config, ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim) {
	if(anBlock == ANALOG_IN_BLOCK_A) {
		analogInAChannels[channel - 1].config = config;
		// Bug: KBB Channel not initiated. Fixed
		analogInAChannels[channel-1].channel = channel;
		uint8_t seqIndex = 0;
		uint8_t i;
		for(i = 0; i < 8; i++) {
			if(analogInAChannels[i].config.mode != Mode_Off) {
				analogInACHSequencer[seqIndex++] = analogInAChannels[i].channel;
				channelBitFlags |= 1 << i;
			} else {
				channelBitFlags &= ~(uint8_t)(1 << i);
			}
		}
		for(i = seqIndex; i < 8; i++) {
			analogInACHSequencer[i] = 0;
		}
	}
	else if(anBlock == ANALOG_IN_BLOCK_B) {
		analogInBChannels[channel - 1].config = config;
		// Bug: KBB Channel not initiated. Fixed
				analogInBChannels[channel-1].channel = channel;

		uint8_t seqIndex = 0;
		uint8_t i;
		for(i = 0; i < 8; i++) {
			if(analogInBChannels[i].config.mode != Mode_Off) {
				analogInBCHSequencer[seqIndex++] = analogInBChannels[i].channel;
			}
		}
		for(i = seqIndex; i < 8; i++) {
			analogInBCHSequencer[i] = 0;
		}
	}
	uint32_t state = HAL_ADC_GetState(hadc);
	if (state & HAL_ADC_STATE_BUSY_REG) {
		if (channelBitFlags==0) {
			HAL_ADC_Stop_DMA(hadc); // Turn off ADC
		}
	} else {
		if (channelBitFlags!=0) {
			HAL_ADC_Start_DMA(hadc, (uint32_t*)ADC_VAL, 1);  // Start ADC
			ADC_Calibrate(hadc,htim);
		}
	}
}

/**
  * @brief	This function stops all Analog In acquisitions and clears buffers
  * @param	None
  * @return	None
  */
void AnalogInStopAll() {
	//Clear Analog IN Channel sequencers
	uint8_t i;
	for(i = 0; i < ANALOG_IN_SEQUENCER_LENGTH; i++) {
		analogInACHSequencer[i] = 0;
		analogInBCHSequencer[i] = 0;
	}
	analogInAChSeqIndex = 0;
	analogInBChSeqIndex = 0;

	//Clear Analog IN Channel settings
	for(i = 0; i < 8; i++) {
		//Clear Analog IN A Channel settings, all to OFF
		analogInAChannels[i].config.mode = Mode_Off;
		//Clear Analog IN A Buffers
		analogInAChannels[i].bufferReadIndex = 0;
		analogInAChannels[i].bufferWriteIndex = 0;

		//Clear Analog IN B Channel settings, all to OFF
		analogInBChannels[i].config.mode = Mode_Off;
		//Clear Analog IN B Buffers
		analogInBChannels[i].bufferReadIndex = 0;
		analogInBChannels[i].bufferWriteIndex = 0;
	}
}

/**
  * @brief	This function reads an Analog IN Channel
  * @param	anBlock: Which analog block, A or B
  * @param	channel: Which analog channel, 1 to 8
  * @param	data: data array to be filled with the buffer data
  * @return	Length of the buffer
  */
uint16_t AnalogInGetData(uint8_t anBlock, uint8_t channel, uint8_t* data) {
	uint16_t length = 0;
	if(anBlock == ANALOG_IN_BLOCK_A) {
		int16_t lenAux = analogInAChannels[channel - 1].bufferWriteIndex - analogInAChannels[channel - 1].bufferReadIndex;

		if(lenAux < 0) {
			lenAux += 512;
		}
		length = lenAux;

		uint16_t i;
		for(i = 0; i < length; i++) {
			data[i] = analogInAChannels[channel - 1].buffer[analogInAChannels[channel - 1].bufferReadIndex++];

			if(analogInAChannels[channel - 1].bufferReadIndex >= 512) {
				analogInAChannels[channel - 1].bufferReadIndex = 0;
			}
		}
	}
	else if(anBlock == ANALOG_IN_BLOCK_B) {
		int16_t lenAux = analogInBChannels[channel - 1].bufferWriteIndex - analogInBChannels[channel - 1].bufferReadIndex;

		if(lenAux < 0) {
			lenAux += 512;
		}
		length = lenAux;

		uint16_t i;
		for(i = 0; i < length; i++) {
			data[i] = analogInBChannels[channel - 1].buffer[analogInBChannels[channel - 1].bufferReadIndex++];

			if(analogInBChannels[channel - 1].bufferReadIndex >= 512) {
				analogInBChannels[channel - 1].bufferReadIndex = 0;
			}
		}
	}
	return length;
}

/**
  * @brief	This function handles all the analog In functions, switching and re-configurations
  * @param	anBlock: Signals the Handler which ADC IRQ called this function, which analog Block A or B
  * @return	None
  */
AnalogInMode prevMode = Mode_Off;
void AnalogInHandler(uint8_t anBlock) {
	if(anBlock == ANALOG_IN_BLOCK_A) {
		//GPIOWrite(GPIO_IO_GPIO0, 1);

		//uint32_t test = LL_TIM_GetPrescaler(TIM1);
		//HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);

		//Check for ADC Overflow error
		//if(LL_ADC_IsActiveFlag_OVR(ADC1) == 0x01) {
			//ADC1 Overflow error
		//	LL_ADC_ClearFlag_OVR(ADC1);
		//}

		/*Read ADC to temp variable and get respective channel number*/
		//uint16_t value = LL_ADC_REG_ReadConversionData32(ADC1);
		//uint8_t channel = analogInACHSequencer[analogInAChSeqIndex];
		/*
		//Set up next channel to be read: Set ADA4254
		//This has to be done before read ADC value processing so that SPI is starting/busy faster
		analogInAChSeqIndex += 1;
		if(analogInAChSeqIndex >= ANALOG_IN_SEQUENCER_LENGTH || analogInACHSequencer[analogInAChSeqIndex] == 0x00) {
			analogInAChSeqIndex = 0;
		}

		uint8_t nextChannel = analogInACHSequencer[analogInAChSeqIndex];
		if(nextChannel != 0x00) {
			uint8_t reg = 0;

			//First switch the internal MUX, this is slower so it has to be done first to have more time to stabilize
			//Check if Mode has to be changed, if is different then for previous acquisition
			if(analogInAChannels[nextChannel - 1].config.mode != prevMode) {
				if(analogInAChannels[nextChannel - 1].config.mode == Mode_Differential) {
					reg = SW_A1_MASK + SW_A2_MASK;
				}
				else if(analogInAChannels[nextChannel - 1].config.mode == Mode_Single) {
					if((nextChannel % 2) == 0x01) {
						reg = SW_A1_MASK + SW_B2_MASK;
					}
					else {
						reg = SW_B1_MASK + SW_B2_MASK;
					}
				}
				ADA4254WriteRegister(ANALOG_IN_BLOCK_A, INPUT_MUX, reg);
			}
			prevMode = analogInAChannels[nextChannel - 1].config.mode;

			uint8_t extMux = 3 - ((nextChannel - 1) >> 1);
			uint8_t gain = analogInAChannels[nextChannel - 1].config.gain;
			reg = ((gain << 3) & IN_AMP_GAIN_MASK) + (extMux & EXT_MUX_MASK);
			ADA4254WriteRegister(ANALOG_IN_BLOCK_A, GAIN_MUX, reg);
		} */

		//Check if read ADC value is from a channel and read it to buffer if yes
		/*if(channel != 0x00) {
			//Apply calibration offset

		*/
		for (uint8_t channel=1; channel<2; channel++)
		{
			uint16_t value = ADC_VAL[channel-1];
			value += analogInA.offsetCall;

			//Write to Buffer
			analogInAChannels[channel - 1].buffer[analogInAChannels[channel - 1].bufferWriteIndex] = (value >> 8);
			analogInAChannels[channel - 1].buffer[analogInAChannels[channel - 1].bufferWriteIndex + 1] = value;

			//Update Buffer index, buffer is circular
			analogInAChannels[channel - 1].bufferWriteIndex += 2;
			if(analogInAChannels[channel - 1].bufferWriteIndex >= 512) {
				analogInAChannels[channel - 1].bufferWriteIndex = 0;
			}
		}/*

		GPIOWrite(GPIO_IO_GPIO0, 0);*/
	}/*
	else if(anBlock == ANALOG_IN_BLOCK_B) {
		GPIOWrite(GPIO_IO_GPIO1, 1);

		//Check for ADC Overflow error
		if(LL_ADC_IsActiveFlag_OVR(ADC2) == 0x01) {
			//ADC1 Overflow error
			LL_ADC_ClearFlag_OVR(ADC2);
		}

		//Read ADC to temp variable and get respective channel number
		uint16_t value = LL_ADC_REG_ReadConversionData32(ADC2);
		uint8_t channel = analogInBCHSequencer[analogInBChSeqIndex];

		//Set up next channel to be read: Set ADA4254
		//This has to be done before read ADC value processing so that SPI is starting/busy faster
		analogInBChSeqIndex += 1;
		if(analogInBChSeqIndex >= ANALOG_IN_SEQUENCER_LENGTH || analogInBCHSequencer[analogInBChSeqIndex] == 0x00) {
			analogInBChSeqIndex = 0;
		}

		uint8_t nextChannel = analogInBCHSequencer[analogInBChSeqIndex];
		if(nextChannel != 0x00) {
			uint8_t reg = 0;

			//First switch the internal MUX, this is slower so it has to be done first to have more time to stabilize
			if(analogInBChannels[nextChannel - 1].config.mode == Mode_Differential) {
				reg = SW_A1_MASK + SW_A2_MASK;
			}
			else if(analogInBChannels[nextChannel - 1].config.mode == Mode_Single) {
				if((nextChannel % 2) == 0x01) {
					reg = SW_A1_MASK + SW_B2_MASK;
				}
				else {
					reg = SW_B1_MASK + SW_B2_MASK;
				}
			}
			ADA4254WriteRegister(ANALOG_IN_BLOCK_B, INPUT_MUX, reg);

			uint8_t extMux = 3 - ((nextChannel - 1) >> 1);
			uint8_t gain = analogInBChannels[nextChannel - 1].config.gain;
			reg = ((gain << 3) & IN_AMP_GAIN_MASK) + (extMux & EXT_MUX_MASK);
			ADA4254WriteRegister(ANALOG_IN_BLOCK_B, GAIN_MUX, reg);
		}

		//Check if read ADC value is from a channel and read it to buffer if yes
		if(channel != 0x00) {
			//Apply calibration offset
			value += analogInB.offsetCall;

			//Write to Buffer
			analogInBChannels[channel - 1].buffer[analogInBChannels[channel - 1].bufferWriteIndex] = (value >> 8);
			analogInBChannels[channel - 1].buffer[analogInBChannels[channel - 1].bufferWriteIndex + 1] = value;

			//Update Buffer index, buffer is circular
			analogInBChannels[channel - 1].bufferWriteIndex += 2;
			if(analogInBChannels[channel - 1].bufferWriteIndex >= 512) {
				analogInBChannels[channel - 1].bufferWriteIndex = 0;
			}
		}

		GPIOWrite(GPIO_IO_GPIO1, 0);
	}*/

}

void SetTimer1(uint8_t index) {
	uint16_t timer1PreScaler = preScalerIndex[index];
	uint16_t timer1CounterPeriod = counterPeriodIndex[index];
	TIM1->CR1 &= ~TIM_CR1_CEN; // Clear the counter enable bit
	TIM1->ARR = timer1CounterPeriod; // Set the new Auto-Reload Register (ARR) value
	TIM1->PSC = timer1PreScaler;
	TIM1->EGR |= TIM_EGR_UG; // Set the Update Generation (UG) bit to reload the prescaler and counter with the new values immediately
	TIM1->SR &= ~TIM_SR_UIF; // Clear the update interrupt flag if necessary
	TIM1->CR1 |= TIM_CR1_CEN; // Start the timer. Set the counter enable bit
}



