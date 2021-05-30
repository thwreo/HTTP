build:
	gcc client.c requests.c helpers.c buffer.c parson.c -o client
run:
	./client
clean:
	rm client