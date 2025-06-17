#include <wiringPi.h>
#include <softTone.h>

#define SPKR	6
#define TOTAL 	32

int notes[] = {440};

int musicPlay(){
	int i;
	softToneCreate(SPKR);
	for(i=0; i<TOTAL; ++i){
		softToneWrite(SPKR, notes[i]);
		delay(280);}

	return 0;
}

int main()
{wiringPiSetup();
	musicPlay();
	return 0;}

