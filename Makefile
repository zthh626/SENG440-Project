compile:
	cp ./main.c /tmp/main.c && /opt/arm/4.3.2/bin/arm-linux-gcc -mfloat-abi=softfp -mfpu=neon -static -O3 -S /tmp/main.c && rm /tmp/main.c