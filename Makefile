clean:
	rm -f *.o
	find . -executable -type f -not -iname "*.*" -delete

helloncurses:
	gcc -o helloncurses helloncurses.c -lncurses

drawpattern:
	gcc -o drawpattern drawpattern.c -lncurses

drawgrid:
	gcc -o drawgrid drawgrid.c -lncurses

cursortrack:
	gcc -o cursortrack cursortrack.c -lncurses

drawmultigrid:
	gcc -o drawmultigrid drawmultigrid.c -lncurses

bouncestr:
	gcc -o bouncestr bouncestr.c -lncurses

bouncestr_async:
	gcc -o bouncestr_async bouncestr_async.c -lncurses

bouncestr2:
	gcc -o bouncestr2 bouncestr2.c -lncurses

bouncestr_aio:
	gcc -o bouncestr_aio bouncestr_aio.c -lncurses -lrt

upcase_client:
	gcc -I. upcaseclient1.c -o upcaseclient1

upcase_server:
	gcc -I. upcased1.c -o upcased1
