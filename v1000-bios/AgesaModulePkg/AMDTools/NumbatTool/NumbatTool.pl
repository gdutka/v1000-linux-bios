use File::Basename;
use lib dirname($0);
use File::Basename;
use File::Spec::Functions;
use Cwd qw(abs_path);
use lib 'C:\Program Files (x86)\AMD\Kysy\Perl';
use Time::HiRes qw(usleep);
use Getopt::Long; #Library to par
use Kysy;
use Data::Dumper;
use XML::Simple qw(XMLin XMLout );
use bigint;
use bignum;

#Global dft setting, will be used if not specified in CMD line parameters
# Wombat IP, Password, User Name
my $dft_ip_addr = "192.168.1.1";
my $default_password = "hwe";
my $default_username = "hwe";
# Time for collect the postcode
my $dft_seconds = 10;
# Control for reset at script start up
my $resetAtStartUp = 1;

my $ver = "1.01";
#Version 1.01 08/17/2017
#        Add support CTRL+C break ouut

#Version 1.00 12/28/2016
#        Initial check in
#Parse Input, Init global setting
ParseInput ();
#HDT connect
my $logfile;
PrintVersion ();
print ("Connecting to HDT device $ip_addr $username $password ...\n");
my $wombat = undef;
$wombat = Kysy::Wombat::create ("$ip_addr", "$username", "$password");
open(RAWXML, ">rawdata.xml") or die "can't open rawdata.xml\n";
# open(REFINEDATA, ">refine.Data") or die "can't open refine.data\n";
open(CSV, ">$logfile") or die "can't open $logfile\n";

#lpcReader constructor will throw out the error, if LPC connector not connected
#No need to 
print ("Connecting to Numbat LPC Reader\n");
$lpcReader = $wombat->lpcReader ();
#Set & Check Numbat capability
print ("Set & Check Numbat capability\n");
$numbatOptions = $lpcReader->options ();
#bool Kysy::LPCReaderOptions::passiveMonitor
#True enables passive monitoring of POST Codes (not claiming the LPC cycle). False enables actively claiming POST Code cycles (claiming the LPC cycle).
$numbatOptions->{"passiveMonitor"} = 1;
# bool Kysy::LPCReaderOptions::circularBuffer;
# True enables circular buffering (discard oldest data rather than newest on overflow).
$numbatOptions->{"circularBuffer"} = 1;
# bool Kysy::LPCReaderOptions::autoReset
# True enables automatic clearing of the FIFO based on certain system conditions
$numbatOptions->{"autoReset"} = 1;
$lpcReader->options ($numbatOptions);

$numbatOptions = $lpcReader->options ();
printf "\tPassive Monitor: %x: [%s]\n", $numbatOptions->{"passiveMonitor"}, ($numbatOptions->{"passiveMonitor"} == 1) ? "Set Correctly" : "[Warning] Value unexpected";
printf "\tFifo Circular Buffer: %x: [%s]\n", $numbatOptions->{"circularBuffer"},  ($numbatOptions->{"circularBuffer"} == 1) ? "Set Correctly" : "[Warning] Value unexpected";
printf "\tFifo Auto Reset: %x: [%s]\n",  $numbatOptions->{"autoReset"}, ($numbatOptions->{"autoReset"} == 1) ? "Set Correctly" : "[Warning] Value unexpected";
# Port Numbers
$port = 0x80;
$lpcReader->port ($port);
$port = $lpcReader->port ();
printf "\tLPC port 0x%x: [%s]\n",  $port, ($port == 0x80) ? "Set Correctly" : "[Warning] Value unexpected";
#Clear the buffer & status
# print ("Clear the buffer & status\n");
# $lpcReader->clearStatus ();
# $lpcReader->clear();
# printLpcReaderStatus ();

#Issue the reset
if ($resetAtStartUp) {
  print "Issue Cold reset via relay [!!!] Please keep Wombat Cold reset relay connected\n";
  $wombat->platformPower->coldReset ();
}
# Stop script when Ctrl+C is pressed
$SIG{INT} = sub { interrupt(); };

# Handle crashes
$SIG{BREAK} = sub { interrupt(); };

$running = 1;
#Collect the raw data
while (($seconds--) && $running) {
  print "\n\n========================== $seconds S ==========================\n";
  printLpcReaderStatus ();
  if ($lpcStatus->overflow || $lpcStatus->full) {
    print ("[Warning] FIFO overflow or full detected, some postCode may be missed\n", )
  }
  $buffer = $lpcReader->read ();
  $postCodeRawXml .= $lpcReader->serialize ($buffer);
# $postCodeReader = Kysy::PostCodeReader::create ($buffer);
# $postCodes = $postCodeReader->postCodes;
# print $postCodes->size;
# while ($postCode = $postCodes->pop) {
#   print REFINEDATA $postCode->toString."\n";
# }
  sleep (1);
}

#process the raw data make it to parsed by XMLIN module
$postCodeRawXml = "<Root>\n".$postCodeRawXml."</Root>\n";
@postCodeRawXmlArray = split /\n/, $postCodeRawXml;
foreach (@postCodeRawXmlArray) {
  if (/\<LPCBytes\>/) {
  } elsif (/\<\/LPCBytes\>/) {
  } else {
    $postCodeRawXmlRefine .= $_."\n";
  }
}
print RAWXML $postCodeRawXmlRefine;

$postCodeXmlin = XMLin("$postCodeRawXmlRefine", forcearray => "LPCByte");
#strip out the line with statusByte True, timestamp equal to 0
$LpcBytes = $postCodeXmlin->{'LPCByte'};
$postcodesRefined = [];
foreach $postCode (@$LpcBytes) {
  # if (($postCode->{"statusByte"} eq 'false') && ($postCode->{"timeStamp"}) ne '0x0000000000000000') {
    push @$postcodesRefined, $postCode;
  # }
}
# print REFINEDATA Dumper $postcodesRefined;
#Do the merge and calculate the final data
$postcodesFinal = [];
$i = 0;
$abstimestamp = 0;
$numEntry = scalar (@$postcodesRefined);
for ($i = 0; $i < $numEntry; $i++) {
  $postCode = $postcodesRefined->[$i];
  # Get the timestamp from entry with offset value 0
  if ($postCode->{'offset'} eq '0x00000000') {
    $reltimestamp = $postCode->{'timeStamp'};
    $postCodeHex  = 0;
    $abstimestamp += hex ($reltimestamp);
  }
  #merge the 4 data as one UINT32 post code
  $shiftNum = hex ($postCode->{'offset'}) * 8;
  $postCodeHex |= hex ($postCode->{'data'}) << $shiftNum;
  # Check if finish one record: next entry offset 0 or end of list
  if (($postcodesRefined->[$i+1]->{'offset'} eq '0x00000000') || (($i + 1) == scalar @$postcodesRefined)) {
    # print $abstimestamp;
    # printf " %s - 0x%08x\n", $reltimestamp, $postCodeHex;
    push $postcodesFinal, {'postCode' => $postCodeHex, 'abstimestamp'=>$abstimestamp, 'reltimestamp'=>$reltimestamp,};
  }
}

# Generate final csv file
print CSV "POSTCODE,abs.TimeStamp(ms),rel.Timestamp(ms)\n";
foreach $postCode(@$postcodesFinal) {
  printf CSV "0x%x,", $postCode->{'postCode'};
  printf CSV "%f", $postCode->{'abstimestamp'}/1000000;
  print CSV ",";
  printf CSV "%f", hex($postCode->{'reltimestamp'})/1000000;
  print CSV "\n";
}

print "Numbat data has been saved to $logfile\n";
# $buffer = $lpcReader->read ();
# print Dumper( $PostCode );

sub interrupt ()
{
  $running = 0;
}
sub printLpcReaderStatus ()
{
  $lpcStatus = $lpcReader->status ();
  # printf ("\tlpcStatus.overflow: %x: [%s]\n", $lpcStatus->overflow, ($lpcStatus->overflow == 0) ? "Set Correctly" : "[Warning] Value unexpected");
  # printf ("\tlpcStatus.underflow: %x: [%s]\n", $lpcStatus->underflow, ($lpcStatus->underflow == 0) ? "Set Correctly" : "[Warning] Value unexpected");
  # printf ("\tlpcStatus.count: %x: [%s]\n", $lpcStatus->count, ($lpcStatus->count == 0) ? "Set Correctly" : "[Warning] Value unexpected");
  # printf ("\tlpcStatus.empty: %x: [%s]\n", $lpcStatus->empty, ($lpcStatus->empty == 1) ? "Set Correctly" : "[Warning] Value unexpected");
  # printf ("\tlpcStatus.full: %x: [%s]\n", $lpcStatus->full, ($lpcStatus->full == 0) ? "Set Correctly" : "[Warning] Value unexpected");
  printf ("\tlpcStatus.overflow: %x\n");
  printf ("\tlpcStatus.underflow: %x\n");
  printf ("\tlpcStatus.count: %x\n");
  printf ("\tlpcStatus.empty: %x\n");
  printf ("\tlpcStatus.full: %x\n");
}

sub PrintVersion {
  print "+-------------------------------------------------------------------+\n";
  print "|                                                                   |\n";
  print "|                   Numbat Tool Version $ver                        |\n";
  print "|                                                                   |\n";
  print "+-------------------------------------------------------------------+\n";
}
sub ShowHelp {
  PrintVersion ();
  print   "\n============================== HELP =================================\n";
  print   "  Tips:All parameter is case-insensitive\n";
  print   "       Use first letter instead of whole word like -Y is short of --Yaap\n\n";
  print   "\n\nUSAGE: perl AblBp.pl\n";
  print   "  -Y --YAAP [ip=ipaddress] [username=name] [password=password]\n";
  print   "    default is IP=$dft_ip_addr, username=$default_username, password=$default_password, \n";
  print   "  -H --HELP get help\n";
  print   "  [-Logfile FILE] Specify the logfile name include path\n";
  print   "    default is %cd%\\PostCode_YMDT.csv \n";
  print   "    -Logfile C:\\Mylog\\PostCode.csv save the logfile to C:\\Mylog\\PostCode.csv\n";
  print   "  [-Time timeinsecond] Specify the time in seconds\n";
  print   "    default is $dft_seconds\n";
  exit 0;
}

sub ParseInput {
  my @yaap;
  my @filters;
  GetOptions(
            'help'              => \$help,
            'logfile=s'         => \$logfile,
            'yaap=s{0,3}'       => \@yaap,
            'time=s'             => \$seconds,
        ) or die "Incorrect usage! Try --help for help with usage\n";
  $logfile =~ s/\\/\//g;
  ParseYaap(@yaap);

#Init default value
  Initdefault();

  printf  "    logfile: %s \n",$logfile;
  printf  "    ip_addr: %s \n",$ip_addr;
  printf  "    password: %s \n", $password;
  printf  "    username: %s \n", $username;
  printf  "    seconds: %s \n", $seconds;

    if ($help) {
      ShowHelp();
    }

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

sub Initdefault {
  if ($logfile eq '') {
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime;
    $logfile = sprintf("PostCode_%d_%d_%d_%02d_%02d_%02d.csv",$mon+1,$mday,$year+1900,$hour,$min,$sec);
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
  if ($seconds eq '') {
    $seconds = $dft_seconds;
  }
}