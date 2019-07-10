EasyLogger

THIS LIBRARY WAS WRITTEN BY: JOSÉ LUIS SANZ
AT UNIVERSIDAD POLITÉCNICA DE CARTAGENA.

Just for free use, but not for ussing without atribution.

VER 1.0 IMPLEMENTS:


USAGE:

¬ SUMMARY:

EasyLogger myLogger(folder, name, chipSelect);

myLogger.addln("String of data");	// Adds a line of data.
myLogger.add("String of data");		// Adds data without CR.
getFileName();		// Returns the name of the file.
getFullPath();		// Returns the Full Path to the file.
getFolderName();	// Returns the just the path.


¬ EXPLANATION:

1- CREATING A NEW OBJECT OF EASYLOGGER CLASS:
	#define folder "/myLog/"
	#define name "log.txt"
	#chipSelect 4
	

	EasyLogger myLogger(folder, name, chipSelect);
	
	this means, that the object is inicialised with the folder path, the desired name and the SD on SPI BUS device configured as 4.
	
2- ADDING DATA 

	myLog.add(Just");
	myLog.add(" a ");
	
	this adds just a character or an string of characters, without carriage return or any.
	
	myLog.addln(test);
	
	this just adds a line of data...
	
3- STATISTIC

	myLog.addln( "...




