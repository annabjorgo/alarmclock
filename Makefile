
CFILES:=$(wildcard *.c)

.PHONY: clean all

all: alarmclock

alarmclock: ${CFILES}
	cc $^ -o $@

clean:
	rm -f alarmclock