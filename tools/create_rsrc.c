#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "zlib.h"
//#include <windows.h>
//#include <ddk/Ntifs.h>

typedef unsigned long ULONG;

typedef struct	origpe
{
	unsigned char	*buffer;
	unsigned long	size;
}		t_origpe;

typedef struct	comppe
{
	unsigned long	size;
	unsigned char	*compbuffer;
}		t_comppe;

typedef struct	rsrchdr
{
	unsigned long	origsize;
	unsigned long	compsize;
}		t_rsrchdr;

t_origpe	*read_binary_intobuffer(char *path)
{
	int		fd;
	struct	stat	st;
	t_origpe	*pe;

	if (!(pe = (t_origpe*)malloc(sizeof(pe))))
		return (NULL);
	if (stat(path, &st))
		return (NULL);
	pe->size = st.st_size;
	if ((fd = open(path, O_RDONLY /*| O_BINARY*/)) <= 0)
		return (NULL);
	if (!(pe->buffer = (unsigned char *)malloc(sizeof(unsigned char) * pe->size)))
		return (NULL);
	if (read(fd, pe->buffer, st.st_size) < 0)
		return (NULL);
	close(fd);
	return (pe);
}

t_comppe	*compress_bin(t_origpe *pe)
{
	t_comppe	*cpe;
	void		*workspace;
	ULONG		cbs;
	ULONG		cfs;

	/*RtlGetCompressionWorkSpaceSize( COMPRESSION_FORMAT_LZNT1, &cbs, &cfs);
	if (!(workspace = malloc(cbs)))
		return (NULL);*/
	if (!(cpe = (t_comppe*)malloc(sizeof(t_comppe))))
		return (NULL);
	if (!(cpe->compbuffer = (unsigned char *)malloc(sizeof(unsigned char) * pe->size)))
		return (NULL);
	cpe->size = pe->size;
	if (compress(cpe->compbuffer, &cpe->size, pe->buffer, pe->size) != Z_OK)
		return (NULL);
	/*if (RtlCompressBuffer( COMPRESSION_FORMAT_LZNT1 | COMPRESSION_ENGINE_MAXIMUM,
		pe->buffer, pe->size, cpe->compbuffer, pe->size, 4096, &cpe->size,workspace))
			return (NULL);//will leak*/
	printf("Original size == %ld Compressed size == %ld\n", pe->size, cpe->size);
	return (cpe);
}

int	write_rsrc(t_origpe *pe, t_comppe *cpe, char *out)
{
	int fd = 0;
	t_rsrchdr hdr;

	hdr.origsize = pe->size;
	hdr.compsize = cpe->size;
	if ((fd = open(out, O_RDWR /*| O_BINARY*/ | O_CREAT | O_TRUNC)) < 0)
	{
		printf("[!] Can't open %s for writing!\n", out);
		return (0);
	}
	write(fd, &hdr, sizeof(t_rsrchdr));
	write(fd, cpe->compbuffer, cpe->size);
	close(fd);
	return (1);
}

int	main(int argc, char **argv)
{
	t_origpe	*pe;
	t_comppe	*cpe;

	if (argc != 3)
	{
		printf("Usage: %s inputfile outputfile\n", argv[0]);
		return (0);
	}
	if (!(pe = read_binary_intobuffer(argv[1])))
		printf("[!] Cannot read bin!\n");
	if (!(cpe = compress_bin(pe)))
		printf("[!] Cannot compress bin!\n");
	if (!(write_rsrc(pe, cpe, argv[2])))
		printf("[!] Cannot write resource file\n");
	free(pe->buffer);
	free(cpe->compbuffer);
	return (0);
}
