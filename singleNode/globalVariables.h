// ---------- These are some global variables used throughout the program. ----------
// ---------- Some notes are added at the bottom of this file. ----------------------
// ---------- Functions with default arguments have also been prototyped here. ------


#define ADC_AREF 3.3f
#define BATVOLT_R1 4.7f
#define BATVOLT_R2 10.0f

#define SDI12_DATA_PIN 7         // The pin of the SDI-12 data bus
#define DEPTH_SENSOR_POWER 8

#define ROTATORY_CLK_PIN 2
#define ROTATORY_DT_PIN 3
#define ROTATORY_SW_PIN 4
#define ROTATORY_PLUS 5 // Is used as the ground of the encoder even though is says "+"
#define DISPLAY_POWER_MOSFET 6
#define TEMPERATURE_POWER_GROUND A1
#define SD_CARD_POWER_MOSFET 9
#define SerialUBLOX Serial1


// ----- Change these six values to set the current initial time and date -----
// ----- Be aware that you must also change the initial alarm if you change these. -----
uint8_t dummySeconds = 1;
uint8_t dummyMinutes = 1;
uint8_t dummyHours = 1;
uint8_t dummyDay = 1;
uint8_t dummyMonth = 1;
uint8_t dummyYear = 1;


// ----- Other global variables. -----
uint8_t hoursNow = 0;
uint8_t minutesNow = 0;
uint8_t yearNow = 0;
uint8_t monthNow = 0;
uint8_t dateNow = 0;

uint16_t batteryVoltage = 0;

uint32_t connectionStartTime = 0;
uint32_t connectionTime = 0;

uint32_t temperature = 0;

uint16_t unsent_Status[100] = {0};
uint16_t unsent_year[100] = {0};
uint16_t unsent_month[100] = {0};
uint16_t unsent_date[100] = {0};
uint16_t unsent_hour[100] = {0};
uint16_t unsent_minute[100] = {0};
uint16_t unsent_connectionTime[100] = {0};
uint16_t unsent_depth[100] = {0};
uint16_t unsent_batteryVoltage[100] = {0};
uint16_t unsent_temperature[100] = {0};
uint16_t unsent_counter[100] = {0};

uint16_t password = 1223;
uint16_t userInputPassword = 0;

uint32_t latitude = 0;
uint32_t longitude = 0;

uint32_t counter = 1;
uint32_t depth = 0;
uint32_t currentDepth = 0;
uint16_t sensorDepth = 0;
uint32_t depthCompensation = 0;
uint8_t depthCompensationSign = 0;


// ---------- Prototypes of functions with default arguments. ------------------
void sendToAllThingsTalk(uint32_t asset1Value = NULL, uint32_t asset2Value = NULL, uint32_t asset3Value = NULL, uint32_t asset4Value = NULL, uint32_t asset5Value = NULL, uint32_t asset6Value = NULL, uint32_t asset7Value = NULL, uint32_t asset8Value = NULL, uint32_t asset9Value = NULL, uint32_t asset10Value = NULL);
String numbersToCSVString(uint32_t data0, uint32_t data1 = NULL, uint32_t data2 = NULL, uint32_t data3 = NULL, uint32_t data4 = NULL, uint32_t data5 = NULL, uint32_t data6 = NULL, uint32_t data7 = NULL, uint32_t data8 = NULL, uint32_t data9 = NULL, uint32_t data10 = NULL);

/*
 * NOTES:
 * 
 * Note 1 - The mode switch and the ground of the temperature sensor are both connected to analog pins. These pins are connected internally via a multiplexer. 
 *        - The internal resistance between these two pins can at times be lesser than the resistance of the pullup-on A0 (mode switch).
 *        - If A1 (temperature ground) is low, A0 might read low as well even when it's supposed to be high.
 *        - A1 is therefore set high before reading A0. 
 *        - If A0 then is high, it read high. If it's supposed to be low, it's connected directly to ground and will therefore read low.
 *        - Set A1 low again (if the I2C bus is to be used) immediately after reading A0.
 *        
 */       
