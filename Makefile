all: clean initializer producer spy finisher

clean:
	rm -f Initializer/*.o Producer/*.o Spy/*.o Finisher/*.o Initializer/initializer Producer/producer Spy/spy Finisher/finisher

initializer: Initializer/initializer.c
	cd Initializer && gcc -o initializer initializer.c -lpthread -lm

producer: Producer/producer.c
	cd Producer && gcc -o producer producer.c -pthread -lm

spy: Spy/spy.c
	cd Spy && gcc -o spy spy.c -pthread -lm

finisher: Finisher/finisher.c
	cd Finisher && gcc -o finisher finisher.c -pthread -lm