#include "Arduino_feeder.h" 

void setup() {

   // setup right wheel pins 
  pinMode(MOTOR_ODOMETRY, INPUT);  // make pin 9 an input (odometry)  
  pinMode(MOTOR_PWMF,OUTPUT); // make pin 11 an output (PWM forward)
  pinMode(MOTOR_PWMR,OUTPUT); // make pin 3 an output (PWM reverse)
    
  Serial.begin(9600);
  
  //set PWM frequency to 3906 Hz for pin 3 (and 11)
  TCCR2B=(TCCR2B&0xF8) | 1; 

  // Setup motor PWM
  analogWrite(MOTOR_PWMF,120); //do 50% PWM on pin 11 at the frequency set in TCCR2B(right wheel) 

  // Setup the LCD  
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.setCursor(0,0);
  lcd.print("Motor Vel = 0");  
  lcd.setCursor(0,1);
  lcd.print("Set Req vel:");
  Serial.begin(9600);
 
}

void loop() 
{
  uint32_t t = millis();
  float current_velocity = 0.0;

  // Poll motor odometry
  current_velocity = calculateVelocity();
  
  // Call RPM controller
  if ((t-tTime[0]) >= (1000 / CONTROL_MOTOR_SPEED_FREQUENCY))
  {      
    tTime[0] = t;
  }

  menu_handler();
  
}

void menu_handler()
{
   int x;
  x = analogRead (0);
  lcd.setCursor(10,1);
 
  if (x < 100) 
  {
    lcd.print ("Right ");
    Serial.print("The value at pim A0 Right key pressed is  :");
    Serial.println(x,DEC);
  }
  else if (x < 200) {
    lcd.print ("Up    ");
    Serial.print("The value at pim A0 UP key pressed is  :");
    Serial.println(x,DEC);
  }
  
  else if (x < 400){
    lcd.print ("Down  ");
    Serial.print("The value at pim A0 Down key pressed is  :");
    Serial.println(x,DEC);
  }
  else if (x < 600){
    lcd.print ("Left  ");
    Serial.print("The value at pim A0 Left key pressed is  :");
    Serial.println(x,DEC);
  }
  else if (x < 800){
    lcd.print ("Select");
    Serial.print("The value at pim A0 Select key pressed is  :");
    Serial.println(x,DEC);
  }
}

 


float calculateVelocity()
{  

  float current_time;
  float time_diff; 
  
  // Poll the motor odometry pin 
  motor_pulses= digitalRead(MOTOR_ODOMETRY);
  current_time = millis();
  
  // Detect tranbsition and update odometry counter
  if(motor_pulses==0 && prev_motor_pulses== 1 )  //the point where high goes to low(end of the pulse) 
    motor_odo_counter = motor_odo_counter + 1;
  // Update prev motor pulses
  prev_motor_pulses=motor_pulses;
  
  if(motor_odo_counter>=82)
  {    
    // Calculate RPM & speed
    time_diff = current_time-motor_prev_time; //time difference in millisec 
    motor_rpm = (1000/time_diff)*60; 
    motor_velocity = (motor_rpm/60)*150*3.1415; 

    // Init for next time
    motor_prev_time = current_time;
    motor_odo_counter=0;
    
    Serial.print("rpm: ");
    Serial.println(motor_rpm);    
    Serial.print("mm/s of right wheel:");
    Serial.println(motor_velocity);
  } 

  return motor_velocity;
}

/*int calculateDistance(int pulses)
{
 return int(pulses*PULSES_2_MM);
}*/