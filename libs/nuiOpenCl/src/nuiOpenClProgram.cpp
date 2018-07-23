#include "..\include\nuiOpenClProgram.h"

nuiOpenClProgram::nuiOpenClProgram()
{
	int funcs = getFunctionsCount();

	ocl = new ocl_container*[funcs];

	for (int i = 0; i < funcs; i++)
		ocl[i] = new ocl_container();

}

nuiOpenClProgram::~nuiOpenClProgram()
{
	int funcs = getFunctionsCount();

	for (int i = 0; i < funcs; i++)
		delete ocl[i];

	delete ocl;
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
