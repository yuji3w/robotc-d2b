/*
Allows the robot to accurately rotate a set number of degrees or move a set amount of distance at a set speed.
The parameters in the macros need to be changed for different use cases.

-Yujie
*/

#define TABLEWIDTH 75.0
#define TABLEHEIGHT 182.0

#define WHEEL 18.0
#define AXLE (12.0+.5-.25)
//(12+.5+.25-.125)
#define MINDETECTIONS1 20
#define MINDETECTIONS2 22

#define ON_TABLE 50.0
#define AVG_TABLE 20.0
#define OFF_TABLE 0.0

#define WHITE 10
#define BLACK 0
#define CENTER 5

#define OUT false
#define IN true

#define RMOTOR motorA
#define LMOTOR motorD

float tableReflectS1 = 40;
float tableReflectS4 = 40;

//ALTS(sensor) will return motorD if motorA is passed and vice versa.
#define ALTS(A) ((A)==0 ? 3 : 0)

void moveRob(float distance, int speed);
void rotateRob(float angle, int speed);
bool onTable(int sensor);
void alignFront(int time);
void followEdge(float distance, int sensor, int speed);
void rotateLeft(float radius, float angle, float speed);
void rotateRight(float radius, float angle, float speed);

//The two functions below are simple variations of followEdge without conditionals.
void lineFollowS1(float speed, float length, float sensitivity);
void lineFollowS4(float speed, float length, float sensitivity);
void altLineFollow(int speed);
void moveEndFromLeft(int zone);
void moveEndFromRight(int zone);

void escalatorUp();
void escalatorDown();
void clampDown();
void clampOut();
void selectPath();
void stopDrive();
void moveUntilBlack(int speed);
void clampOutStart();
void leftPath();
void rightPath();
void selectClamp();

int zoneNo = 0;
int sweepNo = 0;

int terribleOffset = 0;

task main()
{
	sleep(500);
	tableReflectS1 = getColorReflected(S1)/2;
	tableReflectS4 = getColorReflected(S4)/2;
	selectPath();
	selectClamp();

	if(sweepNo==0)
	{
		leftPath();
	}else
	{
		rightPath();
	}
}

void moveEndFromRight(int zone)
{
	if(zone==0)
	{
		moveUntilBlack(30);
		moveRob(2,20);
		clampOut();
	}else
	{
		float distance = zone*TABLEWIDTH/4*1.1-AXLE-AXLE/2+7;
		rotateLeft(AXLE/2,90,40);
		moveRob(distance,50);
		rotateRight(AXLE/2,90,40);
		moveUntilBlack(30);
		moveRob(2,20);
		clampOut();
	}
}


void moveEndFromLeft(int zone)
{
	if(zone==3)
	{
		moveUntilBlack(100);
		moveRob(2,20);
		clampOut();
	}else
	{
		float distance = (3-zone)*TABLEWIDTH/4*1.1-AXLE-AXLE/2+7;
		rotateRight(AXLE/2,90,30);
		moveRob(distance,100);
		rotateLeft(AXLE/2,90,30);
		moveUntilBlack(20);
		moveRob(2,20);
		clampOut();
	}
}

void stopDrive()
{
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorD,0);
}

void selectPath()
{
	while(getButtonPress(buttonEnter)==false)
	{
		displayCenteredBigTextLine(0, "0 is left: %d", sweepNo);
		displayCenteredBigTextLine(2, "1,2,4,8: %d", pow(2,zoneNo));
		if(getButtonPress(buttonUp)==true)
		{
			zoneNo++;
			zoneNo%=4;
		}else if(getButtonPress(buttonDown)==true)
		{
			zoneNo--;
			if(zoneNo==-1)
			{
				zoneNo = 3;
			}
		}else if(getButtonPress(buttonRight)==true)
		{
			sweepNo++;
			sweepNo%=2;
		}else if(getButtonPress(buttonLeft)==true)
		{
			sweepNo--;
			if(sweepNo==-1)
			{
				sweepNo=1;
			}
		}
		sleep(100.);
	}
	sleep(500);

}

void escalatorUp()
{
	setMotorSpeed(motorB,-35);
	while(getMotorEncoder(motorB)>-900)
	{
	}
	setMotorSpeed(motorB,0);
}

void escalatorDown()
{
	setMotorSpeed(motorB,35);
	while(getMotorEncoder(motorB)<-110)
	{
	}
	setMotorSpeed(motorB,0);
}

void clampDown()
{
	setMotorSpeed(motorC,-100);
	sleep(500);
}

void clampOut()
{
	setMotorSpeed(motorC,10);
	sleep(350);
	setMotorSpeed(motorC,0);
}

/*
moveRob takes distance in cm. Speed is from -100 to 100. Negative for either value is backwards.
*/
void moveRob(float distance, int speed)
{
	//Our robot has backwards motors
	if(speed < 0)
	{
		speed *= -1;
		distance *= -1;
	}
	int encoder[2];
	encoder[0] = getMotorEncoder(motorA);
	encoder[1] = getMotorEncoder(motorD);
	setMotorSpeed(motorA,speed*sgn(distance));
	setMotorSpeed(motorD,speed*sgn(distance));
	distance = distance * (360 / WHEEL);
	encoder[0] += distance;
	encoder[1] += distance;
	int flag = sgn(encoder[0]-getMotorEncoder(motorA));
	while(flag+sgn(encoder[0]-getMotorEncoder(motorA)) && flag+sgn(encoder[1]-getMotorEncoder(motorD))){}
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorD,0);
}

void rotateLeft(float radius, float angle, float speed)
{
	int rightEncoderStart = getMotorEncoder(RMOTOR);
	int encoderDist = 100;
	float rightRadius = radius + AXLE/2;
	float leftRadius = radius - AXLE/2;
	float rightDist = angle * PI / 180.0 * rightRadius;
	rightDist = rightDist * (360 / WHEEL);
	float rightSpeed = speed;
	float leftSpeed = speed * leftRadius / rightRadius;
	setMotorSpeed(RMOTOR, rightSpeed);
	setMotorSpeed(LMOTOR, leftSpeed);
	while(encoderDist < rightDist)
	{
		encoderDist = getMotorEncoder(RMOTOR) - rightEncoderStart;
	}
}

void rotateRight(float radius, float angle, float speed)
{
	int leftEncoderStart = getMotorEncoder(LMOTOR);
	int encoderDist = 100;
	float leftRadius = radius + AXLE/2;
	float rightRadius = radius - AXLE/2;
	float leftDist = angle * PI / 180.0 * leftRadius;
	leftDist = leftDist * (360 / WHEEL);
	float leftSpeed = speed;
	float rightSpeed = speed * rightRadius / leftRadius;
	setMotorSpeed(RMOTOR, rightSpeed);
	setMotorSpeed(LMOTOR, leftSpeed);
	while(encoderDist < leftDist)
	{
		encoderDist = getMotorEncoder(LMOTOR) - leftEncoderStart;
	}
}


/*
rotateRob takes angle out of 360 with counterclockwise being positive and clockwise being negative. Speed is -100 to 100.
*/
void rotateRob(float angle, int speed)
{
	int encoder[2];
	encoder[0] = getMotorEncoder(motorA);
	encoder[1] = getMotorEncoder(motorD);
	setMotorSpeed(motorA,speed*sgn(angle));
	setMotorSpeed(motorD,speed*-sgn(angle));
	float distance = angle * (AXLE * PI * (360.0 / WHEEL)) / 360.0;
	encoder[0] += distance;
	encoder[1] -= distance;
	int flag[2];
	flag[0] = sgn(encoder[0]-getMotorEncoder(motorA));
	flag[1] = sgn(encoder[1]-getMotorEncoder(motorA));
	while(flag[0]+sgn(encoder[0]-getMotorEncoder(motorA)) && flag[1]+sgn(encoder[1]-getMotorEncoder(motorB))){}
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorD,0);
}

/*
Modify this for your use case by changing the while(1) to while(desired conditional) TODO: make function generic
*/
void lineFollowS1(float speed, float length, float sensitivity)
{
	resetMotorEncoder(motorD);
	length = length * (360.0 / WHEEL);
	while(getMotorEncoder(motorD) < length)
	{
		float offset =  getColorReflected(S1)/tableReflectS1;
		offset-=1.0;
		offset*=sensitivity;
		//TODO:
		setMotorSpeed(motorA, speed + offset);
		setMotorSpeed(motorD, speed);
	}
}

void lineFollowS4(float speed, float length, float sensitivity)
{
	resetMotorEncoder(motorA);
	length = length * (360.0 / WHEEL);
	while(/*getColorReflected(S1)>tableReflectS1/2 && */getMotorEncoder(motorA) < length)
	{
		float offset =  getColorReflected(S4)/tableReflectS4;
		offset-=1.0;
		offset*=sensitivity;
		//TODO:
		setMotorSpeed(motorA, speed);
		setMotorSpeed(motorD, speed + offset);
	}
}

/*
Modify this for your use case by changing the while(1) to while(desired conditional) TODO: make function generic
*/
void altLineFollow(int speed)
{
	while(1)
	{
		int error = getColorReflected(S1)-CENTER;
		setMotorSpeed (motorA,speed + error);
		setMotorSpeed (motorB,speed - error);
	}
}

/*
Returns whether the sensor is on or off the table.
*/
bool onTable(int sensor)
{
	if(SensorValue[sensor]<AVG_TABLE)
	{
		return OUT;
	}else
	{
		return IN;
	}
}

/*
Aligns the robot's front with the edge of the table if both sensors are correctly positioned. Time is in milliseconds.
TODO: change time to actual time.
*/
void alignFront(int time)
{
	int i = 0;
	while(1){
		//The /3 is slightly arbitrary. TODO: make passable?
		setMotorSpeed(motorD,(-tableReflectS1+getColorReflected(S1))/3.0);
		setMotorSpeed(motorA,(-tableReflectS4+getColorReflected(S4))/3.0);
		if(++i>=time){
			break;
		}
	}
}

/*
followEdge takes distance in cm; sensor as 0, 1, 2, or S1, S2, S3; speed from -100 to 100.
This is designed for a robot with two sensors in line, so it's not ideal.
*/
void followEdge(float distance, int sensor, int speed)
{
	float coeff = .25;
	//The severity at which the robot follows the edge. Its aggressiveness.
	distance -= 3;
	//Cuts robot's linefollow significantly as it nears the edge due to rounded corners on table.
	distance = distance * (360 / WHEEL);
	int initMotorEncoder = abs(getMotorEncoder(motorA));
	int deltaEncoder = 0;
	while(onTable(ALTS(sensor))){
		while(onTable(ALTS(sensor))){
			deltaEncoder = abs(getMotorEncoder(motorA))-initMotorEncoder;
			deltaEncoder = abs(deltaEncoder);
			if(deltaEncoder >= distance)
			{
				coeff = 0.0625;
				//TODO: Make this argument or make generic.
			}
			if(sensor){
				setMotorSpeed(motorA,-1*(speed-coeff*(AVG_TABLE-SensorValue(sensor))));
				setMotorSpeed(motorD,-1*(speed+coeff*(AVG_TABLE-SensorValue(sensor))));
				}else{
				setMotorSpeed(motorA,-1*(speed+coeff*(AVG_TABLE-SensorValue(sensor))));
				setMotorSpeed(motorD,-1*(speed-coeff*(AVG_TABLE-SensorValue(sensor))));
			}
			//sleep(30);
		}
		//sleep(30);
	}
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorD,0);
}

void moveUntilBlack(int speed)
{
	playSound(soundBeepBeep);
	setMotorSpeed(motorA,speed);
	setMotorSpeed(motorD,speed);
	while(getColorReflected(S1)>tableReflectS1 || getColorReflected(S4)>tableReflectS4){}
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorD,0);
}

void clampOutStart()
{
	setMotorSpeed(motorC,10);
	sleep(700);
	setMotorSpeed(motorC,0);
}

void leftPath()
{
	//Henceforth I shall go along the left
	rotateRight(AXLE/2+30-terribleOffset+2,90,60);
	lineFollowS1(80,80.0/2,30);
	lineFollowS1(50,80.0/2,30/3);
	clampDown();
	moveEndFromLeft(zoneNo);
	stopDrive();
	//Henceforth I stop going along left
}

void rightPath()
{
	rotateRight(30,180,50);
	moveRob(20,50);
	alignFront(10000);
	rotateLeft(0,90,20);
	lineFollowS4(40,45.0/2,20);
	lineFollowS4(20,45.0/2,20/4);
	clampDown();
	moveEndFromRight(zoneNo);
}

void selectClamp()
{
	displayCenteredBigTextLine(0,"Right for clamp");
	displayCenteredBigTextLine(0,"Left for going out\n and back in");
	while(getButtonPress(buttonEnter)==false)
	{
		if(getButtonPress(buttonRight)==true)
		{
			clampDown();
		}else if(getButtonPress(buttonLeft)==true)
		{
			moveRob(18,50);
			moveRob(-17,20);
			sleep(2000);
			clampDown();
			escalatorUp();
			moveRob(16,20);
			clampOutStart();
			moveRob(-8,50);
			clampDown();
			escalatorDown();
			clampOutStart();
			moveRob(9,50);
			clampDown();
			//remove after or else
			terribleOffset = 7;
			return;
		}else if(getButtonPress(buttonEnter)==true)
		{
			moveRob(10,50);
			alignFront(10000);
			return;
		}
	}
}
