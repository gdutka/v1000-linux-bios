use File::Basename;
use lib dirname($0);
use File::Basename;
use File::Spec::Functions;
use Cwd qw(abs_path);
use lib 'C:\Program Files (x86)\AMD\Kysy\Perl';
use Time::HiRes qw(usleep);
use Getopt::Long; #Library to par
use Kysy;

#Date:7/14/2016 Version:1.00
# - 1st Publish vesion

#Date:6/30/2016 Version:0.02
# - Update Filter bit map
# - Update print filter function in show help
# - Print Version in help message

#Date:5/26/2016 Version:0.01
#Init Version
my $MajorVersion=0x1;
my $MinorVersion=0x00;
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
use constant APCB_BREAKPOINT_CMD_ABL_BP_FOUND  => 0x5000C001;         # Command from ABL indicating that BP found
use constant APCB_BREAKPOINT_CMD_ABL_BP_DONE => 0x5000DEAD;         # Command from ABL indicating that All BPs are done
use constant APCB_BREAKPOINT_CMD_CONTINUE   => 0xC001C001;        # Command from script to indicating continue execution
use constant APCB_BREAKPOINT_CMD_EXTERNAL_ENABLE  => 0xC001BEE5;  # External Command to enable breakpoints
use constant APCB_BREAKPOINT_CMD_BP_LIST  => 0xC001BB11;          # Command from script to indicating 

use constant BP_DATA_MAILBOX_REG => "PPR::UMC::CTRL::socket0::die0::umc0::UserDataPattern2";
use constant BP_CMD_MAILBOX_REG => "PPR::UMC::CTRL::socket0::die0::umc0::UserDataPattern3";

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

my %FilterBit2StrMap = reverse %FilterStr2BitMap;

my $gBpCounter = 0;

main ();

sub PrintVersion {
  printf "\n\nAblBp Ver %x.%02x\n",$MajorVersion,$MinorVersion;
}

sub main {
  ParseInput();
  PrintVersion  ();
  print "\"-h\" for help with usage\n\n";
  connect_to_target();
    # Stop script when Ctrl+C is pressed
  $SIG{INT} = sub { interrupt(); };

  # Handle crashes
  $SIG{BREAK} = sub { interrupt(); };
  $SIG{__DIE__} = sub { interrupt();};

  print "Wait till APCB_BREAKPOINT_CMD_EXTERNAL_ENABLE filled by ABL\n";
  while (readBpMailbox (BP_CMD_MAILBOX_REG) != APCB_BREAKPOINT_CMD_EXTERNAL_ENABLE) {
    ;
  }
  #Fill filter and send APCB_BREAKPOINT_CMD_BP_LIST command
  printf "Enable BP Filter 0x%x\n", $Filter;
  writeBpMailbox (BP_DATA_MAILBOX_REG, $Filter);
  writeBpMailbox (BP_CMD_MAILBOX_REG, APCB_BREAKPOINT_CMD_BP_LIST);
  # ctrl + c to break
  printf ("\nWaiting for BPs Press CTRL + C to EXIT ...\n", $gBpCounter++);
  while ($running) {
    #Delay to less impact host running
    usleep (100);
    # Pooling to find if BP occurs
    $cmd = readBpMailbox (BP_CMD_MAILBOX_REG);
    if ($cmd == APCB_BREAKPOINT_CMD_ABL_BP_FOUND) {
      # BPbreak set by ABL, BPID set by ABL
      # ABL set BPbreak, and waiting to be cleared
      printf ("\n============ BP Break (%d) ============\n", $gBpCounter++);
      # print BP information
      printBPInfo ();
      print "Any Key to continue>>>\n";
      my $input = <STDIN>;
      # send APCB_BREAKPOINT_CMD_CONTINUE
      writeBpMailbox (BP_CMD_MAILBOX_REG, APCB_BREAKPOINT_CMD_CONTINUE);
    } elsif ($cmd == APCB_BREAKPOINT_CMD_ABL_BP_DONE){
      print "All BPs completed\n";
      $running = 0;
    }
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

sub readBpMailbox {
  my $regpath = shift;
  my $reg = $platform->regByPath( $regpath);
  $accessLogic = $reg->accessLogic('SMN');
  $reg->read($accessLogic);
  return $reg->value();
}

sub writeBpMailbox {
  my $regpath = shift;
  my $val = shift;
  my $reg = $platform->regByPath($regpath);
  $accessLogic = $reg->accessLogic('SMN');
  $reg->read($accessLogic);
  $reg->value ($val);
  $reg->write ($accessLogic);
}

sub printBPInfo {
  $BpVal = readBpMailbox (BP_DATA_MAILBOX_REG);
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