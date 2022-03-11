��������� ������� Sdi1800065

��� �� compilation:make
��� �������� ��� ������� ��� ���������� �� makefile : make clean

��� �� bash ��� ������ project: ./testFile.sh virusesFile.txt countriesFile.txt 1000 1
��� �� bash ��� �������� project: ./create_infiles.sh inputFile.txt input_dir 4

��� ��� ��������: ./travelMonitor -m numMonitors -b buffersize -s bloomsize -i input_dir


���� �� �������� ��� ������� ��� ���� ���� ��������� �� ������ "Everything is stored." 
������� �� ������ �� ������� ���� ������������ ���� �������� �� /travelRequest citizenID date countryFrom countryTo virusName

��� ������� ��� ����� ��� ���������������� ����� �� ����� �� �� ����� project ��� ��� ���� ���� �� ��� ��������� ��� readme.

���� �������� ��� ����� ����� � Fd_countries ��� �������������� ��� �� ����� ��� ����� ��� ������������� ���� Monitor ����� 
��� � Stat_countries ��� �������������� ��� �� ������� �� requests ��� �� ������ ���� ������ /travelStats virusName date1 date2 [country] ���� ��� ��� log_file���.

�� ��� ������� ����������� ��� ���������������� ����� � :

template  Temp>  
void bufferwrite(int fd,int bufferSize,int sizeofelement,Temp send_element)

��� �

template Temp>
void bufferread(int fd,int bufferSize,Temp temp)

����� �� ��� ����������� ����� ��������� ��� ��� ����� ����������� ������ ��� parent ��� �� ���� ���� ����� �����
�������� ��� ����������� ����������� ��� buffersize �������� ( bufferSize>4 ) ��� ��� ����������������.

���� ���� bufferread ��� children ������� while ���� ��� if ��� �� read ������ ��� ������� ��� �� �������� �� interupts ��� �������������� �� ��������� ������� signal .

��� ��� �������� ��� BloomFilter �� ���� ��� ���������� ���� ������������ ���� ����� �� ���������� �� unsigned int ��� ����������
�� ���� ���� bloom ���� ��������.

H memset ������� �� ���������� ������ ���� ������ ����� �������� ���� ������ ����� ���������� ��� buffer ��� ��������� �� ���������.


���������� �� ������ ������� ��� ��� ������� 1 ���� 2 �������� ���� newmain.cpp � ����� ������ �������� �� parent ���� ��� ���� main.cpp � �����
����� � main ��� ������������ �������� �� ������� ������� ��� ��� ���������� ��� children.

���� ����� � ������ /exit ��� � �������� ������������ ����� �� ����� kill ��� ��� �� ������ � ����� ���� �� ������������ ��� �� ����������� �����
��� �� �������� �� pipe ��� ����� ������� , ��� ��������� ��� ������������ ��� �� ������� ��� ������� ���� ��� ����� ���������� ��� ����� ��� ������
�� pipes (���� ��������� � ������ pause() ��� ��������� �� signal ��� �� parent ��� �� ���������� �� Monitor) ��� ��� ���� ��� ���� ����� ������������ ���� �������� �� errors ���� valgrind.

