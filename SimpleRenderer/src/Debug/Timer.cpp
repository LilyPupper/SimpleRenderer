#include "Debug/Timer.h"

#include <fstream>
#include <iostream>

ScopeTimer::ScopeTimer(const char* instanceName)
	: InstanceName(instanceName)
{
	Start = Time::now();
}

ScopeTimer::~ScopeTimer()
{
	End = Time::now();
	Duration difference = End - Start;
	//Millisecond ms = std::chrono::duration_cast<Millisecond>(difference);
	Microsecond us = std::chrono::duration_cast<Microsecond>(difference);

	std::string message(InstanceName + " took " + std::to_string(us.count()) + "us.");

	std::ofstream out("out.txt");
	out << message;
	out.close();
}