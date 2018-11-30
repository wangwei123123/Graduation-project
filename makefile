cc = gcc
prom = ww_shell
deps = $(shell find ./ -name "*.h")
src = ww_shell.c prompt.c read_command.c parse.c exec.c builtin.c login.c
obj = $(src:%.c=%.o)

$(prom) : $(obj)
	$(cc) -o $(prom) $(obj) /usr/lib64/libreadline.so -I -lreadline -lncurses -g -pthread
	rm -rf $(obj)

%.o : %.c $(deps)
	$(cc) -c $< -o $@ 

.PHONY: clean
clean :
	rm -rf $(prom) test *.o

install :
#

test :
	$(cc) -o test test.c
