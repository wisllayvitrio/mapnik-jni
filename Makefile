all: mapnik-jni

mapnik-jni:
	ant

clean:
	ant clean

test:
	ant test

.PHONY: clean test
