
const int encoder_input[5] = {A0, A1, A2, A3, A4};
const int resistor_input[5][2] = {{2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}};

int resistor[5];
unsigned long counter[5] = {0, 0, 0, 0, 0};
unsigned long prev_counter[5] = {0, 0, 0, 0, 0};
unsigned long counter_time[5];
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
  attachInterrupt(encoder_input[0], count_function_1, FALLING);
  attachInterrupt(encoder_input[1], count_function_2, FALLING);
  attachInterrupt(encoder_input[2], count_function_3, FALLING);
  attachInterrupt(encoder_input[3], count_function_4, FALLING);
  attachInterrupt(encoder_input[4], count_function_5, FALLING);
}

// Interrupt functions to count wheel rotations
void count_function_1() {counter[0]++; counter_time[0] = millis(); }
void count_function_2() {counter[1]++; counter_time[1] = millis(); }
void count_function_3() {counter[2]++; counter_time[2] = millis(); }
void count_function_4() {counter[3]++; counter_time[3] = millis(); }
void count_function_5() {counter[4]++; counter_time[4] = millis(); }

void loop() {
  for (int i = 0; i <= 4; i++) {
    if(counter[i] > prev_counter[i]){
        resistor[i] = get_resistor(resistor_input[i]);
        write_log(encoder_input[i], counter_time[i], counter[i], resistor[i]);
        
        prev_counter[i] = counter[i];
    }
  }
}

int get_resistor(const int resistor_input[]) {
  int resistor_value[2]; int R;
  resistor_value[0] = digitalRead(resistor_input[0]);
  resistor_value[1] = digitalRead(resistor_input[1]);
  
  if      (resistor_value[1] == 0 && resistor_value[0] == 0) R = 1;
  else if (resistor_value[1] == 0 && resistor_value[0] == 1) R = 10;
  else if (resistor_value[1] == 1 && resistor_value[0] == 0) R = 100;
  else if (resistor_value[1] == 1 && resistor_value[0] == 1) R = 1000;

  return R;
}

void write_log(int wheel, long t, long count, int res) {
  // write data in log file
  // [Wheel Number];[Time in ms];[counter];[Resistor Value]
  Serial.print(wheel);       Serial.print(";");
  Serial.print(t);  Serial.print(";");
  Serial.print(count);     Serial.print(";");
  Serial.println(res);
}
