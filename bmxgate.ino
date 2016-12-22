// NeoPixel Ring simple sketch (c) 2013 Shae Erisson
// released under the GPLv3 license to match the rest of the AdaFruit NeoPixel library

#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define DELAY_DROP_TONE_MS 60
#define DELAY_DROP_TONE_FINAL_MS 2250
#define DELAY_GATE_RISE_WARN_MS 25
#define DELAY_ABORT_TONE_1_MS 220
#define DELAY_ABORT_TONE_2_MS 440

#define DELAY_RAND_MIN 100
#define DELAY_RAND_MAX 2700

#define TONE_DROP_HZ 632
#define TONE_GATE_RISE 1150
#define TONE_ABORT_1_HZ 740
#define TONE_ABORT_2_HZ 680

// Which pin on the Arduino is connected to the NeoPixels?
// On a Trinket or Gemma we suggest changing this to 1
#define PIN_NEO_PIXEL 7
#define PIN_BUTTON_GO 2
#define PIN_SPEAKER 5
#define PIN_RELAY 4
#define PIN_GATE_STATUS_LED_RED 18
#define PIN_GATE_STATUS_LED_GREEN 19

// How many NeoPixels are attached to the Arduino?
#define NUMPIXELS      8

//TMRpcm tmrpcm;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
// Note that for older NeoPixel strips you might need to change the third parameter--see the strandtest
// example for more information on possible values.
Adafruit_NeoPixel strip = Adafruit_NeoPixel(8, PIN_NEO_PIXEL, NEO_RGBW + NEO_KHZ800);

struct gate_step {
  int tone_length;
  int wait_length;
  int light_num;
};
struct gate_step gate_steps[] =  {
  { DELAY_DROP_TONE_MS, 0, 1 },
  { 0, 60, 0 },
  { DELAY_DROP_TONE_MS, 0, 2 },
  { 0, 60, 0 },
  { DELAY_DROP_TONE_MS, 0, 3 },
  { 0, 60, 0 },
  { DELAY_DROP_TONE_FINAL_MS, 0, 4 }
};
int gate_step_count = 7; // can't programatically count dynamic array

int delayval = 500; // delay for half a second
int FLAG_BUTTON_PRESSED = 1;
int FLAG_ABORT_DROP = 0;
int FLAG_SEQUENCE_RUNNING = 0;
int FLAG_GATE_ARMED = 0; // gate it locked, ready for release trigger

int currentStep = 0;

uint32_t ORANGE = strip.Color(255, 100, 0);
uint32_t GREEN = strip.Color(0, 255, 0);
uint32_t RED = strip.Color(255, 0, 0, 0);
uint32_t YELLOW = strip.Color(255, 255, 0, 10);
uint32_t BLUE = strip.Color(0, 0, 255);
uint32_t WHITE = strip.Color(0, 0, 0, 255);

  /*chase(strip.Color(255, 0, 0)); // Red
  chase(strip.Color(0, 255, 0)); // Green
  chase(strip.Color(0, 0, 255)); // Blue
  chase(strip.Color(44, 42, 0)); // yellow
  chase(strip.Color(24, 0, 24)); // magenta
  chase(strip.Color(44, 21, 0)); // redest orange
  chase(strip.Color(255, 40, 5)); // orange
  */

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(PIN_SPEAKER, OUTPUT);
  pinMode(PIN_BUTTON_GO, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);
  attachInterrupt(1, Interrupt1, RISING);
  digitalWrite(3, HIGH); // interrupt on 3
  digitalWrite(PIN_RELAY, LOW);
  strip.begin();
  strip.setBrightness(255);
  //pinMode(PIN_BUTTON_GO,INPUT);
  //chase(BLUE);

  //set_ready();

  //tmrpcm.PIN_SPEAKER = PIN_SPEAKER;

  //begin_sequence();
  /*Serial.begin(9600);
  if (!SD.begin(SD_ChipSelectPin)) {
    //Serial.println("SD fail");
    tone(PIN_SPEAKER, 100, 2);
    return;
  }
  //tmrpcm.setVolume(6);
  tmrpcm.play("1.wav");*/
}
 
void loop() {

 /* strip.setPixelColor(0, RED);
  strip.setPixelColor(1, RED);
  strip.setPixelColor(2, RED);
  strip.setPixelColor(3, RED);
  strip.setPixelColor(4, RED);
  strip.setPixelColor(5, RED);
  strip.setPixelColor(6, RED);
  strip.setPixelColor(7, RED);
  strip.show();
  return;*/
  
  /*if (digitalRead(PIN_BUTTON_GO) == HIGH)
  {
    chase(GREEN);   // green
    tone(PIN_SPEAKER, TONE_DROP_HZ, 100);
    
  }
  else if (digitalRead(PIN_BUTTON_GO) == LOW)
  {
    
    chase(RED); // Red
  }
  return;*/

  if (digitalRead(PIN_BUTTON_GO) == LOW) {
    FLAG_BUTTON_PRESSED = 0; // reset
  }
  else if (digitalRead(PIN_BUTTON_GO) == HIGH)
  {
    if (FLAG_SEQUENCE_RUNNING) {
      FLAG_ABORT_DROP = 1;
    }
    if (!FLAG_BUTTON_PRESSED) {
      FLAG_BUTTON_PRESSED = 1;
      // do button press action
      //light_start_seq_led(getCurrentStep());
      begin_sequence();
      //abort_seq();
    }
    // else still holding button    
  }
}

int getCurrentStep() {
  currentStep++;
  if (currentStep > 4)
    currentStep = 1;
  return currentStep;
}

// likely needs an interrupt
void begin_sequence() {
  digitalWrite(LED_BUILTIN, HIGH);
  
  int start_time = millis();

  FLAG_SEQUENCE_RUNNING = 1;
  int step = 0;
  int playing_tone = 0;
  int waiting = 0;
  int timer_start = 0;

  //printf("array %lu \n", sizeof(&gate_steps)-1);
  int wait_timer = millis() + random(DELAY_RAND_MIN,DELAY_RAND_MAX+1);
  tone(PIN_SPEAKER,TONE_DROP_HZ,100);
  while((millis() < wait_timer) && (!FLAG_ABORT_DROP)) {
  }

  while((step < gate_step_count) && (!FLAG_ABORT_DROP)) {
    int now = millis();
    int timer = now - timer_start;

    if (playing_tone && (timer >= gate_steps[step].tone_length)) {
      // reset timer
      playing_tone = 0;
      stop_tone();
      step++;
      //printf("[%i] tone stop %i \n", timer, step);
      //timer = 0;
    }
    if (!playing_tone && (gate_steps[step].tone_length > 0)) {
      timer_start = now;
      start_tone(TONE_DROP_HZ);
      light_start_seq_led(gate_steps[step].light_num);
      playing_tone = 1;
    }
    if (waiting && (timer >= gate_steps[step].wait_length)) {
      waiting = 0;
      step++;
      //printf("[%i] wait stop %i \n", timer, step);
      //timer = 0;
    }
    if (!waiting && (gate_steps[step].wait_length > 0)) {
      timer_start = now;
      waiting = 1;
    }
    FLAG_SEQUENCE_RUNNING = 0;
    ////printf("step %i \n", step);
  }
  FLAG_SEQUENCE_RUNNING = 0;
  stop_tone();
  led_reset();
  if (FLAG_ABORT_DROP) {
    //abort_seq();
    FLAG_ABORT_DROP = 0;
  }

  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(PIN_RELAY, LOW);
}
 
static void chase(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels()+4; i++) {
      strip.setPixelColor(i  , c); // Draw new pixel
      strip.setPixelColor(i-4, 0); // Erase pixel a few steps back
      strip.show();
      delay(50);
  }
}

void light_start_seq_led(int step) {
  switch (step) {
    case 1:
      led_reset(); 
      strip.setPixelColor(0, RED);
      strip.setPixelColor(1, RED);
      break;
    case 2:
      strip.setPixelColor(2, YELLOW);
      strip.setPixelColor(3, YELLOW);
      break;
    case 3:
      strip.setPixelColor(4, YELLOW);
      strip.setPixelColor(5, YELLOW);
      break;
    case 4:
      drop_gate();
      strip.setPixelColor(6, GREEN);
      strip.setPixelColor(7, GREEN);
      break;
     default:
      led_reset();
  }
  strip.show();
}

void led_reset() {
  for(int i=0;i<8;i++) {
    strip.setPixelColor(i,0,0,0,0);
  }
  strip.show();
}

void start_tone(int hz) {
  tone(PIN_SPEAKER, TONE_DROP_HZ);
}

void stop_tone() {
  noTone(PIN_SPEAKER);
}

void abort_seq() {
  for(int i=0;i<8;i++) {
    strip.setPixelColor(i,GREEN);
  }
  strip.show();
  tone(PIN_SPEAKER, TONE_ABORT_1_HZ, DELAY_ABORT_TONE_1_MS);
  delay(DELAY_ABORT_TONE_1_MS);
  tone(PIN_SPEAKER, TONE_ABORT_2_HZ, DELAY_ABORT_TONE_2_MS);
  led_reset();
  delay(3000);
  set_ready();
}

void set_ready() {
  led_reset();
  strip.setPixelColor(0, BLUE);
  strip.show();
}

void drop_gate() {
  // if gate armed
    // trigger relay drop
    // set gate LED to green   
    digitalWrite(PIN_RELAY, HIGH);
}

// called when reed sensor triggered
void arm_gate() {
  // set LED to red

  // turn on electro magnet

  // beep
 
}

// interrrupt
void Interrupt1()
{
   FLAG_ABORT_DROP = 1;
  abort_seq();
}


