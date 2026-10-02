all: producer consumer

producer: producer.cpp
	g++ -o producer producer.cpp -pthread

consumer: consumer.cpp
	g++ -o consumer consumer.cpp -pthread

 clean:
	rm -f consumer producer

	# Remove shared memory
	ipcrm -M 0x1234

	# Remove semaphores
	ipcrm -S 0x2345
	ipcrm -S 0x3456
	ipcrm -S 0x4567
