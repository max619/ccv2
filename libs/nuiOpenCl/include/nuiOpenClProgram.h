#pragma once
#include "nuiOpenClStructures.h"

#define DEFAULT_OPENCL_PROGRAM_INTERFACE_DECLARATION()\
public:\
	char * getSourceCode(int id);\
	int getFunctionsCount();\



class nuiOpenClProgram
{
public:
	nuiOpenClProgram(int functions);
	~nuiOpenClProgram();

	virtual char* getSourceCode(int id);
	virtual int getFunctionsCount();
	const char* getFunctionName(int id);
	ocl_container* getOclFunctionPtr(int id);

protected:
	ocl_container** ocl;
	const char** functionsNames;
};