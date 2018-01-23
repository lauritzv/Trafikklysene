#pragma once
class Car
{
private:
	int pos;
	bool through;

public:
	Car(int initialpos);
	~Car();
	int getPos();
	void setPos(int newpos);
	void setThrough();
	bool getThrough();
	void drive(int distance);

};

