#include "stdafx.h"
#include "Car.h"


Car::Car(double initialpos)
{
	through = false;
	pos = initialpos;

}


Car::~Car()
{
}

double Car::getPos()
{
	return pos;
}

void Car::setPos(double newpos)
{
	pos = newpos;
}

void Car::setThrough()
{
	through = true;
}

bool Car::getThrough()
{
	return through;
}

void Car::drive(double distance)
{
	pos += distance;
}

