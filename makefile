CFLAGS = -Wall -ansi -pedantic
CC = gcc
GLOBAL_DEPS = global.h
EXE_DEPS = main.o pre_processor.o utils.o text_engine.o global.o stage_1.o stage_2.o labels_linked_list.o external_linked_list.o

#Runable
assembler: $(EXE_DEPS) $(GLOBAL_DEPS)
	$(CC) -g $(CFLAGS) $(EXE_DEPS) -lm -o assembler


#Main
main.o: main.c $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) main.c

global.o: global.c $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) global.c

pre_processor.o: pre_processor.c pre_processor.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) pre_processor.c

stage_1.o: stage_1.c stage_1.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) stage_1.c

stage_2.o: stage_2.c stage_2.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) stage_2.c

text_engine.o: text_engine.c text_engine.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) text_engine.c

utils.o: utils.c utils.h $(GLOBAL_DEPS)
	$(CC) -c $(CFLAGS) utils.c

labels_linked_list.o: labels_linked_list.c labels_linked_list.h
	$(CC) -c $(CFLAGS) labels_linked_list.c

external_linked_list.o: external_linked_list.c external_linked_list.h
	$(CC) -c $(CFLAGS) external_linked_list.c


#Clean
clean:
	rm -rf *.o assembler

cleanall:
	rm -rf *.o *.am *.ob *.ext *.ent assembler