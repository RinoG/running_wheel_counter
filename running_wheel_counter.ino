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

#include "cpp_queue\queue.h"
#include "cpp_queue\queue.cpp"

int resistor[5];
const int encoder_input[5] = {1, 2, 3, 4, 5};
const int analog_pin[5] = {A0, A1, A2, A3, A4};
const int resistor_input[5][2] = {{12, 11}, {7, 6}, {15, 16}, {25, 27}, {33, 35}};

unsigned long prev_time_ms[5] = {0};

Queue * queue_wheel_1;
Queue * queue_wheel_2;
Queue * queue_wheel_3;
Queue * queue_wheel_4;
Queue * queue_wheel_5;


/*****************************************************************************
 * @brief setup
 *****************************************************************************/
void setup() {
  Serial.begin(57600);
  
  queue_wheel_1 = new Queue();
  queue_wheel_2 = new Queue();
  queue_wheel_3 = new Queue();
  queue_wheel_4 = new Queue();
  queue_wheel_5 = new Queue();

  for (int i = 0; i <= 4; i++) {
    /* Initialize inputs from encoder (detect motor rotation) */
    pinMode(encoder_input[i], INPUT);
  
    /* Initialize inputs to detect which resistor is on */
    pinMode(resistor_input[i][0], INPUT);
    pinMode(resistor_input[i][1], INPUT);
  }
  
  /* Initialize Interrupts to count wheel rotations */
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

  if (Serial) {
    /* write data in log-file if size of Queue has changed */
    if(queue_wheel_1->getSize() > 0) {
      write_log(encoder_input[0],queue_wheel_1->dequeue(), counter[0]++, queue_wheel_1->dequeue(), get_resistor(resistor_input[0]));
    }
  
    if(queue_wheel_2->getSize() > 0) {
      write_log(encoder_input[1],queue_wheel_2->dequeue(), counter[1]++, queue_wheel_2->dequeue(), get_resistor(resistor_input[1]));
    }
  
    if(queue_wheel_3->getSize() > 0) {
      write_log(encoder_input[2],queue_wheel_3->dequeue(), counter[2]++, queue_wheel_3->dequeue(), get_resistor(resistor_input[2]));
    }
  
    if(queue_wheel_4->getSize() > 0) {
      write_log(encoder_input[3],queue_wheel_4->dequeue(), counter[3]++, queue_wheel_4->dequeue(), get_resistor(resistor_input[3]));
    }
  
    if(queue_wheel_5->getSize() > 0) {
      write_log(encoder_input[4],queue_wheel_5->dequeue(), counter[4]++, queue_wheel_5->dequeue(), get_resistor(resistor_input[4]));
    }
  }
}

/*****************************************************************************
 * @brief wheel interrupt hanlder 1-5
 *****************************************************************************/
void handle_wheel_1() { handle_wheel(queue_wheel_1, &prev_time_ms[0], analog_pin[0]); }
void handle_wheel_2() { handle_wheel(queue_wheel_2, &prev_time_ms[1], analog_pin[1]); }
void handle_wheel_3() { handle_wheel(queue_wheel_3, &prev_time_ms[2], analog_pin[2]); }
void handle_wheel_4() { handle_wheel(queue_wheel_4, &prev_time_ms[3], analog_pin[3]); }
void handle_wheel_5() { handle_wheel(queue_wheel_5, &prev_time_ms[4], analog_pin[4]); }

/*****************************************************************************
 * @brief get_resistor
 *****************************************************************************/
int get_resistor(const int resistor_input[]) {
  int resistor_value[2]; int R;
  resistor_value[0] = digitalRead(resistor_input[0]);
  resistor_value[1] = digitalRead(resistor_input[1]);
  
  if      (resistor_value[1] == 0 && resistor_value[0] == 0) R = 1;
  else if (resistor_value[1] == 1 && resistor_value[0] == 0) R = 10;
  else if (resistor_value[1] == 0 && resistor_value[0] == 1) R = 100;
  else if (resistor_value[1] == 1 && resistor_value[0] == 1) R = 1000;
  else R = 0;

  return R;
}

/*****************************************************************************
 * @brief handle_wheel
 *****************************************************************************/
void handle_wheel(Queue * queue, unsigned long * prev_handle_time_ms, int analog_in)
{
  /* Debounce interrupt input, otherwise plugging in the connector will also be interpreted as a signal from the encoder. */
  unsigned long timediff_ms = millis() - (* prev_handle_time_ms);
  if (timediff_ms > 1) {
    /* write elapsed time in the Queue */
    queue->enqueue(millis());

    /* write voltage in the Queue */
    unsigned long voltage = analogRead(analog_in);
    queue->enqueue(voltage);
  }
  (* prev_handle_time_ms) = millis();
}


/*****************************************************************************
 * @brief write_log
 *****************************************************************************/
void write_log(int wheel, unsigned long t, long count, unsigned long v, int res) {
  // write data in log file
  // [Wheel Number];[Time elapsed];[counter];[Voltage];[Resistor Value]
  Serial.print(wheel);
  Serial.print(";");
  Serial.print(t);
  Serial.print(";");
  Serial.print(count);
  Serial.print(";");
  Serial.print(v);
  Serial.print(";");
  Serial.println(res);
}
