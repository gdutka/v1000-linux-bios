################################################
#TO generate dsc, fdf files             	   #
#for AGESA-V9                                  #
#Author: Rick Xu                               #
#        Lance Han                             #
################################################

#!/usr/bin/perl
use strict;
use warnings;

################################################
#global variables                              #
################################################

my @gl_Program;
my @gl_PlatForm;
my %gl_MacroMap;
my $gl_importPath = "";
my $gl_exportPath = "";
my $gl_fdfExportPath = "";
my $gl_programs;
my $gl_platFormNames = "";
my $gl_outputFileName = "";

&main();

################################################
#Main Process                                  #
################################################
sub main{
    my (@param) = @ARGV;
    my @outputList;
    &parseParam(@param);
    print "Parsing .inc.dsc file...\n";
    my @symbols = &symbolsMapping();
    @outputList = &parseFile($gl_importPath . "AgesaModulePkg.inc.dsc", \@symbols);
    &genBuildFile(".inc.dsc",\@outputList);
    print "Parsing .dxe.inc.fdf...\n";
    @outputList = &parseFile($gl_importPath . "AgesaModulePkg.dxe.inc.fdf", \@symbols);
    &genBuildFile(".dxe.inc.fdf",\@outputList);
    print "Parsing .pei.inc.fdf...\n";
    @outputList = &parseFile($gl_importPath . "AgesaModulePkg.pei.inc.fdf", \@symbols);
    &genBuildFile(".pei.inc.fdf",\@outputList);
}

################################################
#Sub symbolsMapping                            #
#Function: Generate key words list to parse    #
#          lines from config files             #
################################################

sub symbolsMapping{ 
    open(FILE, "<", $gl_importPath . "Products.config") || die "Error: Cannot open the config file \"Products.config\" \n";
    my @lines = <FILE>;
    close FILE;

    my %symbols;
    my %retMap;
    my @programs;
    my @ret;
    my $comboName = "";

    # Generate Macro Map
    foreach my $line (@lines){
        if ($line =~ /=([^=]+;)*/){
            my @strs = split(/=/, $line);
            my $symbol = $strs[0];
            $symbol =~ s/^\s*|\s*$//g;
            my $programs = $strs[1];
            my @programList = split(/\|/,$programs);
            my $nodeListPtr_;
            if (!exists $gl_MacroMap{$symbol}){
                my @list_;
                $gl_MacroMap{$symbol} = \@list_;
            }
            $nodeListPtr_ = $gl_MacroMap{$symbol};
            foreach my $str(@programList){
                $str =~ s/^\s|\s$//g;
                if (!exists $gl_MacroMap{$str}){
                    my @list_;
                    $gl_MacroMap{$str} = \@list_;
                }
                push(@$nodeListPtr_, $str);
            }
        }
    }

    # Generate key words of combined programs
    foreach my $program(@gl_Program){
        foreach my $platform(@gl_PlatForm){
            my $programName = $platform . $program;
            $programName =~ tr/a-z/A-Z/;
            push(@programs,$programName);
            push(@ret,$programName);
        }
    }

    # Generate key words from Macro Map
    foreach my $node (keys %gl_MacroMap){
        my $symbol_ = $node;
        my $listPtr_ = $gl_MacroMap{$node};
        if (&RecurseMacroMap(\@programs,$node)){
            $retMap{$symbol_} = 1;
        }
    }

    foreach my $key (keys %retMap){
        push(@ret,$key);
	}

    # Generate key word of specific combo bios e.g. AM4SM + AM4BR = *AM4SMBR
    foreach my $program(@gl_Program){
        $comboName = $comboName . $program;
    }
    foreach my $platform(@gl_PlatForm){
        my $key = "*" . $platform . $comboName;
        $key =~ tr/a-z/A-Z/;
        push(@ret,$key);
    }

    return @ret;
}

################################################
#Sub RecurseMacroMap                           #
#Function: Recurse MacroMap to find whether    #
#          current symbol including required   #
#          programs                            #
#Params: 1. List ptr of required programs list #
#        2. Name of current symbol             #
################################################

sub RecurseMacroMap{
    my ($programListPtr_, $key_) = @_;
    my $listPtr_ = $gl_MacroMap{$key_};
    foreach my $name_ (@$listPtr_){
        if (&listExist($programListPtr_,$name_,0)){
            return 1;
        }
        if (&RecurseMacroMap($programListPtr_, $name_)){
            return 1;
        }
    }
    return 0;
}

################################################
#Sub genBuildFile                              #
#Function: Write lines in outputlist into top  #
#          level build files to generate build #
#          files                               #
################################################

sub genBuildFile{
    my ($suffix,$outputListPtr) = @_;
    my @outputList = @$outputListPtr;
    my $outputPath = $gl_outputFileName . $suffix;
    my $lastComment = "";
    my $lastBlankFlag = 0;
    my $firstLine = 1;
    print "Generating build file $outputPath...\n";
    if ($suffix =~ /dsc$/){
        &printCopyRight($outputPath);
        open(FILE,">>","$outputPath") || die "Error: Cannot open the export file \"$outputPath\" \n";
    }
    else{
        open(FILE,">","$outputPath") || die "Error: Cannot open the export file \"$outputPath\" \n";
    }
    foreach my $node (@outputList){
        my $nodeName = $node->{name};
        if ($nodeName ne ""){
            print FILE "\n\[$nodeName\]\n";
            $firstLine = 1;
        }
        my $lineListPtr = $node->{ptr};
        my @lineList = @$lineListPtr;
        my %printedLines;
        foreach my $line (@lineList){
            my $print = $line->{str};
            my $str = $print;
            $str = $1 if ($str =~ /(^.*){/);
            $str =~ s/\s//g;
            if (!exists($printedLines{$str})){
                my $curComment = $line->{comment};
                $printedLines{$str} = 1;
                if ($lastBlankFlag != $line->{blankFlag}){
                    print FILE "\n" if ($firstLine == 0);
                    $lastBlankFlag = $line->{blankFlag};
                }
                if ($curComment ne "" && $curComment ne $lastComment) {
                    print FILE "$curComment\n" if (!($curComment =~ /^\s*#\s*$/ && $lastComment eq ""));
                }
                print FILE "$print\n";
                $firstLine = 0;
                $lastComment = $line->{comment};
            }
        }
    }
    close(FILE);
}

################################################
#Sub parseParam                                #
#Function: Parse param and init global         #
#          variables                           #
################################################

sub parseParam{
#   print "Parsing parameters...\n";
    my (@param) = @_;
    my $valFlag = 0;
    my $variablePtr;
    foreach my $cmd(@param){
        if ($valFlag){
            $$variablePtr = $cmd;
            $valFlag = !$valFlag;
            next;
        }
        if ($cmd eq "-i"){
            $variablePtr = \$gl_importPath;
            $valFlag = !$valFlag;
        }
        elsif ($cmd eq "-e"){
            $variablePtr = \$gl_exportPath;
            $valFlag = !$valFlag;
        }
        elsif ($cmd eq "-o"){
            $variablePtr = \$gl_programs;
            $valFlag = !$valFlag;
        }
        elsif ($cmd eq "-p"){
            $variablePtr = \$gl_platFormNames;
            $valFlag = !$valFlag;
        }
        elsif ($cmd eq "-h"){
            &printHelp();
            exit;
        }
        else{
            die "Error: Unable to resolve the command \"$cmd\"!\nFor any help please use param -h.\n";
        }
    }

#   Formulate import & export path
    $gl_importPath =~ s/\\/\//g;
    my @importPath = split (/\//,$gl_importPath);
    $gl_importPath = "";
    foreach my $dir (@importPath){
        if ($gl_importPath eq "") {
            $gl_importPath = $dir . "/";
        }
        else{
            $gl_importPath = $gl_importPath . $dir . "/";
        }
    }

    $gl_exportPath =~ s/\\/\//g;
    my @exportPath = split (/\//,$gl_exportPath);
    $gl_exportPath = "";
    foreach my $dir (@exportPath){
        if ($gl_exportPath eq "") {
            $gl_exportPath = $dir . "/";
        }
        else{
            $gl_exportPath = $gl_exportPath . $dir . "/";
        }
    }

    @gl_PlatForm = split(/\|/,$gl_platFormNames);
    foreach my $platFormName(@gl_PlatForm){
        $gl_outputFileName = $gl_outputFileName . $platFormName;
    }

    @gl_Program = split(/\|/,$gl_programs);
    foreach my $proName(@gl_Program){
        $gl_outputFileName = $gl_outputFileName . $proName;
    }

    $gl_outputFileName = $gl_exportPath . "Agesa" . $gl_outputFileName . "ModulePkg";
    &printParam();
}

################################################
#Sub parseDir                                  #
#Function: Parse existed files in folder       #
################################################

sub parseDir{
    my $dirpath = $gl_importPath;
    my ($fileType) = @_;
    print "Collecting .$fileType files...\n";
    my @files = glob("$dirpath/*.$fileType");
    my $fileCounts = @files;
    if ($fileCounts == 0){
        die "Error: No .$fileType file found in dir \"$dirpath\"!\n";
    }
    print "Classifying .$fileType files according to symbol \n";
    foreach my $file(@files){
        if ($fileType eq "dsc"){
            &parseDSCFile($file);
        }
        elsif ($fileType eq "fdf"){
            &parseFDFFile($file);
        }
    }
}

################################################
#Sub parseFile                                 #
#Function: Parse lines symboled with required  #
#          key words from config files         #
################################################

sub parseFile{ # param: file path
    my ($filepath,$buildOptListPtr) = @_;
    my @buildOptList = @$buildOptListPtr;
    my $suffix = "";
    if ($filepath =~ /^[^\.]*(\..*)$/){
        $suffix = $1;
    }
    my $node = "";
    my @nodesPtrList; # list to store $nodes
    open(FILE,"<","$filepath") || die"Error: Cannot open the import file \"$filepath\" \n";
    my @linelist = <FILE>;
    my $curStr = "";
    my $comment = "";
    my $commentFlag = 0; #0:end 1:start
    my $blankFlag = 0;
#   parse lines from file
    foreach my $line(@linelist){
        if ($line =~ /^\s*$/){
            $blankFlag = $blankFlag + 1;
            next;
        }
        if ($curStr eq "" && $line =~ /^\s*\[(.*)\]\s*$/){
            $comment = "";
            $node = $1;
            next;
        }
        if ($curStr eq "" && $line =~ /^(\s*#.*)/){
            if ($commentFlag){
                $comment = $comment . "\n" . $1;
            }
            else{
                $comment = $1;
            }
            $commentFlag = 1;
            next;
        }
        $commentFlag = 0;
        if ($line =~ /.*\{\s*$/){ #	solve "{}" block: meet "{"
            $curStr = $line;
            next;
        }
        elsif ($curStr ne ""){	# solve "{}" block: find "}"
            $curStr = $curStr.$line;
            if (!($line =~ /\}/)){
                next;
            }
        }
        else{	# not "{}" block
            $curStr = $line;
        }

        if ($curStr =~ /^([\s\S]+?)\s*<([^<]+)>\s*$/){
            my $str = {
                str => $1, comment => $comment, blankFlag => $blankFlag
            };
            my @buildOpts = split(/\|/,$2);
    #       my $buildOpt = $buildOpts[0];
            foreach my $buildOpt(@buildOpts){
                if(!&listExist(\@buildOptList,$buildOpt)){
                    $curStr = "";
                    next;
                }
                my $nodes = {
                    name => undef, ptr => undef, # node name, ptr of list to store lines
                };
                $nodes->{name} = $node;
                my $foundFlag = 0;
                foreach my $var (@nodesPtrList){
                    if ($var->{name} eq $nodes->{name}){
                        $foundFlag = 1;
                        $nodes = $var;
                    }
                }
                if ($foundFlag == 0){
                    my @list;
                    $nodes->{ptr} = \@list;
                    push(@nodesPtrList,$nodes);
                }
                my $listPtr = $nodes->{ptr};
                push(@$listPtr,$str);
            }
        }
        $curStr = "";
    }
    return @nodesPtrList;
}

################################################
#Sub listExist                                 #
#Function: Parse lines symboled with required  #
#          key words from config files         #
################################################

sub listExist{
    my ($listPtr,$dst,$matchCase) = @_;
    my @list = @$listPtr;
    my $ret = 0;
    foreach my $var (@list){
        if (!$matchCase){
            $var =~ tr/a-z/A-Z/;
            $dst =~ tr/a-z/A-Z/;
        }
        $ret = 1 if ($var eq $dst);
    }
    return $ret;
}

################################################
#Sub printParam                                #
#Function: Print init params                   #
################################################

sub printParam{
    print "Running with parameters:
  ConfigDir:$gl_importPath
  OutputDir:$gl_exportPath
  PlatformName:$gl_platFormNames
  BuildOpt:$gl_programs\n";
}

################################################
#Sub printHelp                                 #
#Function: Print help document                 #
################################################

sub printHelp{
    print "************************************************
-h,          Print help and exit
-o,          Set program names, split by \"|\"
                 e.g. -o \"Sm|Br\"
-p,          Set socket names, split by \"|\"
                 e.g. -p \"Am4\"
Optional param:
-i,          Set config files path
-e,          Set output files path
************************************************";
}

################################################
#Sub printCopyRight                            #
#Function: Write Copy right to build files     #
################################################

sub printCopyRight{
    my ($filepath) = @_;
    open(FILE,">","$filepath") || die "Error: Cannot open the export file \"$filepath\" \n";
    print FILE "#;*****************************************************************************
#;
#; Copyright (C) 2008-2022 Advanced Micro Devices, Inc. All rights reserved.
#;
#;*****************************************************************************
\n";
    close(FILE);
}