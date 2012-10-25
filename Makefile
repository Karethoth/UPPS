all:
	g++ src/*.cc -o upps -levent -g

clean:
	rm -f upps upps.exe

