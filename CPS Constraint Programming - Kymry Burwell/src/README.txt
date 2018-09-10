This README explains how to use the included Gecode constraint program, box.cpp, and the associate Makefile.

Step 1:
Place both box.cpp and Makefile in your chosen directory and run the following from the command line:
  $ make box

Step 2:
With your input file in hand, copy the full file path and file name.
Example: "/Google Drive/UPC/Spring 2018/CPS/bwp_10_8_1.in"

Step 3:
The program takes a single input, that is the full path and file name of the input file.
From the command line type the following:
    $ ./box "paste_file_path_and_name_here"

Step 4:
That's it. The program will run and output a file with the results in the same directory as the input file.
The output file name will be of the format bwp_W_N_K_CP.out. The program will continue to run until it is finished or
it has been manually stopped.

As the programming is running, it will show, in the terminal, the intermediate solutions as it searches the solution space.
The format is as follows:
xtl: 0 0       top-left x-coordinate of the boxes
ytl: 0 1       top-left y-coordinate of the boxes
xbr: 1 1       bottom-right x-coordinate of the boxes
ybr: 0 1       bottom-right y-coordinate of the boxes

In the above example there are two boxes as follows
Box 1: top-left coordinates (0,0), bottom-right coordinates (1,0).
Box 2: top-left coordinates (0,1), bottom-right coordinates (1,1)
