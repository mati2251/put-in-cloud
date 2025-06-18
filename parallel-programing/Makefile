hand_over_hand: hand_over_hand.c
	gcc -g -o $@ $<

no_hand_over_hand: no_hand_over_hand.c
	gcc -g -o $@ $<

read_write_lock: read_write_lock.cpp
	g++ -g -o $@ $<

naive_barier: naive_barier.cpp
	g++ -g -o $@ $<

simple_barrier: simple_barrier.cpp
	g++ -g -o $@ $<

clean:
	rm -f hand_over_hand no_hand_over_hand read_write_lock simple_barrier naive_barier
