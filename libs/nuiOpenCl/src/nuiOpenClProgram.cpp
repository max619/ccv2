#include "..\include\nuiOpenClProgram.h"

nuiOpenClProgram::nuiOpenClProgram(int functions)
{
	ocl = new ocl_container*[functions];

	for (int i = 0; i < functions; i++)
		ocl[i] = new ocl_container();

}

nuiOpenClProgram::~nuiOpenClProgram()
{
	int funcs = getFunctionsCount();

	for (int i = 0; i < funcs; i++)
	{
		delete ocl[i];
		delete functionsNames[i];
	}
	delete ocl;
	delete functionsNames;
}

char * nuiOpenClProgram::getSourceCode(int id)
{
	return NULL;
}

int nuiOpenClProgram::getFunctionsCount()
{
	return 0;
}

const char * nuiOpenClProgram::getFunctionName(int id)
{
	return functionsNames[id];
}

ocl_container * nuiOpenClProgram::getOclFunctionPtr(int id)
{
	return ocl[id];
}
