_neon-s:
	/opt/arm/4.3.2/bin/arm-linux-gcc -mfloat-abi=softfp -mfpu=neon -static -O3 -S /tmp/main.c

_neon-exe:
	/opt/arm/4.3.2/bin/arm-linux-gcc -mfloat-abi=softfp -mfpu=neon main.s -o main.exe

_neon:
	make _neon-s && make _neon-exe

_exe:
	arm-linux-gcc -static -o main.exe /tmp/main.c

neon:
	cp ./main.c /tmp/main.c && make _neon && rm /tmp/main.c

exe:
	cp ./main.c /tmp/main.c && make _exe && chmod +x main.exe && rm /tmp/main.c
