#include <stdio.h>
#include <stdlib.h>
#include "mbr.h"
#include "drive.h"
#include "super.h"

int main(void)
{
  init_master();
  check_disk();
  load_mbr();
  display_filesystem();
  return 0;

}
