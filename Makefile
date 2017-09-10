CC=i686-w64-mingw32-gcc
WINDRES=i686-w64-mingw32-windres
CFLAGS=-Wall -Wextra -Werror
SRCD=src
OBJD=obj
RSRCD=res
INC=include
SRCS=	main.c\
	hollow.c\
	globale.c\
	resolve.c\
	parse.c\
	nanomite.c
RSRCS=	x32.rc\
	nm32.rc
NAME=cyann.exe

SRC=$(addprefix $(SRCD)/, $(SRCS))
OBJ=$(patsubst %.c, %.o, $(SRC))
TRS=$(addprefix $(RSRCD)/, $(RSRCS))
RSRC=$(patsubst %.rc, %.o, $(TRS))

all: $(NAME)

$(RSRC): %.o : %.rc
	@printf "[+] Making rsrc $<                                                         \r"
	@$(WINDRES) $< $@

$(OBJ) : %.o: %.c
	@printf "[+] Current $<                                              \r"
	@$(CC) -I$(CFLAGS) -I$(INC) -c $< -o $@

$(NAME): $(OBJ) $(RSRC)
	@printf "[+] Linking all together...                        		\n"
	@$(CC) -Ltools $(CFLAGS) -o $(NAME) $(OBJ) $(RSRC) -lz
	@printf "[+] Done! \n"

clean:
	@rm $(NAME)
	@rm src/*.o
	@rm res/*.o
	@rm res/*.res

re: clean all
