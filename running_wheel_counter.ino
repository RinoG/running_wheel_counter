/*****************************************************************************
 * @file running_wheel_counter.ino
 * @author RinoG
 * @brief 
 * @version 0.1
 * @date 2021-02-01
 * 
 * @copyright Copyright (c) 2021
 * 
 *****************************************************************************/

#include "cpp_queue/queue.h"

#define MAX_MS_DELAY_WHEEL_0 5000u
#define MAX_MS_DELAY_WHEEL_1 5000u
#define MAX_MS_DELAY_WHEEL_2 5000u
#define MAX_MS_DELAY_WHEEL_3 5000u
#define MAX_MS_DELAY_WHEEL_4 5000u

int resistor[5];
const int encoder_input[5] = {A0, A1, A2, A3, A4};
const int resistor_input[5][2] = {{2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}};

static unsigned long prev_time_ms[5] = {0};

Queue * timediff_queue_0;
Queue * timediff_queue_1;
Queue * timediff_queue_2;
Queue * timediff_queue_3;
Queue * timediff_queue_4;


/*****************************************************************************
 * @brief setup
 *****************************************************************************/
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  for (int i = 0; i <= 4; i++) {
    // Initialize inputs from encoder (detect motor rotation)
    pinMode(encoder_input[i], INPUT);
  
    // Initialize inputs to detect which resistor is on
    pinMode(resistor_input[i][0], INPUT);
    pinMode(resistor_input[i][1], INPUT);
  }
  
  // Initialize Interrupts to count wheel rotations
  attachInterrupt(encoder_input[0], handle_wheel_1, FALLING);
  attachInterrupt(encoder_input[1], handle_wheel_2, FALLING);
  attachInterrupt(encoder_input[2], handle_wheel_3, FALLING);
  attachInterrupt(encoder_input[3], handle_wheel_4, FALLING);
  attachInterrupt(encoder_input[4], handle_wheel_5, FALLING);
}

/*****************************************************************************
 * @brief loop
 *****************************************************************************/
void loop() {
  static long counter[5] = {0};
  /* is counter overflow ok? */
  
  if(timediff_queue_0->getSize() > 0)
    write_log(encoder_input[0],timediff_queue_0->dequeue, counter[0]++, get_resistor(resistor_input[0]))

  if(timediff_queue_1->getSize() > 0)
    write_log(encoder_input[1],timediff_queue_1->dequeue, counter[1]++, get_resistor(resistor_input[1]))

  if(timediff_queue_2->getSize() > 0)
    write_log(encoder_input[2],timediff_queue_2->dequeue, counter[2]++, get_resistor(resistor_input[2]))

  if(timediff_queue_3->getSize() > 0)
    write_log(encoder_input[3],timediff_queue_3->dequeue, counter[3]++, get_resistor(resistor_input[3]))

  if(timediff_queue_4->getSize() > 0)
    write_log(encoder_input[4],timediff_queue_4->dequeue, counter[4]++, get_resistor(resistor_input[4]))
}

/*****************************************************************************
 * @brief wheel interrupt hanlder 1-5
 *****************************************************************************/
void handle_wheel_1() { handle_wheel(timediff_queue_0, &prev_time_ms[0], MAX_MS_DELAY_WHEEL_0) }
void handle_wheel_2() { handle_wheel(timediff_queue_1, &prev_time_ms[1], MAX_MS_DELAY_WHEEL_1) }
void handle_wheel_3() { handle_wheel(timediff_queue_2, &prev_time_ms[2], MAX_MS_DELAY_WHEEL_2) }
void handle_wheel_4() { handle_wheel(timediff_queue_3, &prev_time_ms[3], MAX_MS_DELAY_WHEEL_3) }
void handle_wheel_5() { handle_wheel(timediff_queue_4, &prev_time_ms[4], MAX_MS_DELAY_WHEEL_4) }

/*****************************************************************************
 * @brief get_resistor
 *****************************************************************************/
int get_resistor(const int resistor_input[]) {
  int resistor_value[2]; int R;
  resistor_value[0] = digitalRead(resistor_input[0]);
  resistor_value[1] = digitalRead(resistor_input[1]);
  
  if      (resistor_value[1] == 0 && resistor_value[0] == 0) R = 1;
  else if (resistor_value[1] == 0 && resistor_value[0] == 1) R = 10;
  else if (resistor_value[1] == 1 && resistor_value[0] == 0) R = 100;
  else if (resistor_value[1] == 1 && resistor_value[0] == 1) R = 1000;
  else R = 0;

  return R;
}

/*****************************************************************************
 * @brief handle_wheel
 *****************************************************************************/
void handle_wheel(Queue * queue, unsigned long * prev_handle_time_ms, unsigned long max_delay_ms)
{
  long timediff_ms = millis() - (* prev_handle_time_ms);
  if( (timediff_ms > 0) && (timediff_ms < max_delay_ms) )
    queue->enqueue(timediff_ms);
  (* prev_handle_time_ms) = millis();
}


/*****************************************************************************
 * @brief write_log
 *****************************************************************************/
void write_log(int wheel, long t, long count, int res) {
  // write data in log file
  // [Wheel Number];[Timediff in ms];[counter];[Resistor Value]
  Serial.print(wheel);       Serial.print(";");
  Serial.print(t);  Serial.print(";");
  Serial.print(count);     Serial.print(";");
  Serial.println(res);
}
