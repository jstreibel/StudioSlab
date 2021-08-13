set terminal dumb
plot 'data.txt' using 1:2 with lines, \
     'data.txt' using 1:3 with lines, \
     'data.txt' using 1:4 with lines
