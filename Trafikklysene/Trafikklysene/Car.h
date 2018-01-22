#pragma once
class Car
{
private:
	int pos;
	bool through;
	bool dirx;

public:
	Car(int initialpos, bool dirx);
	~Car();
	bool isDirx();
	int getPos();
	void setPos(int newpos);
	void setThrough();
	void drive(int distance);

};

