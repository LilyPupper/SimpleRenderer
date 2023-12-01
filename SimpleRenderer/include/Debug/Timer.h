#pragma once

#include<chrono>
#include <string>
#include <vector>

#define DEBUG_SCOPETIMER ScopeTimer dt(__FUNCTION__);

typedef std::pair<float, std::string> ScopeTimer_MessageType;

class ScopeTimer
{
public:
	ScopeTimer(const char* instanceName);
	~ScopeTimer();

private:
	using Millisecond = std::chrono::milliseconds;
	using Microsecond = std::chrono::microseconds;
	using Time = std::chrono::high_resolution_clock;
	using Timepoint = std::chrono::steady_clock::time_point;
	using Duration = std::chrono::duration<float>;

	std::string InstanceName;
	Timepoint Start;
	Timepoint End;
};