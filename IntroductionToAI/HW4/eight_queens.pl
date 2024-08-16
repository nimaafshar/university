member_(List, Element) :- member(Element, List).

domain_list(Length, Domain, List) :-
    length(List, Length),
    maplist(member_(Domain), List).

alldif([]).
alldif([E|Es]) :-
   maplist(dif(E), Es),
   alldif(Es).

all_pairs(L1,L2,Pairs):-
  findall({A,B}, (member(A, L1), member(B, L2)), Pairs).

add_index([],[],[]).

add_index([E|L1],[I|L2],Pairs):-
    add_index(L1,L2,P),
    append([{E,I}],P,Pairs).

diagonal({X_I,I},{X_J,J}):-
    I==J;
    T is X_I - X_J,
    P is I-J,
    Q is J-I,
    (T \== P,T \== Q).

abc(Count,List):-
    aggregate_all(count, (add_index(List,[1,2,3,4,5,6,7,8],X),
    all_pairs(X,X,Pairs),
    member({P,Q},Pairs),diagonal(P,Q)), Count).

% solves n queens.
nq(List):-
    domain_list(8,[1,2,3,4,5,6,7,8],List),
    alldif(List),
    abc(64,List).

p(_,0).
p(A,Count):-
    Count1 is Count - 1,
    p(A,Count1),
    write(A).

print_board([]).

print_board([L|List]):-
    L2 is L-1,
    p('\u25FC ',L2),
    write('\u2655 '),
    L3 is 8-L,
    p('\u25FC ',L3),
    nl,
    print_board(List).

% use this function to print an 8-queens page
print_n_queens():-
    nq(X),print_board(X).