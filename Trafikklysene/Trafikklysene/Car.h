#pragma once
class Car
{
private:
	double pos;
	bool through;

public:
	Car(double initialpos);
	~Car();
	double getPos();
	void setPos(double newpos);
	void setThrough();
	bool getThrough();
	void drive(double distance);

};

