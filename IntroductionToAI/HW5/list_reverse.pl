# this writes the list in reverse order.
p([]).

p([A|B]):-
    p(B),
    write(A),nl.
