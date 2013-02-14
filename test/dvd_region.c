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

/*****************************************************************************
 * ioctl_SendRPC: set RPC status for the drive
 *****************************************************************************/
static int ioctl_SendRPC( int i_fd, int i_pdrc )
{
    int i_ret;

#if defined( HAVE_LINUX_DVD_STRUCT ) && defined( DVD_HOST_SEND_RPC_STATE )
    dvd_authinfo auth_info = { 0 };

    auth_info.type = DVD_HOST_SEND_RPC_STATE;
    auth_info.hrpcs.pdrc = i_pdrc;

    i_ret = ioctl( i_fd, DVD_AUTH, &auth_info );

#elif defined( HAVE_LINUX_DVD_STRUCT )
    /* FIXME: OpenBSD doesn't know this */
    i_ret = -1;

#elif defined( HAVE_BSD_DVD_STRUCT )
    struct dvd_authinfo auth_info = { 0 };

    auth_info.format = DVD_SEND_RPC;
    auth_info.region = i_pdrc;

    i_ret = ioctl( i_fd, DVDIOCSENDKEY, &auth_info );

#elif defined( __BEOS__ )
    INIT_RDC( GPCMD_SEND_KEY, 8 );

    rdc.command[ 10 ] = DVD_SEND_RPC;

    p_buffer[ 1 ] = 6;
    p_buffer[ 4 ] = i_pdrc;

    i_ret = ioctl( i_fd, B_RAW_DEVICE_COMMAND, &rdc, sizeof(rdc) );

#elif defined( HPUX_SCTL_IO )
    INIT_SCTL_IO( GPCMD_SEND_KEY, 8 );

    sctl_io.cdb[ 10 ] = DVD_SEND_RPC;

    p_buffer[ 1 ] = 6;
    p_buffer[ 4 ] = i_pdrc;

    i_ret = ioctl( i_fd, SIOC_IO, &sctl_io );

#elif defined( SOLARIS_USCSI )
    INIT_USCSI( GPCMD_SEND_KEY, 8 );

    rs_cdb.cdb_opaque[ 10 ] = DVD_SEND_RPC;

    p_buffer[ 1 ] = 6;
    p_buffer[ 4 ] = i_pdrc;

    i_ret = SolarisSendUSCSI( i_fd, &sc );

    if( i_ret < 0 || sc.uscsi_status )
    {
        i_ret = -1;
    }

#elif defined( DARWIN_DVD_IOCTL )
    INIT_DVDIOCTL( dk_dvd_send_key_t, DVDRegionPlaybackControlInfo,
                   kDVDKeyFormatSetRegion );

    dvd.keyClass = kDVDKeyClassCSS_CPPM_CPRM;
    dvdbs.driveRegion = i_pdrc;

    i_ret = ioctl( i_fd, DKIOCDVDSENDKEY, &dvd );

#elif defined( WIN32 )
    if( WIN2K ) /* NT/2k/XP */
    {
        INIT_SPTD( GPCMD_SEND_KEY, 8 );

        sptd.Cdb[ 10 ] = DVD_SEND_RPC;

        p_buffer[ 1 ] = 6;
        p_buffer[ 4 ] = i_pdrc;

        i_ret = SEND_SPTD( i_fd, &sptd, &tmp );
    }
    else
    {
        INIT_SSC( GPCMD_SEND_KEY, 8 );

        ssc.CDBByte[ 10 ] = DVD_SEND_RPC;

        p_buffer[ 1 ] = 6;
        p_buffer[ 4 ] = i_pdrc;

        i_ret = WinSendSSC( i_fd, &ssc );
    }

#elif defined( __QNXNTO__ )

    INIT_CPT( GPCMD_SEND_KEY, 8 );

    p_cpt->cam_cdb[ 10 ] = DVD_SEND_RPC;

    p_buffer[ 1 ] = 6;
    p_buffer[ 4 ] = i_pdrc;

    i_ret = devctl(i_fd, DCMD_CAM_PASS_THRU, p_cpt, structSize, NULL);

#elif defined( __OS2__ )
    INIT_SSC( GPCMD_SEND_KEY, 8 );

    sdc.command[ 10 ] = DVD_SEND_RPC;

    p_buffer[ 1 ] = 6;
    p_buffer[ 4 ] = i_pdrc;

    i_ret = DosDevIOCtl( i_fd, IOCTL_CDROMDISK, CDROMDISK_EXECMD,
                         &sdc, sizeof(sdc), &ulParamLen,
                         p_buffer, sizeof(p_buffer), &ulDataLen );

#else
#   error "DVD ioctls are unavailable on this system"

#endif
    return i_ret;
}

static int set_region(int fd, int region)
{
  int ret, region_mask;

  if(region > 8 || region <= 0) {
    printf("Invalid region( %d)\n", region);
    return 1;
  }
  printf("Setting drive region can only be done a finite " \
         "number of times, press Ctrl-C now to cancel!\n");
  getchar();

  region_mask = 0xff & ~(1 << (region - 1));
  printf("Setting region to %d( %x)\n", region, region_mask);
  if( (ret = ioctl_SendRPC(fd, region_mask)) < 0) {
    perror("dvd_region");
    return ret;
  }

  return 0;
}

static int print_region(int fd)
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
    printf("non-RPC( all)\n");

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
  char device_name[FILENAME_MAX], c, set, region = 0;
  int fd, ret;

  strcpy(device_name, DEFAULT_DEVICE);
  set = 0;
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
