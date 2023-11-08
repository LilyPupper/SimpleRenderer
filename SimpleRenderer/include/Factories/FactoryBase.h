#pragma once

class FactoryBase
{
protected:
    virtual ~FactoryBase() = default;

public:
    // Source: https://stackoverflow.com/questions/18847739/how-does-delete-on-destructor-prevent-stack-allocation
	template<class T, class B> struct IsTypeDerivedFrom
    {
        static void DownCast(T* p)
        {
    	    B* pb = p;
        }
        IsTypeDerivedFrom()
        {
    	    void(*p)(T*) = DownCast;
        }
    };
};