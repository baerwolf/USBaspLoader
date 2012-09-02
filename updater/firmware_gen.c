
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


#define myfirmware_rawfilename	"./usbasploader.raw"
#define myout			stdout

int main(int argc, char** argv) {
  int fd;
  uint16_t b;
  uint64_t c;
  struct stat fwstat;

  fprintf(myout, "\n");
  fprintf(myout, "#ifndef FIRMWARE_H_5f27a7e9840141b1aa57eef07c1d939f\n");
  fprintf(myout, "#define FIRMWARE_H_5f27a7e9840141b1aa57eef07c1d939f 1\n");
  fprintf(myout, "\n");
  fprintf(myout, "#include <stdint.h>\n");
  fprintf(myout, "#include <avr/io.h>\n");
  fprintf(myout, "#include \"../firmware/spminterface.h\"\n");
  fprintf(myout, "\n");
  fprintf(myout, "//firmware generator generated\n");
  
  fd=open(myfirmware_rawfilename, O_RDONLY);
  if (fd > 2) {
    fstat(fd, &fwstat);
    fprintf(myout, "#define	SIZEOF_new_firmware						%llu\n",(long long unsigned int)fwstat.st_size);
    fprintf(myout, "const uint16_t firmware[SIZEOF_new_firmware>>1] PROGMEM	=	{");
    fprintf(myout, "\n");

    c=0;
    while (read(fd, &b, 2) == 2) {
      c+=2;
      fprintf(myout,"0x%04x, ", (unsigned int)b);
      if ((c % 20) == 0) fprintf(myout,"\n");
    }
    if ((c % 20) != 0) fprintf(myout,"\n");
    fprintf(myout, "};\n");
    fprintf(myout, "const uint8_t *new_firmware	=	(void*)&firmware;\n");
    fprintf(myout, "\n");

    close(fd);
  } else {
    fprintf(stderr, "error opening %s\n", myfirmware_rawfilename);
    
    fprintf(myout, "#define	SIZEOF_new_firmware						0\n");
    fprintf(myout, "const uint16_t firmware[SIZEOF_new_firmware>>1] PROGMEM	=	{};\n");
    fprintf(myout, "const uint8_t *new_firmware	=	(void*)&firmware;\n");
    fprintf(myout, "\n");
  }
  
  fprintf(myout, "#endif\n");
  fprintf(myout, "\n");
  return 0;
}
