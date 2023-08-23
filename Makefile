_neon-s:
<<<<<<< HEAD
	gcc -mfloat-abi=softfp -mfpu=neon -static -O3 -S ./main.c
=======
	gcc -mfloat-abi=softfp -mfpu=neon -static -O3 -S /tmp/main.c
>>>>>>> e419849 (Co-authored-by: Matthew Van Brummelen <matthewvb77@users.noreply.github.com>)

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
