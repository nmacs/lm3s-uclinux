#include <stdio.h>

//#if 0
#define USE_USTL

#ifdef USE_USTL


#include <ustl.h>
#define std ustd

#else

#include <vector>
#include <string>
#include <iostream>

#endif
//#endif

int main()
{
	std::vector<std::string> v;
	v.push_back("test");

	for( int i = 0; i < 100; i++ )
		v.push_back("test3");

	std::string r;
	std::vector<std::string>::iterator iter;
	for( iter = v.begin(); iter != v.end(); ++iter )
		r += *iter;

	try
	{
		std::cout << "throw 1" << std::endl;
		//printf("throw 1\n");
		throw 1;
		//printf("bad\n");
		std::cout << "bad" << std::endl;
	}
	catch(int i)
	{
		//printf("catch int\n");
		std::cout << "catch int" << std::endl;
	}
	catch(...)
	{
		//printf("catch unknown\n");
		std::cout << "catch" << std::endl;
	}

	//printf("success\n");
	std::cout << "row length: " << r.size() << std::endl;
	std::cout << "success" << std::endl;

	return 0;
}