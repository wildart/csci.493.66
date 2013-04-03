clean:
	rm *.o
	find . -executable -type f -not -iname "*.*" -delete

helloncurses:
	gcc -o helloncurses helloncurses.c -lncurses

drawpattern:
	gcc -o drawpattern drawpattern.c -lncurses

drawgrid:
	gcc -o drawgrid drawgrid.c -lncurses

cursortrack:
	gcc -o cursortrack cursortrack.c -lncurses
	
	
