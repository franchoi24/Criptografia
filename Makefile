executable = ss
includeFolder = include

all:
	gcc src/*.c -I $(includeFolder) -o $(executable)

clean:
	rm -rf ./$(executable)