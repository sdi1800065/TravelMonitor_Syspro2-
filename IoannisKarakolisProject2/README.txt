Καρακολης Ιωαννης Sdi1800065

Για το compilation:make
Για διαγραφη των αρχειων που δημιουργει το makefile : make clean

Για το bash του πρωτου project: ./testFile.sh virusesFile.txt countriesFile.txt 1000 1
Για το bash του δευτερου project: ./create_infiles.sh inputFile.txt input_dir 4

Για την εκτελεση: ./travelMonitor -m numMonitors -b buffersize -s bloomsize -i input_dir


Μετα το διαβασμα των αρχειων και αφου εχει εκτυπωθει το μυνημα "Everything is stored." 
μπορουν να δωθουν οι εντολες οπως αναφαιρονται στην εκφωνηση πχ /travelRequest citizenID date countryFrom countryTo virusName

Όσο αναφορά τις δομές που χρησιμοποιήθηκαν είναι οι ίδιες με το πρώτο project για τον λόγο αυτό θα τις παραλείψω στο readme.

Κάτι επιπλέον από δομές είναι η Fd_countries που χρησιμοποιείτε για να κρατά τις χώρες που διαχειρίζεται κάθε Monitor καθώς 
και η Stat_countries που χρησιμοποιείτε για να κρατάει τα requests και να απαντά στην εντολή /travelStats virusName date1 date2 [country] αλλα και στο log_fileχχχ.

Οι δυο βασικές συναρτήσεις που χρησιμοποιήθηκαν είναι η :

template  Temp>  
void bufferwrite(int fd,int bufferSize,int sizeofelement,Temp send_element)

και η

template Temp>
void bufferread(int fd,int bufferSize,Temp temp)

Αυτές οι δυο συναρτήσεις είναι υπεύθυνες για την σωστή επικοινωνία μεταξύ του parent και το κάθε φορά παιδί καθώς
στέλνουν τις απαραίτητες πληροφορίες ανά buffersize κομμάτια ( bufferSize>4 ) και τις ανακατασκευάσουν.

Μέσα στην bufferread των children υπάρχει while αντί για if για τα read πράγμα που γίνεται για να αποφεύγω τα interupts που δημιουργούνται σε περίπτωση κάποιου signal .

Για την αποστολή των BloomFilter το μόνο που χρειάζεται στην συγκεκριμένη δομή είναι να αποσταλούν τα unsigned int που απαρτίζουν
το κάθε φορά bloom προς αποστολή.

H memset υπάρχει σε υπερβολικό σημείο στον κώδικα καθώς αποτελεί έναν εύκολο τρόπο καθαρισμού του buffer και αποτρέπει τα σκουπίδια.


Γενικότερα οι κυρίως αλλαγές από την εργασία 1 στην 2 υπάρχουν στην newmain.cpp η οποία κυρίως υλοποιεί το parent αλλά και στην main.cpp η οποία
είναι η main της προηγούμενης εργασίας με αρκετές αλλαγές για την υλοποιήσει των children.

Όταν δοθεί η εντολή /exit ενώ η κανονική αντιμετώπιση είναι να γίνει kill για όλα τα παιδιά η μνήμη τους να αποδεσμευθεί από το λειτουργικό καθώς
και να κλείσουν τα pipe που έχουν ανοιχτά , έχω προσθέσει μια προετοιμασία για το σκότωμα των παιδιών κατά την οποία αποδεσμεύω την μνήμη και κλείνω
τα pipes (μετα υπαραρχει η εντολη pause() και περιμενει το signal απο το parent για να τερματισει το Monitor) για τον λόγο ότι στην πρώτη αντιμετώπιση είχα πρόβλημα με errors στον valgrind.

