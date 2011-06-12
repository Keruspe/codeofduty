all: codeofduty

codeofduty: codeofduty.c
	@echo "Compiling codeofduty"
	@gcc -Wall -Wextra -Werror -std=gnu99 -pedantic -O3 -march=native -g -ggdb3 codeofduty.c -o codeofduty -Wl,--as-needed -Wl,-O3
clean:
	@echo "Cleaning"
	@rm -f codeofduty
