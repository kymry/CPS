
sol: sol.o
	$(CCC) $(CCFLAGS) sol.o -o sol $(CCLNFLAGS)
sol.o: sol.cc
	$(CCC) -c $(CCFLAGS) sol.cc -o sol.o

clean:
	rm --force sol sol.o

################################

# Uncomment for CPLEX_Enterprise_Server126
# SYSTEM     = x86-64_linux
# LIBFORMAT  = static_pic
# CPLEXDIR      = /opt/ibm/ILOG/CPLEX_Enterprise_Server126/CPLEX_Studio/cplex
# CONCERTDIR    = /opt/ibm/ILOG/CPLEX_Enterprise_Server126/CPLEX_Studio/concert

# Uncomment for CPLEX_Studio124
SYSTEM     = x86-64_osx
LIBFORMAT  = static_pic
CPLEXDIR      = /Users/kymryburwell/Applications/IBM/ILOG/CPLEX_Studio1271/cplex
CONCERTDIR    = /Users/kymryburwell/Applications/IBM/ILOG/CPLEX_Studio1271/concert

################

CCC = g++ -std=c++11

CCOPT = -m64 -O3 -fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD

CPLEXBINDIR   = $(CPLEXDIR)/bin/$(BINDIST)
CPLEXLIBDIR   = $(CPLEXDIR)/lib/$(SYSTEM)/$(LIBFORMAT)
CONCERTLIBDIR = $(CONCERTDIR)/lib/$(SYSTEM)/$(LIBFORMAT)

CCLNFLAGS = -L$(CPLEXLIBDIR) -lilocplex -lcplex -L$(CONCERTLIBDIR) -lconcert -lm -pthread

CONCERTINCDIR = $(CONCERTDIR)/include
CPLEXINCDIR   = $(CPLEXDIR)/include

CCFLAGS = $(CCOPT) -I$(CPLEXINCDIR) -I$(CONCERTINCDIR)
