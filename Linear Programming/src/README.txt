This README explains how to use the included CPLEX (c++) linear program, paperRoll.cpp, and the associate Makefile.

Step 0:
Open Makefile and set the variables CPLEXDIR and CONCERTDIR to the correct directories in your filesystem

Step 1:
Place both paperRoll.cpp and Makefile in your chosen directory and run the following from the command line:
  $ make paperRoll

Step 2:
With your input file in hand, copy the full file path and file name.
Example: "/Google Drive/UPC/Spring 2018/CPS/bwp_10_8_1.in"

Step 3:
The program takes a single input, that is the full path and file name of the input file.
From the command line type the following:
    $ ./paperRoll "paste_file_path_and_name_here"

Step 4:
That's it. The program will run and output a file with the results in the same directory as the input file.
The output file name will be of the format bwp_W_N_K_LP.out. The program will continue to run until it is finished or
it has been manually stopped.

After the program has finished executing, it will show show the final solution in the terminal. 

The format is as follows:
box 1: xtl ytl xbr ybr       
box 2: xtl ytl xbr ybr        
.
.
.
box n: xtl ytl xbr ybr


xtl = top-left x-coordinate
ytl = top-left y-coordinate
xbr = bottom-right x-coordinate
ybr = bottom-right y-coordinate


