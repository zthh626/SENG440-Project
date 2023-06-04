# Useful SSH URLS

 - seng440.ece.uvic.ca
    - For running ARM
 - ugls.ece.uvic.ca
    - VM for compiling

# Setting up dev env and running

0. SSH into ugls.ece.uvic.ca `ssh NETLINK@ugls.ece.uvic.ca`
1. Use Makefile `make compile-exe`
2. Open another terminal and ssh into seng440.ece.uvic.ca `ssh NETLINK@seng440.ece.uvic.ca`
3. CD to the same location where the the makefile made the executable
4. In seng440.ece.uvic.ca run `lftp user1@arm` cd into am (our folder) -> password on Brightspace
5. Run `put main.exe`
6. Run `chmod +x main.exe`
7. Exit lftp
8. Run `telnet arm` and sign in
9. Go to `/am` again
10. Run `./main.exe`
