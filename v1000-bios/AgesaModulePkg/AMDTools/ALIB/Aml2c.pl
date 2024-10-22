# Version 0.1

use Getopt::Long;

my $buffer;
my $filename=undef;
my $args = $#ARGV + 1;
my $filetag = "_PCIEALIBSSDT_H_";
my $datatag = "AlibSsdt";

$result = GetOptions ("filetag=s" => \$filetag,
                      "datatag=s" => \$datatag,
                      "file=s" => \$filename);

#if (args == 1 || $args == 3 ) {
#  if ($args == 3) {
#    if ($ARGV[0] eq "-filetag") {
#      $filetag = $ARGV[1];
#    } else {
#      die ("ERROR! Unknown argument.");
#    }
#  }
#} else {
#  die ("ERROR! Not anough arguments.");
#}

#my $filename = $ARGV[$#ARGV];
open(DAT, $filename) || die("ERROR! Could not open file!");
$filesize = -s $filename;
binmode (DAT);
read(DAT, $buffer, $filesize) or die ("ERROR! Could not read file!");


my $i = 0;
my $s = "";

print  <<"XXXXXX";
/**
 * \@file
 *
 * ALIB SSDT table
 *
 *
 *
 * \@xrefitem bom "File Content Label" "Release Content"
 * \@e project:     AGESA
 * \@e sub-project: GNB
 * \@e \\\$Revision:\$   \@e \\\$Date:\$
 *
 */
/*
*****************************************************************************
*
* Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
*
* ***************************************************************************
*
*/

XXXXXX

print      "#ifndef ".$filetag."\n"
           ."#define ".$filetag."\n\n"
           ;

print "UINT8  ${datatag}[] = {\n  ";
foreach (split(//, $buffer)) {
    if ($i != 0) {
      if (($i & 0x7) == 0) {
        $s.=",\n  ";
      } else {
        $s.=", ";
      }
    }
    #if (($i & 0x7) == 0) {
    #  $s.="\n  ";
    #}
    $s.= sprintf("0x%02X", ord($_));
    ++$i;
}

$s = $s."\n};\n\n"."#endif\n";

printf  $s;
close(DAT);
