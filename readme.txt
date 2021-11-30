  ______     ______    ______   ____  ____    ______   __             __   __    
 |_   _ `. .' ____ \ .' ____ \ |_   ||   _| .' ____ \ [  |           [  | [  |   
   | | `. \| (___ \_|| (___ \_|  | |__| |   | (___ \_| | |--.  .---.  | |  | |   
   | |  | | _.____`.  _.____`.   |  __  |    _.____`.  | .-. |/ /__\\ | |  | |   
  _| |_.' /| \____) || \____) | _| |  | |_  | \____) | | | | || \__., | |  | |   
 |______.'  \______.' \______.'|____||____|  \______.'[___]|__]'.__.'[___][___]  
                                                                                 
                                                                                  
Steven Rakhmanchik and Derrick Lin – November 2021 – System Level Programming


removeWhite	<- Function that removes excess whitespace from end of command
readLine	<- Calls removeWhite function and then takes in stdin input
parser		<- parses input from command line
checkRedir	<- checks if redirection takes place and returns true or false as 0 or 1
checkPipe	<- similar to checkRedir but works with pipes
parseRedir	<- parses command for redirection
parsePipe	<- parses command for pipe
runChild	<- runs command as a child of the main process
writeFromFile	<- takes input from file for redirection
writeToFile	<- sends output to file from redirection
appendToFile	<- adds output to a file from redirection
runPipe		<- runs a command and uses its output for another command
histAdd		<- adds to history log
histGet		<- gets history from log
runCommand	<- runs a command
cdCommand	<- built in function for changing current directory
shellHelp	<- help menu with info on built-in commands (these commands do not support redirecting)
shellExit	<- called by exit command: exits the shell
shellHistory	<- keeps track of entire history of the commands run by the shell
shellRedirect	<- calls write and appoint functions after figuring out which redirection symbol was used
shellPipe	<- runs parsePipe and runPipe commands as helper functions to set up a pipe
main		<- contains main shell loop that displays the prompt

BUGS:

-> ANSI escape codes show up when attempting to use arrow keys
-> In some environments, such as Hyper Terminal on Mac, the backspace key doesn't work and also leaves an escape code in the input. Backspace does work on the regular terminal app on Mac, but arrow keys still do not.

Attempted to implement:

	-> Attempted to add multiple command support with the ; operator but was unable to because of 	the argument limit I set previously to avoid memory issues.

Successfully added:

	-> Successfully implemented colors into the shell
	-> Successfully added meaningful and helpful error messages to the shell
	-> Successfully added built-in help command
	-> Successfully added built-in "!!" command to run the last attempted command.
	-> Implemented the following built-in commands (don't support piping or redirection):
	
	help        <- Displays help menu with all built-in commands
	cd          <- Changes directory
	listdir     <- Prints files in current directory
	rmv         <- Removes directory, or file
	exrmv       <- Removes all files except those specified
	exit        <- Exits DSSH shell

