 # *****************************************************************************
 # *
 # * Copyright (C) 2016-2022 Advanced Micro Devices, Inc. All rights reserved.
 # *
 # ******************************************************************************
#! /usr/bin/python
import sys
import re
import os
import ConfigParser

class file_class_v2:
    def __init__(self, Path, Name):
        self.Path = Path    #the path of file
        self.Name = Name    #the name of file, the full directory of this file should be os.path.join(Path, Name)
        self.CheckedLines = []  #represent the lines that has be matched before,used to avoid repeat matching
        self.ParamsList = []    #including all the APCB_TOKEN_XXX Tokens(Token)
        self.ParamsSize = 0 #Params Size in the array 'ParameterListxxxValue'(Value)
        self.TokenCon = True    #indicate if APCB_TOKEN_XXX be consistent in both files(Token matched)
        self.SizeCon = True #indicate the lines matched in the array 'ParameterListxxxValue'of both files(Value matched)

    def CollectParamsTokens(self):
        ApcbTokenString = 'APCB_TOKEN_\w+\s*'
        ApcbTokenPattern = re.compile(ApcbTokenString)
        
        ParamsArrayStartString = 'ParameterList\w*Attribute'
        ParamsArrayStartPattern = re.compile(ParamsArrayStartString)
        ParamsArrayEndPattern = re.compile(r'};')
        
        OnProcess = False   #init on-process flag as False
        
        FilePath = os.path.join(self.Path, self.Name)
        try:
            File = open(FilePath,'r')
        except:
            print ("'%s'can not open!" %(FilePath))
            sys.exit(2)
        Lines = File.readlines()    #to read all the lines of the file
        File.close()
        
        for Line in Lines:
            Match = ParamsArrayStartPattern.search(Line)
            if Match:
                OnProcess = True    #if find the array name, we set on-process flag and continue 
                continue
            Match = ParamsArrayEndPattern.match(Line)
            if Match and OnProcess == True:
                OnProcess = False   #if find the array end, we reset on-process flag and break 
                break
                
            if OnProcess:   #during processing
                Match = ApcbTokenPattern.search(Line)   #to match the pattern wherever in one line 
                if Match:
                    self.ParamsList.append(Match.group())   #to add to Params list if matched
        
    def CheckParamsTokensExist(self, FileToCmp):
    
        ParamsArrayStartString = 'ParameterList\w*Attribute'
        ParamsArrayStartPattern = re.compile(ParamsArrayStartString)
        ParamsArrayEndPattern = re.compile(r'};')
        
        self.CollectParamsTokens()  #firstly to collect all the matched Tokens,we need use these tokens as pattern 
                                    #to compare to 'FileToCmp',then to find if all of these tokens also exists.
        try:
            File = open(FileToCmp,'r')
        except:
            print ("'%s'can not open!" %(FileToCmp))
            sys.exit(2)    
        Lines = File.readlines()    #to read all the lines of the file
  
        # to find the array that contains Tokens
        StartLine = 0
        StopLine = 0
        
        CountForLine = 0
        for line in Lines:  
            CountForLine += 1
            if StartLine == 0:
                Match = ParamsArrayStartPattern.search(line)
                if Match:
                    StartLine = CountForLine    
            else:
                Match = ParamsArrayEndPattern.match(line)
                if Match:
                    StopLine = CountForLine   
                    break

        File.close()
        
        for Token in self.ParamsList:  
            ApcbTokenPattern = re.compile(Token)    #to make pattern same as 'Token'
            CountForLine = 0 
            for Line in Lines[StartLine:StopLine]:
                CountForLine += 1
                Match = ApcbTokenPattern.search(Line)   #to match the pattern wherever in one line 
                if Match:
                    if len(Token) != len(Match.group()):     
                        continue    #if the length of matched string differ from the pattern,then skip and continue                  
                    for Index in self.CheckedLines:
                        if Index == CountForLine:
                            break   #if we find the line of matched string already in CheckedLines,skip and continue
                    else:
                        self.CheckedLines.append(CountForLine)  
                        break   #append to CheckedLines if the line never matched before and break as we find the matched line
     
            if CountForLine == (StopLine - StartLine):
                self.TokenCon = False
                print ("Token '%s' not included in '%s'" %(Token, FileToCmp)) 
        
    def CheckParamsValueSize(self):
        ParamsArrayStartString = 'ParameterList\w*Value'    
        ParamsArrayStartPattern = re.compile(ParamsArrayStartString)
        ParamsArrayEndPattern = re.compile(r'};')
        
        ParamsParamsString = '(BSU\d+)|(BSBLN)' #just to match with the value type of 'BSU08/16/32...' and 'BSBLN'
        ParamsParamsPattern = re.compile(ParamsParamsString)
        
        FilePath = os.path.join(self.Path, self.Name)
        try:
            File = open(FilePath,'r')
        except:
            print ("'%s'can not open!" %(FilePath))
            sys.exit(2) 
        Lines = File.readlines()
        File.close()
        
        FindArray = False   #indicate have not found the Params Values array firstly         
        for Line in Lines:
            if FindArray == False:    
                Match = ParamsArrayStartPattern.search(Line)      
                if Match:   #check if the header of Params Values array
                    FindArray = True
            else:
                Match = ParamsArrayEndPattern.match(Line)
                if Match:   #check if the end of Params Values array
                    break
                else:   
                    Match = ParamsParamsPattern.search(Line)
                    if Match:   #check with values type and increase 'ParamsSize'
                        self.ParamsSize += 1   
            
   
def Process_v2(Path1, File1, Path2, File2):
    FileSrc = file_class_v2(Path1, File1)  #creat the file class as source
    FileDes = file_class_v2(Path2, File2)  #creat the file class as destination

    #to compare source and destination files mutually
    FileSrcPath = os.path.join(Path1, File1)    #creat the source file directory
    FileDesPath = os.path.join(Path2, File2)    #creat the destination file directory
    FileSrc.CheckParamsTokensExist(FileDesPath)
    FileDes.CheckParamsTokensExist(FileSrcPath)
    
    #check Parameters values size
    FileSrc.CheckParamsValueSize()
    FileDes.CheckParamsValueSize()

    ErrorBit = 0
    if FileSrc.TokenCon == False or FileDes.TokenCon == False:  #any file.TokenCon flag being cleared shows inconsistency
        print ("APCB v2 configuration Tokens check fail!")
        ErrorBit |= 1    #1 indicate any token mismatched
    if FileSrc.ParamsSize != FileDes.ParamsSize:  #Parameters lines size mismatch
        print ("the size of APCB v2 configuration parameters mismatch in '%s' and '%s'!" %(FileSrcPath, FileDesPath))
        ErrorBit |= 2    #2 indicate any value mismatched
        
    #if ErrorBit == 0:
    #    print ("the APCB Configuration Parameters matched in '%s' and '%s'!" %(FileSrcPath, FileDesPath))
        
    return ErrorBit

class file_class_v3:
    def __init__(self, Path, Name):
        self.Path = Path    #the path of file
        self.Name = Name    #the name of file, the full directory of this file should be os.path.join(Path, Name)
        self.CheckedLines = []  #represent the lines that has be matched before,used to avoid repeat matching
        self.ParamsList = []    #including all the APCB_TOKEN_UID_XXX Tokens(Token)
        self.TokenCon = True    #indicate if APCB_TOKEN_UID_XXX be consistent in both files(Token matched)

    def CollectParamsTokens(self):
        ApcbTokenString = 'APCB_TOKEN_UID_\w+\s*'
        ApcbTokenPattern = re.compile(ApcbTokenString)
        
        ParamsArrayStartString = 'TokeList\[\]'
        ParamsArrayStartPattern = re.compile(ParamsArrayStartString)
        ParamsArrayEndPattern = re.compile(r'};')
        
        OnProcess = False   #init on-process flag as False
        
        FilePath = os.path.join(self.Path, self.Name)
        try:
            File = open(FilePath,'r')
        except:
            print ("'%s'can not open!" %(FilePath))
            sys.exit(2)
        Lines = File.readlines()    #to read all the lines of the file
        File.close()
        
        for Line in Lines:
            Match = ParamsArrayStartPattern.search(Line)
            if Match:
                OnProcess = True    #if find the array name, we set on-process flag and continue 
                continue
            Match = ParamsArrayEndPattern.match(Line)
            if Match and OnProcess == True:
                OnProcess = False   #if find the array end, we reset on-process flag and break 
                break
                
            if OnProcess:   #during processing
                Match = ApcbTokenPattern.search(Line)   #to match the pattern wherever in one line 
                if Match:
                    self.ParamsList.append(Match.group())   #to add to Params list if matched
        
    def CheckParamsTokensExist(self, FileToCmp):
    
        ParamsArrayStartString = 'TokeList\[\]'
        ParamsArrayStartPattern = re.compile(ParamsArrayStartString)
        ParamsArrayEndPattern = re.compile(r'};')
        
        self.CollectParamsTokens()  #firstly to collect all the matched Tokens,we need use these tokens as pattern 
                                    #to compare to 'FileToCmp',then to find if all of these tokens also exists.
        try:
            File = open(FileToCmp,'r')
        except:
            print ("'%s'can not open!" %(FileToCmp))
            sys.exit(2)    
        Lines = File.readlines()    #to read all the lines of the file
  
        # to find the array that contains Tokens
        StartLine = 0
        StopLine = 0
        
        CountForLine = 0
        for line in Lines:  
            CountForLine += 1
            if StartLine == 0:
                Match = ParamsArrayStartPattern.search(line)
                if Match:
                    StartLine = CountForLine    
            else:
                Match = ParamsArrayEndPattern.match(line)
                if Match:
                    StopLine = CountForLine   
                    break

        File.close()
        
        for Token in self.ParamsList:  
            ApcbTokenPattern = re.compile(Token)    #to make pattern same as 'Token'
            CountForLine = 0 
            for Line in Lines[StartLine:StopLine]:
                CountForLine += 1
                Match = ApcbTokenPattern.search(Line)   #to match the pattern wherever in one line 
                if Match:
                    if len(Token) != len(Match.group()):     
                        continue    #if the length of matched string differ from the pattern,then skip and continue                  
                    for Index in self.CheckedLines:
                        if Index == CountForLine:
                            break   #if we find the line of matched string already in CheckedLines,skip and continue
                    else:
                        self.CheckedLines.append(CountForLine)  
                        break   #append to CheckedLines if the line never matched before and break as we find the matched line
     
            if CountForLine == (StopLine - StartLine):
                self.TokenCon = False
                print ("Token '%s' not included in '%s'" %(Token, FileToCmp)) 
    
def Process_v3(Path1, File1, Path2, File2):
    FileSrc = file_class_v3(Path1, File1)  #creat the file class as source
    FileDes = file_class_v3(Path2, File2)  #creat the file class as destination

    #to compare source and destination files mutually
    FileSrcPath = os.path.join(Path1, File1)    #creat the source file directory
    FileDesPath = os.path.join(Path2, File2)    #creat the destination file directory
    FileSrc.CheckParamsTokensExist(FileDesPath)
    FileDes.CheckParamsTokensExist(FileSrcPath)

    ErrorBit = 0
    if FileSrc.TokenCon == False or FileDes.TokenCon == False:  #any file.TokenCon flag being cleared shows inconsistency
        print ("APCB v3 configuration Tokens check fail!")
        ErrorBit |= 1    #1 indicate any token mismatched
  
    return ErrorBit

def Main(Alias):
    #get current path of this script
    Path = os.path.abspath('.')    
    if os.path.isdir(Path):
        CurPath = Path
    elif os.path.isfile(Path):
        CurPath = os.path.dirname(Path) 
    
    #parser 'ApcbParamsCheck.ini'
    Config = ConfigParser.ConfigParser()
    Config.read('ApcbParamsCheck.ini')
    CommonPath = Config.get('path_common', 'path')
    
    #join common configuration path to current path
    ConfigPath = os.path.join(CurPath, CommonPath)
    if os.path.exists(ConfigPath):
        #to get suffix according to project
        Projects = Config.options('project')
        for Project in Projects:
            if Alias.lower() == Project.lower():
                Suffix = Config.get('project', Project)
                break 
        else:
            print("Invalid Parameters!")
            sys.exit(2) 
        #apply multiple configurations according to suffix
        if Suffix: 
            Error = 0
            #multiple paths
            PathSection = "path" + Suffix
            Paths = Config.options(PathSection)
            for PathSrc in Paths:
                for PathDes in Paths[(Paths.index(PathSrc) + 1) : ]:
                    ConfigPathSrc = os.path.join(ConfigPath, Config.get(PathSection, PathSrc))
                    ConfigPathDes = os.path.join(ConfigPath, Config.get(PathSection, PathDes))
                    #multiple files
                    FileSection = "file_v2" + Suffix
                    Files = Config.options(FileSection)
                    for File in Files:
                        #to collect error every checking routine 
                        Error |= Process_v2(ConfigPathSrc, Config.get(FileSection, File), ConfigPathDes, Config.get(FileSection, File))
                    
                    FileSection = "file_v3" + Suffix
                    Files = Config.options(FileSection)
                    for File in Files:
                        #to collect error every checking routine 
                        Error |= Process_v3(ConfigPathSrc, Config.get(FileSection, File), ConfigPathDes, Config.get(FileSection, File))
            else:
                #after all routines completed,we check the result
                if Error != 0:
                    print ("APCB configuration parameters Token/Value pairs mismatched!")
                    sys.exit(2)
        else:
            print("Invalid project alias!")
            sys.exit(2)
    else:
        print("Invalid path!")
        sys.exit(2)
        
if __name__ == "__main__":

    if len(sys.argv) >= 2:  #pass in ''Project' ...
        Main(sys.argv[1])
    else:
        print("Lack of parameters as 'Project'!")
        sys.exit(2)     
    