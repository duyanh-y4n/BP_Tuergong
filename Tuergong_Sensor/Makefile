# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	 platformio -f -c vim run && platformio -f -c vim run --target compiledb

upload:
	platformio -f -c vim run --target upload && platformio -f -c vim run --target compiledb

clean:
	 platformio -f -c vim run --target clean

program:
	platformio -f -c vim run --target program && platformio -f -c vim run --target compiledb

uploadfs:
	platformio -f -c vim run --target uploadfs && platformio -f -c vim run --target compiledb

update:
	platformio -f -c vim update
