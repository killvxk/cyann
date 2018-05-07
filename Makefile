CC=$(PREFIXCC)-w64-mingw32-gcc
ifeq ($(PREFIXCC),x86_64)
	LIBZ=-lz64
else
	LIBZ=-lz
endif
STRIP=$(PREFIXCC)-w64-mingw32-strip
WINDRES=$(PREFIXCC)-w64-mingw32-windres
CFLAGS=-Wall -Wextra -Werror -DCYANNDBG
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
	nm32.rc\
	ico.rc
NAME=cyann.exe

SRC=$(addprefix $(SRCD)/, $(SRCS))
OBJ=$(patsubst %.c, %.o, $(SRC))
TRS=$(addprefix $(RSRCD)/, $(RSRCS))
RSRC=$(patsubst %.rc, %.o, $(TRS))

all: $(NAME)

$(RSRC): %.o : %.rc
	@printf "[+] Making rsrc $<                                                         \r"
	@$(WINDRES) -J rc -O coff -I$(INC) -i $< -o $@

$(OBJ) : %.o: %.c
	@printf "[+] Current $<                                              \r"
	@$(CC) $(CFLAGS) -I$(INC) -c $< -o $@

$(NAME): $(OBJ) $(RSRC)
	@printf "[+] Linking all together...                        		\n"
	@$(CC) -Ltools $(CFLAGS) -o $(NAME) $(OBJ) $(RSRC) $(LIBZ)
	@printf "[+] Striping binary \n"
	@$(STRIP) -s $(NAME)
	@printf "[+] Done! \n"

clean:
	@rm src/*.o
	@rm res/*.o
	@rm res/*.res

fclean: clean
	@rm $(NAME)

re: clean all
