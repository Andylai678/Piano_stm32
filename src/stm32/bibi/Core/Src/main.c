/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include"lcd_16x2.h"
#include "math.h"
#include "stdlib.h"  // for rand()
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CLOCK_FREQ 1000000
#define FREQ_DO 262
#define FREQ_RE 294
#define FREQ_MI 330
#define FREQ_FA 349
#define FREQ_SO 392
#define FREQ_LA 440
#define FREQ_SI 494
#define FREQ_DO_H 523
#define FREQ_RE_H 587
#define FREQ_MI_H 660
//#define FREQ_MI_H 659

float freq[10] = {FREQ_DO, FREQ_RE, FREQ_MI, FREQ_FA,
                FREQ_SO, FREQ_LA, FREQ_SI, FREQ_DO_H ,
			FREQ_RE_H,FREQ_MI_H};

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart3;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_TIM3_Init(void);
static void MX_I2C1_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int pitch_offsets[10] = {0};  // 每�?��?�鍵??�音調�?�移�???-5 ~ +5�???
uint32_t Read_Potentiometer() {
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 100);
    return HAL_ADC_GetValue(&hadc1);
}

uint8_t lcd_col = 0;
uint8_t lcd_row = 0;

#define MAX_NOTE_HISTORY 1000  // ??�??? ?? 32 ? ?�� ??

char note_history[MAX_NOTE_HISTORY][4];  // �??? ? ?���??? ? ?? 3 �??? ?? + null terminator
uint8_t note_index = 0;
uint8_t awaiting_song_selection = 0;


#define MAX_SONGS 20
#define MAX_NOTES_PER_SONG 1000

uint8_t voice_recognition_mode = 0;

char songs[MAX_SONGS][MAX_NOTES_PER_SONG] = {
	"5:4 6:4 5:4 4:4 3:4 4:4 5:4 1:4 1:4",               // 0. Let it Be
	"3:4 3:4 4:4 5:4 5:4 4:4 3:4 2:4 1:4",               // 1. Imagine
	"3:4 4:4 5:4 6:4 6:4 5:4 4:4 3:4",                   // 2. Yesterday
	"5:4 5:4 6:4 5:4 4:4 3:4 3:4 4:4 5:4",               // 3. Hey Jude
	"6:4 5:4 4:4 5:4 6:4 6:4 5:4 4:4",                   // 4. Hello
	"5:4 5:4 5:4 4:4 3:4 3:4 4:4 5:4",                   // 5. Shape of You
	"3:4 3:4 4:4 5:4 5:4 6:4 6:4 5:4",                   // 6. Perfect
	"5:4 6:4 6:4 5:4 4:4 4:4 3:4",                       // 7. Believer
	"3:4 3:4 4:4 5:4 5:4 6:4 6:4 5:4",                   // 8. Counting Stars
	"4:4 5:4 6:4 6:4 5:4 4:4 3:4",                       // 9. Faded
    "5:4 5:4 5:4 5:8 6:8 5:4 4:4 3:4 2:4 1:4 1:4 1:8 6:8 6:4 5:4 4:4 3:4 2:4 1:4",  // 0. ??�白气�??(?��?���???)
    "5:4 5:8 6:8 5:8 1:8 7:4 5:4 5:8 6:8 5:8 1:8 2:4 3:4 2:4 2:8 1:8",               // 1. 说好不哭(?��?���???)
    "1:4 2:4 3:4 5:4 5:4 3:4 2:4 1:8 6:8 6:4 1:4 5:4 5:4 3:4 2:4",                     // 2. 年�?��?�为(??�荣�???)
    "5:4 5:4 5:4 5:8 6:8 7:4 1:4 1:4 1:8 7:8 6:4 5:4 5:4 5:8 6:8 7:4 1:4 2:4 3:4",     // 3. ??�年之�??(G.E.M.)
    "3:4 4:4 5:4 5:4 4:4 3:4 2:4 1:4 1:4 2:4 3:4 4:4 4:4 3:4 2:4",                     // 4. 演�??(??��?�谦)
    "1:8 1:8 1:8 1:8 5:4 5:4 5:8 6:8 6:4 6:4 5:4 5:4 4:4 4:4 3:4 3:4 2:4 2:8 1:8",     // 5. 学猫?��(?���???)
    "5:4 6:4 5:4 4:4 3:4 4:4 5:4 6:4 5:4 4:4 3:4 4:4 5:4 1:4 1:4",                     // 6. 沙�?��?�驼(?���???)
    "3:4 4:4 5:4 5:4 6:4 6:4 5:4 3:4 2:4 3:4 4:4 5:4 5:4 6:4 6:4 5:4 3:4 2:4",         // 7. 体面(于�?��??)
    "1:4 2:4 3:4 3:4 2:4 1:4 1:4 2:4 3:4 3:4 2:4 1:4 1:4 2:4 3:4 4:4 5:4 6:4",         // 8. 纸短??�长(?���???)
    "5:4 5:4 6:4 5:4 4:4 3:4 2:4 1:4 5:4 5:4 6:4 5:4 4:4 3:4 2:4 1:4"                  // 9. �?????��?��??(马良
};

const char *known_song_names[10] = {
    "let it be", "imagine", "yesterday", "hey jude", "hello",
    "shape of you", "perfect", "believer", "counting stars", "faded"
};

int Match_Song_Name(const char *name) {
    for (int i = 0; i < 10; i++) {
        if (strstr(name, known_song_names[i]) != NULL) {
            return i;
        }
    }
    return -1;
}

char Read_Keypad(void)
{
    const char keymap[4][4] = {
        {'1', '2', '3', 'A'},
        {'4', '5', '6', 'B'},
        {'7', '8', '9', 'C'},
        {'*', '0', '#', 'D'}
    };

    uint16_t row_pins[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};
    uint16_t col_pins[4] = {GPIO_PIN_1, GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4};

    for (int row = 0; row < 4; row++)
    {
        // �??? ?  ???  ? ?�� HIGH
        for (int i = 0; i < 4; i++)
        {
            HAL_GPIO_WritePin(GPIOA, row_pins[i], GPIO_PIN_SET);
        }

        // 將當??  ?  ?? LOW
        HAL_GPIO_WritePin(GPIOA, row_pins[row], GPIO_PIN_RESET);

        // ??  ?  ???  ??
        for (int col = 0; col < 4; col++)
        {
            if (HAL_GPIO_ReadPin(GPIOB, col_pins[col]) == GPIO_PIN_RESET)
            {
                // �??? ?  ? ?��?  ??  ? ?��??  ? 次觸?�� ??????????????
                while (HAL_GPIO_ReadPin(GPIOB, col_pins[col]) == GPIO_PIN_RESET);
                return keymap[row][col];
            }
        }
    }

    return 0; // �??? ?  ? ?���??? ?  ??
}
void LCD16_PrintNoteStr(const char* note) {
    // �??? ? ?�� ??
    if (note_index < MAX_NOTE_HISTORY) {
        strncpy(note_history[note_index], note, 3);
        note_history[note_index][3] = '\0';  // 保險�??? ?  ?  ?  ? �??? ??
        note_index++;
    }

    // 顯示?   LCD
    if (lcd_col > 15) {
        lcd_col = 0;
        lcd_row++;
        if (lcd_row >= 2) {
            lcd_row = 0;
            LCD16_Clear();
        }
    }

    LCD16_SetCursor(lcd_col, lcd_row);
    LCD16_PrintStr(note);
    lcd_col += strlen(note);
}

void PlayNoteHistory() {
    LCD16_Clear();
    uint8_t cursor_x = 0;
    uint8_t cursor_y = 0;

    for (uint8_t i = 0; i < note_index; i++) {
        const char* note = note_history[i];
        uint8_t j;

        // ?  ?  �??? ?  ? ?��??? index
        for (j = 0; j < 10; j++) {
            if ((strcmp(note, "0") == 0 && j == 0) ||
                (strcmp(note, "1") == 0 && j == 1) ||
                (strcmp(note, "2") == 0 && j == 2) ||
                (strcmp(note, "3") == 0 && j == 3) ||
                (strcmp(note, "4") == 0 && j == 4) ||
                (strcmp(note, "5") == 0 && j == 5) ||
                (strcmp(note, "6") == 0 && j == 6) ||
                (strcmp(note, "7") == 0 && j == 7) ||
                (strcmp(note, "8") == 0 && j == 8) ||
                (strcmp(note, "9") == 0 && j == 9)) {
                break;
            }
        }

        if (j < 10) {
        	float adjusted_freq = freq[j] * pow(2.0f, pitch_offsets[j] / 12.0f);
        	__HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
        	int width = (int)(CLOCK_FREQ / adjusted_freq / 2);

            __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);

            // 顯示?  ?  ?? 游�?? ? �???
            LCD16_SetCursor(cursor_x, cursor_y);
            LCD16_PrintStr(note);

            // �??? ? 游�?? ? �???
            cursor_x += 1;

            // ??  ?  ?  ?  ??
            if (cursor_x > 15) {
                cursor_x = 0;
                cursor_y++;
            }
            if (cursor_y > 1) {
                HAL_Delay(300);
                LCD16_Clear();
                cursor_x = 0;
                cursor_y = 0;
            }

            HAL_Delay(300);
        }
    }

    // ?  ?  �??? ?  ? ?���??? ? ?��?  ?
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    LCD16_Clear();
}

void Play_Song(const char *notes) {
    LCD16_Clear();
    uint8_t lcd_col = 0, lcd_row = 0;

    LCD16_SetCursor(0, 0);
    LCD16_PrintStr("Playing...");
    LCD16_SetCursor(0, 1);
    LCD16_PrintStr("#=Stop");

    HAL_Delay(1000);
    LCD16_Clear();

    char buffer[10];
    const char *ptr = notes;
    uint8_t interrupted = 0;

    while (*ptr && !interrupted) {
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;

        char note = *ptr++;
        if (note < '0' || note > '9') continue;

        int duration = 400;
        if (*ptr == ':') {
            ptr++;
            char duration_char = *ptr++;
            if (duration_char == '8') duration = 200;
            else if (duration_char == '4') duration = 400;
        }

        uint8_t index = note - '0';
        __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / freq[index]));
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (int)(CLOCK_FREQ / freq[index] / 2));

        // 顯示?���???
        buffer[0] = note;
        buffer[1] = '\0';
        LCD16_SetCursor(lcd_col, lcd_row);
        LCD16_PrintStr(buffer);

        lcd_col++;
        if (lcd_col >= 16) {
            lcd_col = 0;
            lcd_row++;
            if (lcd_row >= 2) {
                HAL_Delay(300);       // 給使?��?��???��?��??
                LCD16_Clear();
                lcd_row = 0;
            }
        }

        // ?��?��等�?�並?��中斷
        uint32_t start_time = HAL_GetTick();
        while ((HAL_GetTick() - start_time) < duration && !interrupted) {
            if (Read_Keypad() == '#') interrupted = 1;
            HAL_Delay(10);
        }

        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);

        if (!interrupted) {
            start_time = HAL_GetTick();
            while ((HAL_GetTick() - start_time) < 50 && !interrupted) {
                if (Read_Keypad() == '#') interrupted = 1;
                HAL_Delay(10);
            }
        }
    }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    LCD16_Clear();
    LCD16_PrintStr(interrupted ? "Stopped!" : "Finished!");
    HAL_Delay(500);
    LCD16_Clear();
}


uint8_t Play_Song_With_Interrupt(const char *notes) {
    LCD16_Clear();
    uint8_t lcd_col = 0, lcd_row = 0;
    char buffer[2];
    const char *ptr = notes;
    uint8_t interrupted = 0;

    while (*ptr && !interrupted) {
        while (*ptr == ' ') ptr++;
        if (!*ptr) break;

        char note = *ptr++;
        if (note < '0' || note > '9') continue;

        int duration = 400;
        if (*ptr == ':') {
            ptr++;
            char duration_char = *ptr++;
            if (duration_char == '8') duration = 200;
            else if (duration_char == '4') duration = 400;
        }

        // ?��?��?���???
        uint8_t index = note - '0';
        __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / freq[index]));
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, (int)(CLOCK_FREQ / freq[index] / 2));

        // 顯示?���???
        buffer[0] = note;
        buffer[1] = '\0';
        LCD16_SetCursor(lcd_col, lcd_row);
        LCD16_PrintStr(buffer);

        lcd_col++;
        if (lcd_col >= 16) {
            lcd_col = 0;
            lcd_row++;
            if (lcd_row >= 2) {
                HAL_Delay(300);  // ??��?��?�給使用?��??
                LCD16_Clear();
                lcd_row = 0;
            }
        }

        // 等�?�播?��??��?�並?��測中?��
        uint32_t start_time = HAL_GetTick();
        while ((HAL_GetTick() - start_time) < duration && !interrupted) {
            if (Read_Keypad() == '#') interrupted = 1;
            HAL_Delay(10);
        }

        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);

        if (!interrupted) {
            start_time = HAL_GetTick();
            while ((HAL_GetTick() - start_time) < 50 && !interrupted) {
                if (Read_Keypad() == '#') interrupted = 1;
                HAL_Delay(10);
            }
        }
    }

    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);
    return interrupted;
}

uint8_t rx_buffer[32];
//char uart_buffer[32];
//
//void UART_ReceiveAndPrint(void)
//{
//    HAL_UART_Receive(&huart3, (uint8_t *)uart_buffer, sizeof(uart_buffer), HAL_MAX_DELAY);
//    LCD16_PrintStr(uart_buffer); // 顯示字串
//}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
  MX_TIM3_Init();
  MX_I2C1_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);

  LCD16_Init(&hi2c1);
  LCD16_SetCursor(0, 0);
  HAL_ADC_Start(&hadc1);
//  HAL_UART_Receive_IT(&huart3, song_buffer, 10);
  HAL_UART_Receive_IT(&huart3, rx_buffer, 16);


//     LCD16_PrintStr("Hello");
//
//     LCD16_SetCursor(0, 1);
//     LCD16_PrintStr("World!");
  uint16_t width;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
//	  for (int i=0; i<8; i++){
//	  		  __HAL_TIM_SET_AUTORELOAD(&htim3, (int) (CLOCK_FREQ/freq[i]));
//	  		  int width = (int)(CLOCK_FREQ/freq[i]/2);
//	  		  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
//	  		  HAL_Delay(1000);
//	  	  }
//	  UART_ReceiveAndPrint();
	  char key = Read_Keypad();

	      if (key != 0)
	      {
	          // ??  ?  ? ?��?
	          HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED_2_Pin | LED_3_Pin, GPIO_PIN_RESET);

	          switch (key)
	          {
	          case '1':
				  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[1] * pow(2.0f, pitch_offsets[1] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);

					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("1");
				  }
	              HAL_GPIO_WritePin(GPIOB, LED_1_Pin, GPIO_PIN_SET);
	              HAL_Delay(300);

	              break;
	          case '2':
				  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[2] * pow(2.0f, pitch_offsets[2] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);

					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("2");
				  }
				  HAL_GPIO_WritePin(GPIOB, LED_2_Pin, GPIO_PIN_SET);
				  HAL_Delay(300);
	              break;
	          case '3':
	        	  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[3] * pow(2.0f, pitch_offsets[3] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);

					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("3");
				  }
	              HAL_GPIO_WritePin(GPIOB, LED_3_Pin, GPIO_PIN_SET);
	              HAL_Delay(300);
	              break;
	          case '4':
	        	  if (awaiting_song_selection) {
	        	  					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[4] * pow(2.0f, pitch_offsets[4] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);

					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("4");
				  }
	              HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED_2_Pin, GPIO_PIN_SET);
	              HAL_Delay(300);
	              break;
	          case '5':
	              if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[5] * pow(2.0f, pitch_offsets[5] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("5");
				  }
	              HAL_Delay(300);
	              break;
	          case '6':
	        	  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[6] * pow(2.0f, pitch_offsets[6] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("6");
				  }
	              HAL_GPIO_WritePin(GPIOB, LED_1_Pin | LED_3_Pin, GPIO_PIN_SET);
	              HAL_Delay(300);
	              break;
	          case '7':
	        	  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[7] * pow(2.0f, pitch_offsets[7] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("7");
				  }
	              HAL_GPIO_TogglePin(GPIOB, LED_1_Pin);
	              HAL_Delay(200);
	              HAL_GPIO_TogglePin(GPIOB, LED_1_Pin);
	              HAL_Delay(100);
	              break;
	          case '8':
	        	  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[8] * pow(2.0f, pitch_offsets[8] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("8");
				  }
	              HAL_GPIO_TogglePin(GPIOB, LED_2_Pin);
	              HAL_Delay(200);
	              HAL_GPIO_TogglePin(GPIOB, LED_2_Pin);
	              HAL_Delay(100);
	              break;
	          case '9':

	        	  if (awaiting_song_selection) {
					  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
				  else {
					  float adjusted_freq = freq[9] * pow(2.0f, pitch_offsets[9] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("9");
				  }
	              HAL_GPIO_TogglePin(GPIOB, LED_3_Pin);
	              HAL_Delay(200);
	              HAL_GPIO_TogglePin(GPIOB, LED_3_Pin);
	              HAL_Delay(100);
	              break;
	          case '0':


	        	  if (awaiting_song_selection) {
	        	  	  uint8_t song_id = key - '0';
					  LCD16_Clear();
					  LCD16_SetCursor(0, 0);
					  LCD16_PrintStr("Playing song ");
					  LCD16_PrintStr((char[]){key, '\0'});
					  HAL_Delay(1000);

					  Play_Song(songs[song_id]);
					  awaiting_song_selection = 0;
					  lcd_row=0;
					  lcd_col=0;
				  }
	        	  else {
	        		  float adjusted_freq = freq[0] * pow(2.0f, pitch_offsets[0] / 12.0f);
					  __HAL_TIM_SET_AUTORELOAD(&htim3, (int)(CLOCK_FREQ / adjusted_freq));
					  width = (int)(CLOCK_FREQ / adjusted_freq / 2);
					  __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, width);
					  LCD16_PrintNoteStr("0");
				  }
	        	  HAL_Delay(300);
	              // ?  ?  ??  ?  ? �????? �??? ?????????????
	              break;
	          case 'A':  // 跑馬???
	        	  awaiting_song_selection = 1;
				  LCD16_Clear();
				  LCD16_SetCursor(0, 0);
				  LCD16_PrintStr("Select song 0-9");
				  LCD16_SetCursor(0, 1);
				  LCD16_PrintStr("Press # to stop");
				  break;


	              for (int i = 0; i < 3; i++)
	              {
	                  HAL_GPIO_WritePin(GPIOB, LED_1_Pin, GPIO_PIN_SET);
	                  HAL_Delay(100);
	                  HAL_GPIO_WritePin(GPIOB, LED_1_Pin, GPIO_PIN_RESET);
	                  HAL_GPIO_WritePin(GPIOB, LED_2_Pin, GPIO_PIN_SET);
	                  HAL_Delay(100);
	                  HAL_GPIO_WritePin(GPIOB, LED_2_Pin, GPIO_PIN_RESET);
	                  HAL_GPIO_WritePin(GPIOB, LED_3_Pin, GPIO_PIN_SET);
	                  HAL_Delay(100);
	                  HAL_GPIO_WritePin(GPIOB, LED_3_Pin, GPIO_PIN_RESET);
	              }
	              break;
	          case 'B':
	          {
	              for (int i = 0; i < 10; i++) {
	                  LCD16_Clear();
	                  LCD16_SetCursor(0, 0);
	                  LCD16_PrintStr("Set pitch for ");
	                  LCD16_SetCursor(14, 0);
	                  LCD16_PrintChar('0' + i);  // 顯示 0~9 ?��符�?��?�數�???

	                  int last_offset = -100; // ??��?��?�為不可?��??��?��?�確保第�???次�?定�?�顯�???
	                  while (1) {
	                      uint32_t adc_val = Read_Potentiometer();
	                      int offset = (adc_val * 11) / 4096 - 5; // -5 ~ +5

	                      if (offset != last_offset) {
	                          char buf[16];
	                          sprintf(buf, "Offset: %+d   ", offset);  // 顯示?��式�??+3??-5 �???
	                          LCD16_SetCursor(0, 1);
	                          LCD16_PrintStr(buf);
	                          last_offset = offset;
	                      }

	                      char key = Read_Keypad();
	                      if (key == '#') {
	                          pitch_offsets[i] = offset;
	                          break;
	                      }
	                  }
	              }

	              LCD16_Clear();
	              LCD16_SetCursor(0, 0);
	              LCD16_PrintStr("Pitch Set Done");
	              HAL_Delay(1000);
	              LCD16_Clear();
	              LCD16_SetCursor(0, 0);
	              lcd_row=0;
	              lcd_col=0;
	          }
	          break;
	          case 'C':  // 歌曲?��?��?��?��??�能
	          {
	              uint8_t song_selected = 0;
	              uint32_t last_key_time = HAL_GetTick();
	              uint8_t global_interrupt = 0;  // ?���???中断??��??

	              LCD16_Clear();
	              LCD16_SetCursor(0, 0);
	              LCD16_PrintStr("C:0-9=Song");
	              LCD16_SetCursor(0, 1);
	              LCD16_PrintStr("*=Loop #=Random");

	              // 等�?�用?��输入，带超时
	              while (!song_selected && (HAL_GetTick() - last_key_time < 5000) && !global_interrupt) {
	                  char key = Read_Keypad();

	                  if (key != 0) {
	                      last_key_time = HAL_GetTick();

	                      if (key == '*') {  // 循环?��?��????��?�曲
	                          LCD16_Clear();
	                          LCD16_SetCursor(0, 0);
	                          LCD16_PrintStr("Looping All");
	                          LCD16_SetCursor(0, 1);
	                          LCD16_PrintStr("#=Stop");

	                          uint8_t loop_exit = 0;
	                          while (!loop_exit && !global_interrupt) {
	                              for (int i = 0; i < MAX_SONGS && !loop_exit && !global_interrupt; i++) {
	                                  global_interrupt = Play_Song_With_Interrupt(songs[i]);

	                                  // �????��?��?��要�?�止
	                                  if (Read_Keypad() == '#') {
	                                      loop_exit = 1;
	                                  }
	                                  HAL_Delay(300);  // 歌曲?��延�??
	                              }
	                          }
	                          song_selected = 1;
	                      }
	                      else if (key == '#') {  // ??�机?��?��
	                          LCD16_Clear();
	                          LCD16_SetCursor(0, 0);
	                          LCD16_PrintStr("Random Play");
	                          LCD16_SetCursor(0, 1);
	                          LCD16_PrintStr("#=Stop");

	                          srand(HAL_GetTick());  // ??��?��?��?�机种�??
	                          uint8_t random_exit = 0;

	                          for (int i = 0; i < 20 && !random_exit && !global_interrupt; i++) {
	                              int r = rand() % MAX_SONGS;
	                              global_interrupt = Play_Song_With_Interrupt(songs[r]);

	                              // �????��?��?��要�?�止
	                              if (Read_Keypad() == '#') {
	                                  random_exit = 1;
	                              }
	                              HAL_Delay(300);  // 歌曲?��延�??
	                          }
	                          song_selected = 1;
	                      }
	                  }
	                  HAL_Delay(50);  // ?��止�?��?��?��?��?测�?�键
	              }

	              // 清�?�显�???
	              LCD16_Clear();
	              if (global_interrupt) {
	                  LCD16_SetCursor(0, 0);
	                  LCD16_PrintStr("Stopped by User");
	                  HAL_Delay(1000);
	              }
	              else if (!song_selected) {
	                  LCD16_SetCursor(0, 0);
	                  LCD16_PrintStr("Timeout");
	                  HAL_Delay(1000);
	              }
	              LCD16_Clear();
	              break;
	          }
	          case 'D':
	              LCD16_Clear();
	              LCD16_PrintStr("Recording...");
	              LCD16_SetCursor(0, 1);
	              LCD16_PrintStr("Please speak");

	              HAL_UART_Transmit(&huart3, (uint8_t *)"START_RECORDING\n", strlen("START_RECORDING\n"), 100);
	              break;
	          case '*':  // �??? ?  ?  ? ?��?
	        	  PlayNoteHistory();
	        	  lcd_row = 0;
	        	  lcd_col = 0;
	              for (int i = 0; i < 3; i++) {
	                  HAL_GPIO_TogglePin(GPIOB, LED_1_Pin | LED_2_Pin | LED_3_Pin);
	                  HAL_Delay(100);
	              }
	              break;
	          case '#':  // ??  ?  ????
	        	  awaiting_song_selection=0;
	        	  LCD16_Clear();
	        	  lcd_row=0;
	        	  lcd_col=0;
	        	  for (int i = 0; i < 10; i++) {
					  pitch_offsets[0] = 0;
				  }
	        	  for (int i = 0; i < 50; i++) {
	        	      note_history[i][0] = '\0'; // ??�填�??? " " 也可�???
	        	  }

	        	  LCD16_PrintStr("Note cleared!");
	        	  note_index=0;
	        	  HAL_Delay(1000);
	        	  LCD16_Clear();
	              HAL_GPIO_TogglePin(GPIOB, LED_1_Pin);
	              HAL_GPIO_TogglePin(GPIOB, LED_2_Pin);
	              HAL_GPIO_TogglePin(GPIOB, LED_3_Pin);

	              break;
	          }

	      }
	      __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_3, 0);


    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_10;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 84-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 4;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, LED_1_Pin|LED_3_Pin|LED_2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : USER_Btn_Pin */
  GPIO_InitStruct.Pin = USER_Btn_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(USER_Btn_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : PA1 PA2 PA3 PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED_1_Pin LED_3_Pin LED_2_Pin */
  GPIO_InitStruct.Pin = LED_1_Pin|LED_3_Pin|LED_2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PBPin PBPin PBPin PBPin */
  GPIO_InitStruct.Pin = GPIO_PIN_1_Pin|GPIO_PIN_2_Pin|GPIO_PIN_3_Pin|GPIO_PIN_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    if (huart->Instance == USART3)
//    {
//        for (int i = 0; i < 10; i++) {
//            if (strncmp((char *)password_buffer, song_passwords[i], strlen(song_passwords[i])) == 0) {
//                const char *msg = "Playing...\r\n";
//                HAL_UART_Transmit(&huart3, (uint8_t *)msg, strlen(msg), 10);
//                Play_Song_With_Interrupt(songs[i]);
//                break;
//            }
//        }
//
//        HAL_UART_Receive_IT(&huart3, password_buffer, sizeof(password_buffer));
//    }
//}



void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART3)
    {
        rx_buffer[15] = '\0';  // 加結尾
        LCD16_Clear();
        LCD16_SetCursor(0, 0);
        LCD16_PrintStr("Now Playing:");
        LCD16_SetCursor(0, 1);
        LCD16_PrintStr((char *)rx_buffer);

        // 根據歌曲名稱找到索引
        int song_index = Match_Song_Name((char *)rx_buffer);
        if (song_index >= 0) {
            Play_Song_With_Interrupt(songs[song_index]);
        }

        HAL_UART_Receive_IT(&huart3, rx_buffer, 16);  // 重啟接收
    }
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
