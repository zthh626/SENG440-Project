_neon-s:
	gcc -mfloat-abi=softfp -mfpu=neon -static -O3 -S ./main.c

_neon-exe:
	gcc -mfloat-abi=softfp -mfpu=neon main.s -o main.exe

_neon:
	make _neon-s && make _neon-exe

_exe:
	arm-linux-gcc -static -o main.exe ./main.c

neon:
	make _neon && chmod +x main.exe

exe:
	make _exe && chmod +x main.exe
