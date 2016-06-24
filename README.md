###Renaming
If the plot is incomplete or missing a few nonces, the program is able to rename the plot and correct the file. Allowing space to be freed by truncating.

###Truncating
As a result of renaming the plots, the plot needs to be truncated to match the stagger. By removing any excess nonces not used by the plot anymore, you get valuable space back otherwise wasted.

###How it manages to do it
Gets the plotfile’s size.
Calculating the amount of real nonces, by dividing the plotfile’s size by 256 kb (1 nonce).
Renaming the plotfile to the number of real nonces, while still being a multiplier of the stagger.
Removing any unused nonces.
