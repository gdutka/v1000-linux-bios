use File::Basename;
use lib dirname($0);
use File::Basename;
use File::Spec::Functions;
use Cwd qw(abs_path);
use lib 'C:\Program Files (x86)\AMD\Kysy\Perl';
use Time::HiRes qw(usleep);
use Getopt::Long; #Library to par
use Kysy;
use Benchmark;
#Date:04/14/2017 Version:0.02
#Add multiple die support

#Date:12/09/2016 Version:0.01
#Init Version

my $MajorVersion=0x0;
my $MinorVersion=0x01;
# # # # # # # # # # # # # # # # # # # # #
# Control flags                         #
# # # # # # # # # # # # # # # # # # # # #


my $dft_ip_addr = "192.168.1.1";

my $default_password = "hwe";
my $default_username = "hwe";

# # # # # # # # # # # # # # # # # # # # #
# Global Values                         #
# # # # # # # # # # # # # # # # # # # # #

my $help;
my $ip_addr;
my $password;
my $username;
my $running = 1;
my $Filter;
# Select which socket & die print on the console window
my $ConsolePrintSocket = 0;
my $ConsolePrintDie = 1;
# The structure used to record all ablout related information per die
my @AblOutInfo;
my $numDieEn = 0;
use constant ABL_DBG_CMD_ABL_BP_FOUND  => 0x5000C001;         # Command from ABL indicating that BP found
use constant ABL_DBG_CMD_ABL_BP_DONE => 0x5000DEAD;         # Command from ABL indicating that All BPs are done
use constant ABL_DBG_CMD_SCRIPT_CONTINUE   => 0xC001C001;        # Command from script to indicating continue execution
use constant ABL_DBG_CMD_ABL_BP_INIT  => 0xC001BEE5;  # External Command to enable breakpoints
use constant ABL_DBG_CMD_SCRIPT_BP_LIST  => 0xC001BB11;          # Command from script to indicating 
use constant ABL_DBG_CMD_ABL_HDTOUT_PRINT => 0x99CC9000;          # Command from ABL to script indicating that print ASCII from Script register buffer
# use constant ABL_DBG_DATA_MAILBOX_REG => "PPR::UMC::CTRL::socket0::die0::umc0::UserDataPattern2";
# use constant ABL_DBG_CMD_MAILBOX_REG => "PPR::UMC::CTRL::socket0::die0::umc0::UserDataPattern3";
use constant ABL_DBG_DATA_MAILBOX_REG => 0x51058;
use constant ABL_DBG_CMD_MAILBOX_REG => 0x5105C;

my %FilterStr2BitMap = (
"UMC_PHASE_1_END"                => 0x1 << 00,
"UMC_START_PMU_ON_IMAGE"         => 0x1 << 01,
"UMC_MISSION_MODE_CONFIG"        => 0x1 << 02,
"UMC_PMU_FINISHED"               => 0x1 << 03,
"UMC_2D_TRAINING_DONE"           => 0x1 << 04,
"UMC_BEFORE_INITIAL_PSTATE"      => 0x1 << 05,
"BP_UMC_MEM_PHASE_2_END"         => 0x1 << 06,
"UMC_AFTER_INITIAL_PSTATE"       => 0x1 << 07,
"DF_INIT_START"                  => 0x1 << 10,
"DF_BEFORE_LINK_TRAINING"        => 0x1 << 11,
"DF_LINK_TRAINING_COMPLETE"      => 0x1 << 12,
"DF_BEFORE_CREDIT_RELEASE"       => 0x1 << 13,
"DF_INIT_COMPLETE"               => 0x1 << 14,
"APCB_BP_CUSTOM"                 => 0x1 << 30,
"APCB_BREAKPOINT_DONE"           => 0x1 << 31,
);

my @mHdtOutBufferTranslationTbl = (
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_0",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_1",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_2",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_3",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_4",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_5",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_6",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_7",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_8",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_9",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_10",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_11",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_12",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_13",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_14",
  "PPR::NBIFMM::socket0::die0::NBIF0::BIOS_SCRATCH_15",

  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_0",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_1",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_2",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_3",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_4",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_5",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_6",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_7",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_8",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_9",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_10",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_11",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_12",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_13",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_14",
  "PPR::NBIFMM::socket0::die0::NBIF1::BIOS_SCRATCH_15",

  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_0",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_1",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_2",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_3",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_4",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_5",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_6",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_7",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_8",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_9",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_10",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_11",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_12",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_13",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_14",
  "PPR::NBIFMM::socket0::die0::NBIF2::BIOS_SCRATCH_15",
);


my %FilterBit2StrMap = reverse %FilterStr2BitMap;

my $gBpCounter = 0;

main ();

sub dwordChr {
  my $dword = shift;
  return chr(($dword & 0xFF) >> 0). chr(($dword & 0xFF00) >> 8). chr(($dword & 0xFF0000) >> 16) . chr(($dword & 0xFF000000) >> 24);
}

sub PrintVersion {
  printf "\n\nAblOut Ver %x.%02x\n",$MajorVersion,$MinorVersion;
}

sub readHdtBufferStr1 {
  my $sizeInDword = shift;
  my $str;
  for ($i = 0; $i < $sizeInDword; $i++) {
    $str .= dwordChr (readRegBySmnPath ($mHdtOutBufferTranslationTbl [$i]));
  }
  return $str;
}
sub readHdtBufferStr {
  my $socket = shift;
  my $die = shift;
  my $size = shift;
  my $str;
  # Below codes is make assumption scratch register address continuals
  # if ($size) {
  #   my $rdsize = $size > 16 ? 16 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10120120, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  # if ($size) {
  #   my $rdsize = $size > 16 ? 16 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10220120, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  # if ($size) {
  #   my $rdsize = $size > 16 ? 16 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10320120, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  # if ($size) {
  #   my $rdsize = $size > 64 ? 64 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10120200, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  # if ($size) {
  #   my $rdsize = $size > 64 ? 64 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10220200, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  # if ($size) {
  #   my $rdsize = $size > 64 ? 64 : $size;

  #   $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10320200, $rdsize, 5);
  #   $SmnAxiBufAccess->read ();
  #   # print $SmnAxiBufAccess->size();
  #   for ($i = 0; $i < $rdsize; $i++) {
  #     $str .= chr $SmnAxiBufAccess->byte ($i);
  #   }
  #   $size -= $rdsize;
  # }
  if ($size) {
    my $rdsize = $size > 31*4 ? 31*4 : $size;

    $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, $socket, $die, 0x00001DB84, $rdsize, 5);
    $SmnAxiBufAccess->read ();
    # print $SmnAxiBufAccess->size();
    for ($i = 0; $i < $rdsize; $i++) {
      $str .= chr $SmnAxiBufAccess->byte ($i);
      # printf "%x\n", $SmnAxiBufAccess->byte ($i);
    }
    $size -= $rdsize;
  }
  return $str;
}

sub readHdtBufferHex {
  my $sizeInDword = shift;
  for ($i = 0; $i < $sizeInDword; $i++) {
    printf "%04x ", readRegBySmnPath ($mHdtOutBufferTranslationTbl [$i]);
    printf OUTPUTFILE "%04x ", readRegBySmnPath ($mHdtOutBufferTranslationTbl [$i]);

    if (($i+1) % 4 == 0) {
      print "\n";
      print OUTPUTFILE "\n";
    }
  }
}
sub printTime {
  my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
  print "\n==$hour:$min:$sec==\n";
  print OUTPUTFILE "\n==$hour:$min:$sec==\n";
}

sub IsValidCmd {
  my $Cmd = shfit;
  return (($Cmd == ABL_DBG_CMD_ABL_BP_FOUND) || ($cmd == ABL_DBG_CMD_ABL_BP_DONE) || (($cmd & 0xFFFFF000) == ABL_DBG_CMD_ABL_HDTOUT_PRINT));
}

my $perf = 0;

sub main {
  ParseInput();
  PrintVersion  ();

  my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
  if ($perf) {
    $perflogfile = sprintf("C:\\HDTOUT\\hdtout_%d_%d_%d_%02d_%02d_%02d_time.txt",$mon+1,$mday,$year+1900,$hour,$min,$sec);
    open(PERFLOG, ">$perflogfile") or die "can't open $perflogfile\n";
  }
  print "\"-h\" for help with usage\n\n";
  connect_to_target();
  printTime ();
  # test ();
    # Stop script when Ctrl+C is pressed
  $SIG{INT} = sub { interrupt(); };

  # Handle crashes
  $SIG{BREAK} = sub { interrupt(); };
  $SIG{__DIE__} = sub { interrupt();};

  # ctrl + c to break
  printf ("\nWaiting for BPs Press CTRL + C to EXIT ...\n", $gBpCounter++);

  # Get the socket and die information
  $access = $platform->platformAccess();
  $coreTopology = new Kysy::CoreTopologyInfo ($access);
  $coreTopology->refreshMappingData ();
  $numSocketsPerPlatform = $coreTopology->sockets;
  $components = $coreTopology->coreTopologyComponentsCount(0);
  $numDiesPerSocket = $components->dies;
  $numCCXPerDie = $components->ccxs;
  $numCoresPerCCX = $components->cores;
  $numThreadsPerCore = $components->threads;
  printf ("%d Sockets %d DiesPerSocket %d CCXPerDie %d CoresPerCCX %d ThreadsPerCore\n", $numSocketsPerPlatform, $numDiesPerSocket, $numCCXPerDie, $numCoresPerCCX, $numThreadsPerCore);

  # create Abl debug information instance for dies
  my $i = 0;
  for ($socket = 0; $socket < $numSocketsPerPlatform; $socket++ ) {
    for ($die = 0; $die < $numDiesPerSocket; $die++ ) {
      my $AblOutInfoUnit = {};
      $AblOutInfoUnit->{"Socket"} = $socket;
      $AblOutInfoUnit->{"Die"} = $die;
      $AblOutInfoUnit->{"AblOutCmd"} = Kysy::SMNAxiBufferAccess::create ($platform, $socket, $die, ABL_DBG_CMD_MAILBOX_REG, 4, 5);
      $AblOutInfoUnit->{"AblOutData"} = Kysy::SMNAxiBufferAccess::create ($platform, $socket, $die, ABL_DBG_DATA_MAILBOX_REG, 4, 5);
      $logfilestr = sprintf ("%s_S%d_D%d.txt", $logfile, $socket, $die);
      $SmnAblDbgCmd = Kysy::SMNAxiBufferAccess::create ($platform, $socket, $die, ABL_DBG_CMD_MAILBOX_REG, 4, 5);
      $curlogfile = $AblOutInfoPerDie->{"LogFile"};
      $SmnAblDbgCmd->read ();
      $cmd = $SmnAblDbgCmd->dword(0);
      if (IsValidCmd ($cmd)) {
        printf "ABLOUT enable on Socket [%d] Die [%d]\n", $socket, $die;
        open($AblOutInfoUnit->{"LogFile"}, ">$logfilestr") or die "can't open $logfilestr\n";
        sleep (1);
        $AblOutInfo[$i++] = $AblOutInfoUnit;
        $numDieEn++;
      }
    }
  }

  $TT0 = new Benchmark;

  $TT0 = new Benchmark;
  while ($running) {
    foreach $AblOutInfoPerDie(@AblOutInfo) {
      $curSocket = $AblOutInfoPerDie->{"Socket"};
      $curDie = $AblOutInfoPerDie->{"Die"};
      $SmnAblDbgCmd = Kysy::SMNAxiBufferAccess::create ($platform, $curSocket, $curDie, ABL_DBG_CMD_MAILBOX_REG, 4, 5);
      $SmnAblDbgData = Kysy::SMNAxiBufferAccess::create ($platform, $curSocket, $curDie, ABL_DBG_DATA_MAILBOX_REG, 4, 5);
      $curlogfile = $AblOutInfoPerDie->{"LogFile"};
      # Pooling to find if BP occurs
      # $TT0 = new Benchmark;
      $XT0 = new Benchmark;
      $SmnAblDbgCmd->read ();
      $XT1 = new Benchmark;
      $cmd = $SmnAblDbgCmd->dword(0);
      # printf "%d-%d %x\n", $curSocket, $curDie, $cmd;
      if ($cmd == ABL_DBG_CMD_ABL_BP_FOUND) {
        # BPbreak set by ABL, BPID set by ABL
        # ABL set BPbreak, and waiting to be cleared
        printf ("\n============ BP Break (%d) ============\n", $gBpCounter++);
        # print BP information
        printBPInfo ();
        print "Any Key to continue>>>\n";
        my $input = <STDIN>;
        # send ABL_DBG_CMD_SCRIPT_CONTINUE
        $SmnAblDbgCmd->dword (0, ABL_DBG_CMD_SCRIPT_CONTINUE);
        $SmnAblDbgCmd->write ();
      } elsif ($cmd == ABL_DBG_CMD_ABL_BP_DONE){
        print "ABL boot Done\n";
        $running = 0;
      } elsif (($cmd & 0xFFFFF000) == ABL_DBG_CMD_ABL_HDTOUT_PRINT) {
        my $bufsize = $cmd & 0xFFF;
        # print "\n===========================\n";
        # print OUTPUTFILE "\n===========================\n";
        # $TT1 = new Benchmark;
        my $str = readHdtBufferStr ($curSocket, $curDie, $bufsize);
        # $TT2 = new Benchmark;
        # readHdtBufferHex ($bufsize);
        # die;
        # send ABL_DBG_CMD_SCRIPT_CONTINUE
        if ($perf) {
          $XT2 = new Benchmark;
        }

        $SmnAblDbgCmd->dword (0, ABL_DBG_CMD_SCRIPT_CONTINUE);
        $SmnAblDbgCmd->write ();
        if ($perf) {
          $XT3 = new Benchmark;
        }
        # writeRegBySmnPath (ABL_DBG_CMD_MAILBOX_REG, ABL_DBG_CMD_SCRIPT_CONTINUE);
        # Print console for selected die
        # if (($ConsolePrintSocket == $CurSocket) && ($ConsolePrintDie == $CurDie)) {
        if ($numDieEn > 1) {
          printf "\n[%d-%d]:\n", $curSocket, $curDie;
        }
        print $str;
        # }
        print $curlogfile $str;
        if ($perf) {
          $XT4 = new Benchmark;
        }
        # print "Any Key to continue>>>\n";
        # my $input = <STDIN>;
        # # $TT3 = new Benchmark;
        if ($perf) {
          $td = timediff ($XT1, $XT0);
          print PERFLOG "read command T1->T0 took:".timestr($td)."\n";
          $td = timediff ($XT2, $XT1);
          print PERFLOG "read SMN T2->T1 took:".timestr($td)."\n";
          $td = timediff ($XT3, $XT2);
          print PERFLOG "write cmd T3->T2 took:".timestr($td)."\n";
          $td = timediff ($XT4, $XT3);
          print PERFLOG "print T4->T3 took:".timestr($td)."\n";
        }
     }
    }
  }
  $TT1 = new Benchmark;
  $td = timediff ($TT1, $TT0);
  print "T1->T0 took:".timestr($td)."\n";
  # Close all open files
  foreach $AblOutInfoPerDie(@AblOutInfo) {
    close $AblOutInfoPerDie->{"LogFile"};
  }
}

sub readLastPc {
  my $lastpc = $platform->regByPath( "PPR::FCH::MISC::socket0::die0::PostCode");
  $lastpc->read();
  return $lastpc->value();
}

sub PcStackHexArray {
  my @pcArrary;
  my $pc = $platform->regByPath( "PPR::FCH::MISC::socket0::die0::PostCodeStack");
  $accessLogic =  $pc->accessLogic('SMN');
  for ($i = 0; $i < 8; $i++) {
    $pc->read($accessLogic);
    push @pcArrary, $pc->value ();
  }
  
  return reverse @pcArrary;
}

#Str join by "\n"
sub PcStackHexStr {
  my $PcStackHexStr;

  foreach (PcStackHexArray ()) {
    $PcStackHexStr.= sprintf ("0x%08X  %s\n", $_, hextochar ($_));
  }
  return $PcStackHexStr;
}
sub hextochar {
  my $hex = shift;
  my $str;
  $str = sprintf ("%c%c%c%c", ($hex>>24) & 0xFF, ($hex>>16) & 0xFF, ($hex>>8) & 0xFF, ($hex>>0) & 0xFF);
  return $str;
}

sub readRegBySmnPath {
  my $regpath = shift;
  my $reg = $platform->regByPath( $regpath);
  $accessLogic = $reg->accessLogic('SMN');
  $reg->read($accessLogic);
  return $reg->value();
}

sub writeRegBySmnPath {
  my $regpath = shift;
  my $val = shift;
  my $reg = $platform->regByPath($regpath);
  $accessLogic = $reg->accessLogic('SMN');
  $reg->read($accessLogic);
  $reg->value ($val);
  $reg->write ($accessLogic);
}

sub printBPInfo {
  $BpVal = readRegBySmnPath (ABL_DBG_DATA_MAILBOX_REG);
  # Print Raw BP ID
  printf "BP ID [%x]: %s\n",$BpVal , $FilterBit2StrMap{$BpVal};
  # Print Last 8 post codes
  print "PostCode Stack:\n[ HexVal ]  [Str]\n".PcStackHexStr ()."\n";
}

sub ParseYaap {
  my @yaap = @_;
  my %YaapHash;
  foreach (@yaap) {
    if(/^\s*(.+)=(.+)\s*$/) {
      $YaapHash{uc $1}=$2;
    }
  }
  foreach (keys %YaapHash) {
    if ((uc $_ ne "IP") and
        (uc $_ ne "USERNAME") and
        (uc $_ ne "PASSWORD")){
        printf "Unrecognized parameter %s in YAAP group\n\n",$_;
        $help = 1;
    }
  }
  $ip_addr = $YaapHash{"IP"};
  $password = $YaapHash{"PASSWORD"};
  $username = $YaapHash{"USERNAME"};

}

sub connect_to_target {
  if ($ip_addr ne "") {
      print ("Connecting to HDT device $ip_addr $username $password ...\n");
      my $wombat = undef;
      $wombat = Kysy::Wombat::create ("$ip_addr", "$username", "$password");
      # $wombat = Kysy::Wombat::create( "10.236.12.26", "dju", "dju" );
      $platform = $wombat->platform();
  } else {
    die "No specified IP address\n";
  }
}

sub Initdefault {
  if ($logfile eq '') {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    $logfile = sprintf("C:\\HDTOUT\\hdtout_%d_%d_%d_%02d_%02d_%02d",$mon+1,$mday,$year+1900,$hour,$min,$sec);
    mkdir ("C://HDTOUT");
  }
  if ($ip_addr eq '') {
    $ip_addr = $dft_ip_addr;
  }
  if ($password eq '') {
    $password =$default_password;
  }
  if ($username eq '') {
    $username =$default_username;
  }
}

sub PrintFilters {
  foreach (keys %FilterStr2BitMap) {
    printf "  %s  | ", $_;
    if ((++$i % 3) == 0) {
      print "\n";
    }
  }
}

sub ShowHelp {
  PrintVersion ();
  print   "\n============================== HELP =================================\n";
  print   "  Tips:All parameter is case-insensitive\n";
  print   "       Use first letter instead of whole word like -Y is short of --Yaap\n\n";
  print   "\n\nUSAGE: perl AblBp.pl\n";
  print   "  -Y --YAAP [ip=ipaddress] [username=name] [password=password]\n";
  print   "  -H --HELP get help\n";
  print   "  -F --Filter Filters Define the filter of BreakPoint\n";
  print   "  Filters:\n";
  PrintFilters ();
  print   "\n    e.g. -Filter UMC_PHASE_1_END DF_INIT_START \n";
  print   "            Ignore other breakpoint except UMC_PHASE_1_END & DF_INIT_START\n";
  print   "    Tips: Wildcard is accepted, use few character instead of whole word group\n";
  print   "    U is Wildcard of all UMC BPs\n";
  print   "    df is Wildcard of All DF BPs\n";
  print   "\n    Use Ctrl+C or Ctrl+Break to stop the script if it does not response.\n\n";
  exit 0;
}

sub ParseInput {
  my @yaap;
  my @filters;
  GetOptions(
            'help'              => \$help,
            'logfile=s'         => \$logfile,
            'yaap=s{0,3}'       => \@yaap,
            'filter=s{0,64}'    => \@filters,
        ) or die "Incorrect usage! Try --help for help with usage\n";

  ParseFilter(@filters);
  ParseYaap(@yaap);

#Init default value
  Initdefault();


  printf  "    ip_addr: %s \n",$ip_addr;
  printf  "    password: %s \n", $password;
  printf  "    username: %s \n", $username;
  printf  "    logfile: %s \n",$logfile;
  if ($help) {
    ShowHelp();
  }

}

sub interrupt {
    $running = 0;
}

sub ParseFilter {
  my @filters = @_;

  my @ParsedFilter;
  foreach my $Prefix(@filters) {
      if ((grep {/^($Prefix)/i}keys %FilterStr2BitMap) == ()) {
        printf "Can't found filter with \"%s\" prefix, ignored\n",$Prefix;
      } else {
        push @ParsedFilter,grep {/^($Prefix)/i}keys %FilterStr2BitMap;
      }
  }
  #generate the final bitsmap
  printf  "filters: \n";
  foreach (@ParsedFilter){
    print "\t".$_."\n";
    my $f = uc $_;
    $Filter |= $FilterStr2BitMap{$f};
  }
  #Always enable APCB_BP_CUSTOM & APCB_BREAKPOINT_DONE
  $Filter |= $FilterStr2BitMap{"APCB_BP_CUSTOM"};
  $Filter |= $FilterStr2BitMap{"APCB_BREAKPOINT_DONE"};
}

sub test {
    # $TT0 = new Benchmark;
 
      # my $str = readHdtBufferStr (240);
#       $TT1 = new Benchmark;

#       my $str = readHdtBufferStr (16);
#       $TT2 = new Benchmark;

#       my $str = readHdtBufferStr (32);
      # $TT0 = new Benchmark;
      # readRegBySmnPath (ABL_DBG_CMD_MAILBOX_REG);
      # $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10320200, 4, 5);
      # $SmnAxiBufAccess->read ();
      # $TT1 = new Benchmark;
      # $srsmindex = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x510A0, 4, 5);
      # $srsmdata= Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x510A4, 4, 5);
      # for (my $i = 0; $i < 60; $i++) {
      #   $srsmindex->dword (0, $i);
      #   $srsmindex->write ();
      #   $srsmdata->read ();
        # printf ("%08x ", $srsmdata->dword (0));
        # if (($i + 1) % 4 == 0) { print "\n"}
      # }
      # $SmnAxiBufAccess->dword (0,0x11111111);
      # $SmnAxiBufAccess->write ();
      # writeRegBySmnPath (ABL_DBG_CMD_MAILBOX_REG, ABL_DBG_CMD_SCRIPT_CONTINUE);
      # $TT2 = new Benchmark;

      # $td = timediff ($TT1, $TT0);
      # print "T1->T0 took:".timestr($td)."\n";
      # $td = timediff ($TT2, $TT1);
      # print "T2->T1 took:".timestr($td)."\n";
      # die;
#       $td = timediff ($TT3, $TT2);
#       print "T3->T2 took:".timestr($td)."\n";
#       $td = timediff ($TT4, $TT3);
#       print "T4->T3 took:".timestr($td)."\n";
  # $SmnAxiBufAccess = Kysy::SMNAxiBufferAccess::create ($platform, 0, 0, 0x10120200, 64, 5);
  # $SmnAxiBufAccess->read ();
  # # print $SmnAxiBufAccess->size();
  # for ($i = 0; $i < 64; $i++) {
  #   $str .= chr $SmnAxiBufAccess->byte ($i);
  # }
  # print $str;
  # # printf "%x", $SmnAxiBufAccess->byte (0);
  # die;
# die;
}