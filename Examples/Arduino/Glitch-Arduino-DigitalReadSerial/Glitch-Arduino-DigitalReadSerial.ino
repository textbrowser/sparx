// Generated by Glitch!
// Sun Jul 3 22:06:33 2022

int pushButton = 2;

void loop(void)
{
	int buttonState = digitalRead(pushButton);
	Serial.println(buttonState);
	delay(1);
}

void setup(void)
{
	Serial.begin(9600);
	pinMode(pushButton, INPUT);
}
