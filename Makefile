out:
	g++ -c src/main.cpp            -o src/main.o
	g++ -c src/FileOperations.cpp  -o src/FileOperations.o
	g++ -c src/Tree.cpp            -o src/Tree.o
	g++ -c src/Tokenizer.cpp       -o src/Tokenizer.o
	g++ -c src/Parser.cpp          -o src/Parser.o
	g++ -c src/GenerateAsmCode.cpp -o src/GenerateAsmCode.o
	g++ src/main.o src/FileOperations.o src/Tree.o src/Tokenizer.o src/Parser.o src/GenerateAsmCode.o -o src/generateAsmCode

	g++ -c CPU/Assembler/main.cpp           -o CPU/Assembler/main.o
	g++ -c CPU/Assembler/FileOperations.cpp -o CPU/Assembler/FileOperations.o
	g++ CPU/Assembler/main.o CPU/Assembler/FileOperations.o -o CPU/Assembler/asm
	
	g++ -O3 -c CPU/CPU/main.cpp           -o CPU/CPU/main.o
	g++ -O3 -c CPU/CPU/FileOperations.cpp -o CPU/CPU/FileOperations.o
	g++ -O3 -c CPU/CPU/Stack.cpp          -o CPU/CPU/Stack.cpp.o
	g++ CPU/CPU/main.o CPU/CPU/FileOperations.o CPU/CPU/Stack.cpp.o -o CPU/CPU/cpu
	
	src/generateAsmCode
	CPU/Assembler/asm
	time -p CPU/CPU/cpu
	
