int decimal_to_binary (int decimal)
{


	for(;;)



	//if last decimal is odd the binary number is 1
	if (decimal%2==1)
	{
		displayCenteredBigTextLine(1,"1");

	}
	else
	{
		displayCenteredBigTextLine(1,"0");
	}

	if ((decimal>>1)%2==1)
	{
		displayCenteredBigTextLine(3,"1");

	}
	else
	{
		displayCenteredBigTextLine(3,"0");
	}
	return 0;
}
task main()
{
	decimal_to_binary(10);
	sleep(10000000);
	/*	resetMotorEncoder(motorA);

	setMotorSpeed(motorA,100);
	setMotorSpeed(motorB,100);

	while (getMotorEncoder(motorA)<=360)
	{
	sleep(10);
	}

	setMotorSpeed(motorA,100);
	setMotorSpeed(motorB,0);

	sleep(1000);

	resetMotorEncoder(motorA);

	setMotorSpeed(motorA,100);
	setMotorSpeed(motorB,100);
	while (getMotorEncoder(motorA)<=360)
	{
	sleep(10);
	}
	setMotorSpeed(motorA,0);
	setMotorSpeed(motorB,0);
	*/
}
