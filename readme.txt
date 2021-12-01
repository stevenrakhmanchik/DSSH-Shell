  ______     ______    ______   ____  ____    ______   __             __   __    
 |_   _ `. .' ____ \ .' ____ \ |_   ||   _| .' ____ \ [  |           [  | [  |   
   | | `. \| (___ \_|| (___ \_|  | |__| |   | (___ \_| | |--.  .---.  | |  | |   
   | |  | | _.____`.  _.____`.   |  __  |    _.____`.  | .-. |/ /__\\ | |  | |   
  _| |_.' /| \____) || \____) | _| |  | |_  | \____) | | | | || \__., | |  | |   
 |______.'  \______.' \______.'|____||____|  \______.'[___]|__]'.__.'[___][___]  
                                                                                 
                                                                                  
Steven Rakhmanchik and Derrick Lin – November 2021 – System Level Programming


breakCommand		Contains if statements to identify which command should be run. Passes errors if command fails for whatever reason.
breakString		Acts as an initial parser identifying whether ; is used and passes some values to breakCommand to run certain built in commands such as exit
showPrompt		Displays command prompt text and funnels it into next function
runShell		Displays login statement with info on system and Kernel.
main			Runs main portion of code and contains loop where the shell runs

BUGS:

	-> ANSI escape codes show up when attempting to use arrow keys

SUCCESSFULLY ADDED:

	-> Successfully implemented colors into the shell
	-> Successfully added error messages and rigorous error checking to ensure stability.
	-> Built-in help command
	-> Implemented the following built-in commands:
	
		help        <- Displays help menu with all built-in commands
		cd          <- Changes directory
		pwd         <- Prints current directory
		exit        <- Exits DSSH shell

