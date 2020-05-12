Brewing is a Linux based program that simulates the five stages of the brewing 
process throughout its creation, termination, and monitoring. User interface
on the Linux terminal provides a recipe to be inputted, along with an overall
inventory. Brewing program will run batches that have enough ingredients in 
inventory, and terminate the program on the first batch that doesn't have
enough ingredients in inventory run the five stages. All records of process
monitoring is then written into activitiesLog.txt in real time. All errors 
that occur during the brewing process are written into errorsLog.txt in real 
time.

Makefile is given to clean the activitiesLog.txt, and errorsLog.txt file using
make clean.
Program can be compiled using the command make
Program can be run using by entering ./brewing into the terminal, where a new
activitiesLog.txt and errorsLog.txt file will be created.

Controls in Brewing Program

q     --> exits out of program

help  --> provides assistance for controls and instructions in program

run   --> runs brewing simulation

input --> prompts input for recipe and inventory  

clear --> clears the activitiesLog.txt and errorsLog.txt
