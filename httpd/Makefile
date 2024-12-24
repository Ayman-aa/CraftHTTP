all:
	@echo "Usage:"
	@echo "  make serv    - Build the HTTP server"
	@echo "  make curl    - Build the curl testing tool"
	@echo "  make clean   - Remove built files"
	@echo "  make re      - Rebuild everything"

serv:
	cc -g3 httpd.c -o serv

curl:
	c++ curl.cpp -o curl

clean:
	rm -rf serv

re: clean all
