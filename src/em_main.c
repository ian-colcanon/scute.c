#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "chunk.h"
#include "debug.h"
#include "value.h"
#include "vm.h"
#include "svg.h"
#include "output.h"
#include "package.h"

void runCode(CompilePackage* code, int index){
	InterpretResult result = interpretCompiled(code, index);
}

CompilePackage* compileCode(const char* code){
	CompilePackage* package = initCompilationPackage();
	runCompiler(package, (char*) code);
	return package;
}
