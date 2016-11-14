rm matrix.db a.out
gcc -pg none_core.c 
./a.out 100
gprof ./a.out | gprof2dot.py |dot -Tpng -o report.png
