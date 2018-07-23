#pragma once
#include "nuiOpenClStructures.h"


class nuiOpenClProgram
{
public:
	nuiOpenClProgram();
	~nuiOpenClProgram();

	virtual char* getSourceCode(int id);
	virtual int getFunctionsCount();
	const char* getFunctionName(int id);
	ocl_container* getOclFunctionPtr(int id);

private:
	ocl_container** ocl;
	const char** functionsNames;
};