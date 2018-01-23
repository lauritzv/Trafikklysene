#include "stdafx.h"
#include "Car.h"


Car::Car(int initialpos)
{
	through = false;
	pos = initialpos;

}


Car::~Car()
{
}

int Car::getPos()
{
	return pos;
}

void Car::setPos(int newpos)
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

void Car::drive(int distance)
{
	pos += distance;
}

