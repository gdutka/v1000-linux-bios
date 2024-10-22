#!/usr/bin/perl
#ARGV[0] Input File
#ARGV[1] Output File
#ARGV[2] Output File Size after padding in Hex

use strict;
use warnings;

use Compress::Zlib;
use Data::Dumper;

#Do simple check if input parameter
if ($#ARGV != 2) {
 print 
 "Incorrect number of input parameters
 
 ARGV[0] Input File
 ARGV[1] Output File
 ARGV[2] Output File Size after padding in Hex
 ";
exit (0);
}

my $Level = 9;
my $WindowBits = 15;
my $Strategy = 0;
my $Pad = 0xFF;

my $x = deflateInit( -Level => $Level,
     -Method => Z_DEFLATED,
     -WindowBits => $WindowBits,
     -Strategy => $Strategy)
 or die "Cannot create a deflation stream\n";

my ($output, $status);
my $buffer;
my @Final = ();

my $input_file;

$input_file = $ARGV[0];
print "input_file: $input_file\n";
my $input_file_handle;
open ($input_file_handle, $input_file)
 or die "Failed to open file!\n";

my ($output_file, $output_file_handle);
$output_file = $ARGV[1];
print "output_file: $output_file\n";
open ($output_file_handle, ">", $output_file)
 or die "Failed to open output file\n";

my $OutputSize = hex ($ARGV[2]);
printf "OutputfileSize: %x\n", $OutputSize;

binmode $output_file_handle;
binmode $input_file_handle;

my $input_bytes;

$input_bytes = -s $input_file;

read ($input_file_handle, $buffer, $input_bytes)
 or die "Failed to read the entire binary file!\n";

($output, $status) = $x->deflate($buffer);
$status == Z_OK
 or die "deflate failed\n";
push @Final, $output;

($output, $status) = $x->flush();
$status == Z_OK
 or die "deflation failed\n";
push @Final, $output;

#Generate 256 bytes Header, offset 0x14 filled as Compressed image size excluding the header
#push(@Header, 0) for(1..256);
#$Header[0x14] =  scalar(@Final);
#Calculate the size of compressed blob
my $CompressSize = 0;
foreach (@Final) {
 my @TempBuf = unpack ("C*", $_);
 $CompressSize += scalar (@TempBuf);
}
printf "Size after Compress: 0x%x Bytes [%.2f%%] Full\n", $CompressSize, $CompressSize/$OutputSize*100;
unshift @Final, pack ("C20L1C232", ((0) x 20, $CompressSize, (0) x 232));


#Do the Padding, check if the input size is correct
my $SizeBeforePadding = 256 + $CompressSize;
if ($OutputSize < $SizeBeforePadding) {
 printf "Output file size (0x%x) set too small, should be larger than 0x%x\n", $OutputSize, $SizeBeforePadding;
 die;
}
#Do the Padding
push @Final, pack ("C*", ($Pad) x ($OutputSize - $SizeBeforePadding));

print $output_file_handle @Final;

printf "\nFile \"%s\" has been generated with padding of size 0x%x Bytes\n", $output_file, $OutputSize;

close $output_file_handle 
 or die "Failed to close output file!";
close $input_file_handle or die "failed to close input file!";