% A matlab file to read SPI data .txt file produced by Python
% and reduce redundant data in the file to simplify analysis 
% Author C. J. Kikkert, 21 Jul 2025
% Call data file to be read SPIData.txt
% Read a line at a time from a .txt file
% and write a line at a time if required
% 
% Data 4 bit count, 6 SPI inputs 24 bits, with 12 bit Checksum added in LS position
%
% if individual SPI checksums or the group SPI checksum fails, then
% no data is written. So only good data is written

clear
Bit24 = power(2,24);
SPIS=zeros(1,10); %allow up to 10 entries per line Use 7
SPI_Write = fopen('SPI_Data_Split.txt','w');
SPI_Data =fopen('SPIBin.txt','r');
%if (ReadFreq) %if = 1
%	VW=textscan(WaveF,'%f %f %f %f %f'); %for wavedata with freq
RecX = 0;
LMax = 100e6;
CSError = 0; Duplicate = 0; Counter = 0; CounterL = 0; Total = 0;
for I = 1:LMax
    LineR = fgetl(SPI_Data);   %read line string 40 Hex char long
    if (LineR == -1)
        disp('end of file');
        break; 
    else
    %    disp(LineR);
        Total = Total +1;
        CounterB = LineR(1:4); Counter = bin2dec (CounterB);
        if (Counter == CounterL)
            Duplicate = Duplicate +1;
          %  fprintf(SPI_Write, '*');
          %  disp(CounterL);
        else
            Data1B = LineR(5:28); Data1 = bin2dec (Data1B);
            Data2B = LineR(29:52); Data2 = bin2dec (Data2B);
            Data3B = LineR(53:76); Data3 = bin2dec (Data3B);
            Data4B = LineR(77:100); Data4 = bin2dec (Data4B);
            Data5B = LineR(101:124); Data5 = bin2dec (Data5B);
            Data6B = LineR(125:148); Data6 = bin2dec (Data6B);
            ChkSumR = bin2dec(LineR(149:160));
            ChkSumC = Counter;
            ChkSumC = ChkSumC + bin2dec(Data1B(1:12)) + bin2dec(Data1B(13:24));
            ChkSumC = ChkSumC + bin2dec(Data2B(1:12)) + bin2dec(Data2B(13:24));
            ChkSumC = ChkSumC + bin2dec(Data3B(1:12)) + bin2dec(Data3B(13:24));
            ChkSumC = ChkSumC + bin2dec(Data4B(1:12)) + bin2dec(Data4B(13:24));
            ChkSumC = ChkSumC + bin2dec(Data5B(1:12)) + bin2dec(Data5B(13:24));
            ChkSumC = ChkSumC + bin2dec(Data6B(1:12)) + bin2dec(Data6B(13:24));
            ChkSumC = mod(ChkSumC, 4096); % ignore overflow
            if (ChkSumC == ChkSumR)
                fprintf(SPI_Write,'%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i \r\n',Counter, Data1, Data2, Data3, Data4, Data5, Data6, ChkSumR, ChkSumC );
            else
                CSError = CSError + 1;
                fprintf(SPI_Write,'%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i\t%i CSERROR\r\n',Counter, Data1, Data2, Data3, Data4, Data5, Data6, ChkSumR, ChkSumC );
%            fprintf(SPI_Write, 'CS Error %i\r\n', CSError);
            end
        end
        CounterL = Counter; % for check duplicate
    end
end
fprintf(SPI_Write, 'Total %i, CS Error %i, Duplicate %i \r\n', Total, CSError, Duplicate);
fclose(SPI_Data);
fclose(SPI_Write);
disp('end');

%Typical input
%
% #Digilent WaveForms Logic Analyzer Acquisition
% #Device Name: Discovery3
% #Serial Number: SN:210415B9FDDD
% #Date Time: 2025-03-05 15:48:44.807.213.070
% #Sample rate: 2e+06Hz
% #Samples: 10000000
%
% Time (s)	SPI	SPI2	SPI3	SPI4	SPI5	SPI6	SPI7
% -2.499952	X	X	X	X	X	X	X
% -2.4999515	X	X	X	X	X	X	X
% -2.499951	3000172677	2258796712	3612549708	2258796712	427094912	1609076180	281584250
% -2.4999505	3000172677	2258796712	3612549708	2258796712	427094912	1609076180	281584250
% -2.49995	3000172677	2258796712	3612549708	2258796712	427094912	1609076180	281584250
% -2.4999495	3000172677	2258796712	3612549708	2258796712	427094912	1609076180	281584250
% -2.499949	3000172677	2258796712	3612549708	2258796712	427094912	1609076180	281584250
% Typically 10 Million lines
% SPI1 to SPI6 are data and SPI7 is the checksum These are 32 bit data
% including an 8 bit checksum

% Typical outut data for same file
% #Digilent WaveForms Logic Analyzer Acquisition
% #Device Name: Discovery3
% #Serial Number: SN:210415B9FDDD
% #Date Time: 2025-03-05 15:48:44.807.213.070
% #Sample rate: 2e+06Hz
% #Samples: 10000000
% Time (s)	SPI	SPI2	SPI3	SPI4	SPI5	SPI6	SPI7
% -2.499951	11719424	8823424	14111522	8823424	1668339	6285453
% -2.499851	11719552	8823296	14111904	8823296	1668733	6285187
% -2.499751	11719680	8823169	14112285	8823169	1669126	6284920
% -2.499651	11719808	8823040	14112666	8823040	1669520	6284654
% -2.499551	11719936	8822913	14113047	8822913	1669914	6284387
% Typically <1% of the lines for the above file.
