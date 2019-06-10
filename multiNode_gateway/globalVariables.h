// ---------- These are some global variables used throughout the program. ----------
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

#define SerialUBLOX Serial1

uint16_t batteryVoltage = 0;
uint32_t connectionStartTime = 0;
uint32_t connectionTime = 0;
uint32_t LoRa_listenStartTime = 0;
uint32_t LoRa_listenTime = 0;

uint8_t hoursNow = 0;
uint8_t minutesNow = 0;
uint8_t yearNow = 0;
uint8_t monthNow = 0;
uint8_t dateNow = 0;

uint32_t temperature = 0;

uint8_t buf[20] = {0};
uint8_t flag = 0;

uint8_t alarmNowMinutes = 0;
uint8_t alarmNowSeconds = 0;

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
