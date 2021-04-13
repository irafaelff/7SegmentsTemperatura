
#include "SevSeg.h"
#include "DHT.h"
#include <ThreeWire.h>
#include <RtcDS1302.h>

#define DHTPIN A0
#define DHTTYPE DHT11
#define MAX_NUMBER_STRINGS 3
#define MAX_STRING_SIZE 8
#define PATTERN_CHANGE_TIME 2000

ThreeWire myWire(A3, A1, A2); //DAT/IO, CLK/SCLK, RST/CE
RtcDS1302<ThreeWire> rtc(myWire);
DHT dht(DHTPIN, DHTTYPE);

//Create an instance of the object.
SevSeg myDisplay;

//Create global variables
int deciSecond = 0;
byte numDigits = 4;
byte digitPins[] = {2, 3, 4, 5};
byte segmentPins[] = {6, 7, 8, 9, 10, 11, 12, 13};
bool resistorsOnSegments = true;    // 'false' means resistors are on digit pins
byte hardwareConfig = COMMON_ANODE; // See README.md for options
bool updateWithDelays = true;       // Default 'false' is Recommended
bool leadingZeros = false;          // Use 'true' if you'd like to keep the leading zeros
bool disableDecPoint = false;       // Use 'true' if your decimal point doesn't exist or isn't connected

char valuesToDisplay[MAX_NUMBER_STRINGS][MAX_STRING_SIZE];

unsigned long timer = millis() - PATTERN_CHANGE_TIME;

float umidade;
float temperatura;
float sensacaoTermica;

byte qtdInformacao = 2;
void setup()
{
  Serial.begin(9600);
  rtc.Begin();

  // RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  // rtc.SetDateTime(compiled);
  int numberOfDigits = 4;
  dht.begin();
  myDisplay.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments, updateWithDelays, leadingZeros, disableDecPoint);

  myDisplay.setBrightness(10);

  timer = millis();
}

void loop()
{
  LerSensorTemperatura();
  PrepareValuesToDisplay();

  if (millis() > (timer + PATTERN_CHANGE_TIME))
  {
    myDisplay.setChars(valuesToDisplay[qtdInformacao]);
    qtdInformacao++;
    if (qtdInformacao >= MAX_NUMBER_STRINGS)
    {
      qtdInformacao = 0;
    }

    timer = millis();
  }

  myDisplay.refreshDisplay();
}

void LerSensorTemperatura()
{
  umidade = dht.readHumidity();
  temperatura = dht.readTemperature() * 10;
  sensacaoTermica = dht.computeHeatIndex(temperatura, umidade, false) * 10;

  if (isnan(umidade) || isnan(temperatura))
  {
    Serial.println("Falha ao ler o sensor DHT11!");
    return;
  }
}

void PrepareValuesToDisplay()
{
  RtcDateTime now = rtc.GetDateTime();
  char charTemperatura[5];
  char charUmidade[5];
  char charHora[5];
  int tempInt = temperatura;
  int umidadeInt = umidade;

  sprintf(charTemperatura, "%02dC", tempInt);
  sprintf(charUmidade, "%02d U", umidadeInt);
  sprintf(charHora, "%02d%02d", now.Hour(), now.Minute());

  strcpy(valuesToDisplay[0], charTemperatura);
  strcpy(valuesToDisplay[1], charUmidade);
  strcpy(valuesToDisplay[2], charHora);
}