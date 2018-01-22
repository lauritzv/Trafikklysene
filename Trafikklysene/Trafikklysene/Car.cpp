#include "stdafx.h"
#include "Car.h"


Car::Car(int initialpos, bool dx)
{
	through = false;
	pos = initialpos;
	dirx = dx;

}


Car::~Car()
{
}

bool Car::isDirx()
{
	return dirx;
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

void Car::drive(int distance)
{
	pos += distance;
}

