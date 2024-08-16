less([], _, []).
less([X|Xs], Y, [X|Zs]) :- 
    X < Y,
    less(Xs, Y, Zs).

less([X|Xs], Y, Zs) :- 
    X >= Y,
    less(Xs, Y, Zs).

more([], _, []).
more([X|Xs], Y, [X|Zs]) :- 
    X >= Y,
    more(Xs, Y, Zs).

more([X|Xs], Y, Zs) :- 
    X < Y,
    more(Xs, Y, Zs).

quick([],[]).
quick([T|Ts],Result):-
    less(Ts,T,Less),
    quick(Less,Less_new),
    more(Ts,T,More),
    quick(More,More_new),
    append(Less_new,[T],R),
    append(R,More_new,Result).
    
