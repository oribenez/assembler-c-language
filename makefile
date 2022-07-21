CFLAGS = -Wall -ansi -pedantic
CC = gcc
GLOBAL_DEPS = global.h
EXE_DEPS = main.o pre_processor.o utils.o text_engine.o global.o stage_1.o labelsLinkedList.o

#Runable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(CFLAGS) $(EXE_DEPS) -o assembler


#Main
main.o: main.c $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) main.c

global.o: global.c $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) global.c

pre_processor.o: pre_processor.c pre_processor.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) pre_processor.c

stage_1.o: stage_1.c stage_1.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) stage_1.c

text_engine.o: text_engine.c text_engine.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) text_engine.c

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) utils.c

labelsLinkedList.o: labelsLinkedList.c labelsLinkedList.h
	$(CC) -c $(CFLAGS) labelsLinkedList.c


#Clean
clean:
	rm -rf *.o
