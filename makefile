all: spectacular.c shipping.c
	gcc -pthread spectacular.c -o spectacular
	gcc -pthread shipping.c -o shipping

clean:
	rm -f spectacular shipping

spectacular: spectacular.c
	gcc -pthread spectacular.c -o spectacular

shipping: shipping.c
	gcc -pthread shipping.c -o shipping