/*
 * Set / inspect region settings on a DVD drive. This is most interesting
 * on a RPC Phase-2 drive, of course.
 *
 * Usage: dvd_region [ -d device ] [ [ -s ] [ -r region ] ]
 *
 * Based on code from Jens Axboe <axboe@suse.de>.
 *
 * FIXME:  This code does _not_ work yet.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>

#include <dvdcss/dvdcss.h>

#include "config.h"
#include "common.h"
#include "ioctl.h"

#define DEFAULT_DEVICE "/dev/dvd"

int set_region(int fd, int region)
{
  int ret, region_mask;

  if(region > 8 || region <= 0) {
    printf("Invalid region( %d)\n", region);
    return 1;
  }
  printf("Setting drive region can only be done a finite " \
	 "number of times, press CTRL-C now to cancel!\n");
  getchar();

  region_mask = 0xff & ~(1 << (region - 1));
  printf("Setting region to %d( %x)\n", region, region_mask);
  if( (ret = ioctl_SendRPC(fd, region_mask)) < 0) {
    perror("dvd_region");
    return ret;
  }

  return 0;
}

int print_region(int fd)
{
  int type, region_mask, rpc_scheme;
  int region = 1;
  int ret;
	
  printf("Drive region info:\n");

  if( (ret = ioctl_ReportRPC(fd, &type, &region_mask, &rpc_scheme)) < 0) {
    perror("dvd_region");
    return ret;
  }

  printf("Type: ");
  switch( type ) {
  case 0:
    printf("No drive region setting\n");
    break;
  case 1:
    printf("Drive region is set\n");
    break;
  case 2:
    printf("Drive region is set, with additional " \
	   "restrictions required to make a change\n");
    break;
  case 3:
    printf("Drive region has been set permanently, but " \
	   "may be reset by the vendor if necessary\n");
    break;
  default:
    printf("Invalid( %x)\n", type);
    break;
  }

  // printf("%d vendor resets available\n", ai->lrpcs.vra);
  // printf("%d user controlled changes available\n", ai->lrpcs.ucca);
  printf("Region: ");
  if( region_mask)
    while(region_mask) {
      if( !(region_mask & 1) )
	printf("%d playable\n", region);
      region++;
      region_mask >>= 1;
    }
  else
    printf("non-rpc( all)\n");

  printf("RPC Scheme: ");
  switch( rpc_scheme ) {
  case 0:
    printf("The Logical Unit does not enforce Region " \
	   "Playback Controls( RPC)\n");
    break;
  case 1:
    printf("The Logical Unit _shall_ adhere to the "
	   "specification and all requirements of the " \
	   "CSS license agreement concerning RPC\n");
    break;
  default:
    printf("Reserved( %x)\n", rpc_scheme);
  }

  return 0;
}

static void usage(void)
{
  fprintf( stderr, 
	   "Usage: dvd_region [ -d device ] [ [ -s ] [ -r region ] ]\n" );
}  

int main(int argc, char *argv[])
{
  char device_name[FILENAME_MAX], c, set, get, region = 0;
  int fd, ret;

  strcpy(device_name, DEFAULT_DEVICE);
  set = 0;
  get = 1;
  while( (c = getopt(argc, argv, "d:sr:h?")) != EOF ) {
    switch( c ) {
    case 'd':
      strncpy(device_name, optarg, FILENAME_MAX - 1);
      break;
    case 's':
      set = 1;
      break;
    case 'r':
      region = strtoul(optarg, NULL, 10);
      printf("region %d\n", region);
      break;
    case 'h':
    case '?':
    default:
      usage();
      return -1;
      break;
    }
  }

  if( optind != argc ) {
    fprintf(stderr, "Unknown argument\n");
    usage();
    return -1;
  }

  /* Print version number */
  printf( "found libdvdcss version %s\n", dvdcss_interface_2 );

  /* TODO: use dvdcss_open instead of open */

  if( (fd = open(device_name, O_RDONLY | O_NONBLOCK)) < 0 ) {
    perror("open");
    usage();
    return 1;
  }

  {
    int copyright;
    ret = ioctl_ReadCopyright( fd, 0, &copyright );
    printf( "ret %d, copyright %d\n", ret, copyright );
  }

  if( (ret = print_region(fd)) < 0 )
    return ret;

  if( set ) {
    if( !region ) {
      fprintf( stderr, "you must specify the region!\n" );
      exit(0);
    }
    
    if( (ret = set_region(fd, region)) < 0 )
      return ret;
  }

  exit( 0 );
}
