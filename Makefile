all:
	g++ src/*.cc -o upps -levent

clean:
	rm -f upps upps.exe

