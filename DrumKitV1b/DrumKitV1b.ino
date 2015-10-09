#define DODEBUG false
//*******************************************************************************************************************
// User settable variables			
//*******************************************************************************************************************

const unsigned char ACOUSTICBASSDRUM = 35;
const unsigned char ACOUSTICSNARE = 38;
const unsigned char OPENHIGHHAT = 46;
const unsigned char CLOSEDHIGHHAT = 42;
const unsigned char PEDALHIGHHAT = 44;
const unsigned char LOWTOM = 45;
const unsigned char HIGHTOM = 50;
const unsigned char CRASHSYM = 49;

unsigned char PadNote[6] = { CRASHSYM,ACOUSTICBASSDRUM,OPENHIGHHAT,ACOUSTICSNARE,LOWTOM,HIGHTOM };         // MIDI notes from 0 to 127 (Mid C = 60)
unsigned char PadNoteSwitched[6] = { CRASHSYM,ACOUSTICBASSDRUM,CLOSEDHIGHHAT,ACOUSTICSNARE,LOWTOM,HIGHTOM };         // MIDI notes from 0 to 127 (Mid C = 60)

int PadCutOff[6] = { 30,30,30,30,30,30 };           // Minimum Analog value to cause a drum hit

unsigned long MaxPlayTime[6] = { 100,100,100,100,100,100 };               // Cycles before a 2nd hit is allowed
unsigned long SampleTime[6] = { 10,10,10,10,10,10 };               // Cycles before a sample max is taken
bool MidiOn[6] = { false,false,false,false,false,false };
boolean Switches[6] = { 0,0,0,0,0,0 };

#define  midichannel	9;                              // MIDI channel from 0 to 15 (+1 in "real world")

boolean VelocityFlag = true;                           // Velocity ON (true) or OFF (false)

int Amplifier[6] = { 50,100,100,50,100,100 };           // Minimum Analog value to cause a drum hit
int dAmplifier[6] = { 100,50,100,50,100,100 };           // Minimum Analog value to cause a drum hit

const int footSwitch = 4;
const int footSwitchNote = 2;


//*******************************************************************************************************************
// Internal Use Variables			
//*******************************************************************************************************************

boolean activePad[7] = { 0,0,0,0,0,0,0 };                   // Array of flags of pad currently playing
unsigned long PinPlayTime[7] = { 0,0,0,0,0,0,0 };                     // Counter since pad started to play
unsigned long VelocityMax[7] = { 0,0,0,0,0,0,0 };                     // Counter since pad started to play

boolean dActivePad[7] = { 0,0,0,0,0,0,0 };                   // Array of flags of pad currently playing
unsigned long dPinPlayTime[7] = { 0,0,0,0,0,0,0 };                     // Counter since pad started to play
unsigned long dVelocityMax[7] = { 0,0,0,0,0,0,0 };                     // Counter since pad started to play
unsigned char dPadNote[7] = { 49,35,42,40,PEDALHIGHHAT,45 };         // MIDI notes from 0 to 127 (Mid C = 60)
bool dMidiOn[7] = { false,false,false,false,false,false,false };
unsigned long dMaxPlayTime[7] = { 1000,1000,1000,1000,100,1000,1000 };               // Cycles before a 2nd hit is allowed

unsigned char status;

int pin = 0;
int hitavg = 0;

//*******************************************************************************************************************
// Setup			
//*******************************************************************************************************************

void setup()
{
	Serial.begin(57600);                                  // connect to the serial port 115200
	pinMode(footSwitch, INPUT);
}

//*******************************************************************************************************************
// Main Program			
//*******************************************************************************************************************

void analogues()
{
	for (int pin = 0; pin < 6; pin++)
	{
		unsigned long t = micros();
		hitavg = min(analogRead(pin), 127);                              // read the input pin

		if ((activePad[pin] == true))
		{
			//          Serial.println((t - PinPlayTime[pin]),DEC);
			  //        Serial.println((1000*MaxPlayTime[pin]),DEC);

			if (VelocityMax[pin] < hitavg)
				VelocityMax[pin] = hitavg;

			if ((t - PinPlayTime[pin] > 1000 * SampleTime[pin]) && !MidiOn[pin])
			{
				MIDI_TX(0x90, Switches[pin] ? PadNoteSwitched[pin] : PadNote[pin], min((VelocityMax[pin] * Amplifier[pin] / 100), 127));
				MidiOn[pin] = true;
			}
			if (t - PinPlayTime[pin] > 1000 * MaxPlayTime[pin])
			{
				activePad[pin] = false;
				VelocityMax[pin] = 0;
				MIDI_TX(0x80, Switches[pin] ? PadNoteSwitched[pin] : PadNote[pin], 0);
				MidiOn[pin] = false;

			}
		}
		else if ((hitavg > PadCutOff[pin]))
		{
			if (DODEBUG)
			{
				Serial.println("");
				Serial.print("pin hit:");
				Serial.println(pin, DEC);
			}

			if ((activePad[pin] == false))
			{
				if (VelocityFlag == true)
				{
					//          hitavg = 127 / ((1023 - PadCutOff[pin]) / (hitavg - PadCutOff[pin]));    // With full range (Too sensitive ?)
					//          hitavg = (hitavg / 8) -1 ;                                                 // Upper range
				}
				else
				{
					hitavg = 127;
				}

				if (VelocityMax[pin] < hitavg)
					VelocityMax[pin] = hitavg;
				PinPlayTime[pin] = t;
				activePad[pin] = true;
			}
		}
	}
}

void switches()
{
	unsigned long t = micros();
	int switchValue = digitalRead(footSwitch);

	Switches[footSwitchNote] = switchValue;

	//    if ( Switches[footSwitchNote] == 1)
	//      Serial.println("jkhlkhlklk");
}

void digitals()
{
	//    for(int pin=0; pin < 6; pin++)
	{
		pin = footSwitch;
		unsigned long t = micros();
		int switchValue = digitalRead(pin);                              // read the input pin


		if (dActivePad[pin])
		{
			if (!dMidiOn[pin])
			{
				MIDI_TX(0x90, dPadNote[pin], min((127 * dAmplifier[pin] / 100), 127));
				dMidiOn[pin] = true;
			}
			else if (t - dPinPlayTime[pin] > 1000 * dMaxPlayTime[pin] && Switches[footSwitchNote] == 0)
			{
				dActivePad[pin] = false;
				MIDI_TX(0x80, dPadNote[pin], 0);
				dMidiOn[pin] = false;

			}
		}
		else if (switchValue == 1)
		{
			if (!dActivePad[pin])
			{
				dPinPlayTime[pin] = t;
				dActivePad[pin] = true;
			}
		}
	}

}


//*******************************************************************************************************************
// Transmit MIDI Message			
//*******************************************************************************************************************
void MIDI_TX(unsigned char MESSAGE, unsigned char PITCH, unsigned char VELOCITY)
{
	if (DODEBUG)
	{
		unsigned long t = micros();
		// print out the value you read:
		Serial.print("t: ");
		Serial.print(t, DEC);
		Serial.print("m: ");
		Serial.print(MESSAGE, DEC);
		Serial.print("p: ");
		Serial.print(PITCH, DEC);
		Serial.print(" v: ");
		Serial.println(VELOCITY, DEC);
	}
	else
	{

		status = MESSAGE + midichannel;
		Serial.write(status);
		Serial.write(PITCH);
		Serial.write(VELOCITY);
	}
}

void loop()
{
	switches();
	analogues();
	digitals();
}

