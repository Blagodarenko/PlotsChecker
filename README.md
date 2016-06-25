###PlotsChecker
PlotsChecker checks and validates plots to see if the name of the plots, matches with the contents of the plot. Incase it finds any errors, the program is able to correct the plot by renaming and truncating the plot, thus removing any space otherwise wasted.

###Renaming
If the plot is incomplete or missing nonces, the program is able to rename the plot and correct the file, which allows space to be freed by truncating. When the plot gets renamed, the program makes sure the nonces is still a multiplier of the stagger size.

###Truncating
As a result of renaming the plots, there is now nonces wasting space instead of being usefull. By truncating the program is able to remove any nonces not within the new plot

###Flow chart
The program gets the plotfile’s size.
The program calculated the amount of real nonces contained within the plot file, by dividing the plotfiles’s size by 256 kb (1 nonce).
Renames the plotfile to the number of real nonces, while still being a multiplier of the stagger.
Truncates the plot, by deleting any unused nonces.
