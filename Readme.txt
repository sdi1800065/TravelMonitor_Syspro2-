The purpose of this paper is to familiarize you with creating processes using system calls fork / exec, process communication through pipes, low-level use I / O and creating bash scripts.
As part of this work you will implement a distributed travelMonitor tool that will
accepts requests from citizens who want to travel to other countries, will check if they have done it
appropriate vaccination, and will approve whether a traveler is allowed to enter a country.
Specifically, you will implement the travelMonitor application which will create a number of
monitor processes that, together with the application, will answer user questions. 


A) The travelMonitor application (90%)
The travelMonitor application will be used as follows:
./travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir
where:
- The numMonitors parameter is the Monitor number of processes that the application will create.
- The bufferSize parameter: is the size of the buffer for reading over pipes.
The sizeOfBloom parameter specifies the size of bloom filters in * bytes *. Indicative size
of bloom filter for exercise data will be in the order of 100Kbytes. This parameter
is of the same logic as the corresponding parameter of the first work.
The input_dir parameter: is a directory that contains subdirectories with the files that will
are processed by Monitor processes. Each subdirectory will have the name of a country and will contain
one or more files. For example, input_dir could contain subdirectories
China / Italy / and Germany / which have the following files:
-
/input_dir/China/China-1.txt
/input_dir/China/China-2.txt
/input_dir/China/China-3.txt
…
/input_dir/Italy/Italy-1.txt
/input_dir/Italy/Italy-2.txt
…
/input_dir/France/France-1.txt
/input_dir/France/France-2.txt
/input_dir/France/France-3.txt
…
Each file contains a series of citizen records where each line describes it
vaccination status of a citizen for a specific virus. For example, if the
contents of the /input_dir/France/France-1.txt .txt file are:

889 John Papadopoulos France 52 COVID-19 YES 27-12-2020
889 John Papadopoulos France 52 H1N1 NO
776 Maria Tortellini France 36 SARS-1 NO
125 Jon Dupont France 76 H1N1 YES 30-10-2020

means that in France we have a citizen (John Papadopoulos) who has been vaccinated for COVID-19 on
27-12-2020 but not for H1N1, Maria Tortellini who has not yet been vaccinated for SARS-1, and
Jon Dupont who was vaccinated for H1N1. Corresponding to the first exercise if there are lines
which is contradicting with previous ones, you can discard records that create inconsistency
in the same way as in the 1st exercise.

Specifically, a record is an ASCII line of text that consists of the following elements with it
the line:
- citizenID: a string (it can have only one digit) that defines each such
sign up.
- firstName: a string consisting of letters without spaces.
- lastName: a string consisting of letters without spaces.
- age: a positive (> 0), integer <= 120.
- virusName: a string consisting of letters, numbers, and possibly a
hyphen “-” but without spaces.
- YES or NO: indicates whether the citizen has been vaccinated against the virus.
- dateVaccinated: date the citizen was vaccinated. If the previous field is NO, no
there is a dateVaccinated field in the record.

To get started, the travelMonitor application should create a set of named pipes for
communication with numMonitors child processes that will create. Next, you need to fork
numMonitors child processes. Each child process will call the exec with an executable file of a program
called Monitor that you will write and that will take as arguments the paths of the named pipes that
will be used for communication between the original parent process travelMonitor and each exec’d
Monitor process. Then, the parent process will inform each Monitor via named pipe about the
subdirectories to be undertaken by the Monitor. TravelMonitor will distribute evenly (with round-robin
alphabetically) the subdirectories with the countries located in input_dir in Monitor processes. You can
assume that subdirectories will be flat, that is, they will only contain files, not subdirectories.

When the application (parent process) finishes the initialization steps, it will wait for a series of
bloom filters from Monitor processes (see below) and when it receives all the information, it will be ready
accept input (commands) from the user from the keyboard (below for commands).
Each Monitor process, for each directory assigned to it, will read all its files and fill them out
a set of data structures that he will use to answer questions posed by
parent process. One structure will be the bloom filter which, as in the first work, will be used for
a quick check to see if a citizen (with a citizenID ID) has been vaccinated for this
virus. Each Monitor process, after it has finished reading the input files, will send via named
pipe in the parent process a bloom filter for each virus that will represent the total of the vaccinated
citizens of the countries managed by the Monitor process. The choice of the number of named pipes between
in the parent process and Monitor processes, as well as other data structures is yours
design choice. If while reading files, a Monitor process detects a problem
registration (eg in syntax or an inconsistent record), will ignore the problematic record.
When the Monitor process finishes reading its files and has sent all Bloom filters to
parent, notifies the parent process via named pipe that the Monitor is ready to accept requests.

If a Monitor process receives a SIGINT or SIGQUIT signal then it prints to a file named log_file.xxx
(where xx is its process ID) the name of the countries (subdirectories) it manages, the total
the number of requests received to enter the countries it manages, and the total number of requests
approved and rejected.

Logfile format:
Italy
China
Germany
TOTAL TRAVEL REQUESTS 29150
ACCEPTED 25663
REJECTED 3487

If a Monitor process receives a SIGUSR1 signal, it means that 1 or more new ones have been installed
files in one of the subdirectories assigned to it. We assume that they are not created
new directories with countries and how existing files are not modified. That is, they will exist
only new files. The monitor process will check the subdirectories to find the new files
reads and updates the data structures it holds in memory. After completing the reading
of new files, will send the updated bloom filters to the parent process.
If a Monitor process terminates abruptly, the parent process will have to fork a new Monitor process
that will replace it. Therefore, the parent process should handle the SIGCHLD signal, such as
also SIGINT and SIGQUIT.

If the parent process receives SIGINT or SIGQUIT, it must first finish editing the current
command by the user and after responding to the user, will send a SIGKILL signal to the Monitors, will
is waiting for them to terminate, and will eventually print to a file named log_file.xxxx where xxx
is its process ID, the name of all countries (subdirectories) that participated in the application with
data, the total number of requests received to enter the countries, and the total number
requests approved and rejected.

Logfile format:
Italy
China
Germany
TOTAL TRAVEL REQUESTS 883832
ACCEPTED 881818
REJECTED 2014

The user will be able to give the following commands to the application:
● / travelRequest citizenID date countryFrom countryTo virusName
The application will first check the bloom filter sent to it by the Monitor process that manages it
countryFrom country. If the bloom filter indicates that the citizenID citizen has not been vaccinated
against virusName prints
REQUEST REJECTED - YOU ARE NOT VACCINATED
If the bloom filter indicates that the citizenID citizen may have been vaccinated against the virusName, the
application requests via named pipe from the Monitor process that manages the countryFrom country if
citizenID has indeed been vaccinated. The Monitor process responds via named pipe YES / NO where in
In the case of YES, it also sends the date of vaccination. For the answer the Monitor process can
(but it is not necessary) to use the structures you had in task 1 (eg the skip list). THE
application checks if the citizen has been vaccinated less than 6 months before the desired date
travel date and prints one of the following messages
REQUEST REJECTED - YOU ARE NOT VACCINATED
REQUEST REJECTED - YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE
REQUEST ACCEPTED - HAPPY TRAVELS
● / travelStats virusName date1 date2 [country]
If no country argument is given, the application will print the number of citizens who have requested approval to
travel through space [date1 ... date2] to countries they control for vaccination against
virusName, and the number of citizens approved and rejected. If given country
argument, the application will print the same information but only for that country. The
date1 date2 arguments will be in DD-MM-YYYY format.
Output format: Example:
TOTAL REQUESTS 29150
ACCEPTED 25663
REJECTED 3487
● / addVaccinationRecords country
With this request the user has placed in input_dir / country one or more files for
edited by Monitor process. The parent process sends a notification via SIGUSR1 signal to
Monitor process that manages the country that there are input files to read in
directory. The Monitor process reads whatever new file it finds, updates the data structures and sends
back to the parent process, via named pipe, the updated bloom filters that represent the
group of vaccinated citizens
● / searchVaccinationStatus citizenID
The parent process forwards to all Monitor processes the request through named pipes. The Monitor process
which manages the citizen with a citizenID ID sends through named pipe whatever information
has for the vaccinations that the specific citizen has done / has not done. When he receives them
information, the parent prints it to stdout.
Output format: Example:
889 JOHN PAPADOPOULOS GREECE
AGE 52
COVID-19 VACCINATED ON 27-12-2020
H1N1 NOT YET VACCINATED
●  / exit
Exit the application. The parent process sends a SIGKILL signal to the Monitors, waiting for them to
terminate, and print to a file named log_file.xxxx where xxx is its process ID, the
name of all countries (subdirectories) that participated in the application with data, the total
the number of applications accepted for entry into the countries, and the total number of applications approved
and rejected. Before it terminates, it will properly release all the free memory.

Log file format (as above):
Italy
China
Germany
TOTAL TRAVEL REQUESTS 883832
ACCEPTED 881818
REJECTED 2014

The communication between the parent process and each Monitor takes place via named pipes (see image).
The default behavior of named pipes is to put the process into standby mode
opens one end until the piping opens at the other end. Of course, we can avoid it
the above behavior if we set the O_NONBLOCK flag to the second argument of the system call
open (). For example, if we want to open a named pipe for reading without entering
waiting, we make the call open (pipe_name, O_RDONLY | O_NONBLOCK). You are free to choose
which method of operation of the piping you want.

Tips & Warnings The most difficult part of the job is managing named pipes. It would be good as well
you plan the work to think about issues such as:
1) What happens when the -b bufferSize parameter is much smaller than the data one wants
process to send over a named pipe?
2) What happens when the Monitor receives a SIGUSR1 signal while in the middle of sending data to
in the parent process?
3) How to design the parent process so that it does not block waiting for a slow Monitor while
is there another Monitor that has sent data and is waiting to be read in the named pipe? (Maybe
you find the select () call useful.)
4) How will a Monitor be informed that the parent process intends to transmit an order to him through
of the named pipe so that the second one can go and read it? (This, for example, could
achieved by sending a pre-agreed signal (signal-software interrupt) or via
use select () call or by using the named_pipe you will design).
5) How will a process know how many bytes are a message from another process? That is, how will
interprets the bytes sent to it by the other process? (You will need to design a “protocol
communication ”between processes).
Whatever design choices you make, you should describe them in a README file to submit
along with your password.


B) The script create_infiles.sh (10%)
You will write a bash script that creates test subdirectories and input files that you will use
for debugging your program. Of course, during the development of your program
you can use small files to debug. The script create_infiles.sh works
as follows:
./create_infiles.sh inputFile input_dir numFilesPerDirectory
- inputFile: a file that has the same format as its citizenRecordsFile input file
first job. Recall the sample example we gave:

889 John Papadopoulos Greece 52 COVID-19 YES 27-12-2020
889 John Papadopoulos Greece 52 Η1Ν1 ΝΟ
776 Maria Tortellini Italy 36 SARS-1 NO
125 Jon Dupont USA 76 H1N1 YES 30-10-2020

- input_dir: the name of a directory where the subdirectories and input files will be placed
- numFilesPerDirectory: the number of files to be created in each subdirectory

The script does the following:
1. Checks for input numbers
2. Creates a directory with the name given in the second input_dir argument. If the list
there is an error message printed and terminated.
3. Reads the inputFile file
4. In the input_dir directory creates subdirectories, one for each country name
locates in the inputFile
In each subdirectory, it creates numFilesPerDirectory files named country-n.txt
where n is a positive integer. As the inputFile is processed it will detect them all
the lines that correspond to a country and will distribute the round robin lines to the files
named country-n.txt

