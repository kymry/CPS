This README explains how to use the included c++ SAT program, paperSAT.cpp, and the associate Makefile.
It is configured for the OSX operating system. The system calls on lines 324 - 328 may need to be altered to run on Linux.

Step 1:
Place paperSAT.cpp, Makefile, and the Lingeling executable in your chosen directory and run the following from the command line:
  $ make paperSAT

Step 2:
With your input file in hand, copy the full file path and file name.
Example: "/Google Drive/UPC/Spring 2018/CPS/bwp_10_8_1.in"

Step 3:
The program takes a single input, that is the full path and file name of the input file.
From the command line type the following:
    $ ./paperSAT "paste_file_path_and_name_here"

Step 4:
That's it. The program will run and output a file with the results in the same directory as the input file.
The output file name will be of the format bwp_W_N_K_SAT.out. The program will continue to run until it is has found an optimal solution or
it has been manually stopped.

After the program has finished executing, it will show show the final solution in the output file.

The format is as follows:
------------------------------------------
length of roll
box 1: xtl ytl xbr ybr
box 2: xtl ytl xbr ybr
.
.
.
box n: xtl ytl xbr ybr
------------------------------------------

xtl = top-left x-coordinate
ytl = top-left y-coordinate
xbr = bottom-right x-coordinate
ybr = bottom-right y-coordinate
