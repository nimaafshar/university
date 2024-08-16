parent(abraham,herb).
parent(abraham,homer).
parent(mona,homer).
parent(clancy,marge).
parent(jackie,marge).
parent(clancy,patty).
parent(jackie,patty).
parent(clancy,selma).
parent(jackie,selma).
parent(homer,bart).
parent(marge,bart).
parent(homer,lisa).
parent(marge,lisa).
parent(homer,maggie).
parent(marge,maggie).
parent(selma,ling).


married(abraham,mona).
married(mona,abraham).
married(jackie,clancy).
married(clancy,jackie).
married(marge,homer).
married(homer,marge).

male(abraham).
male(herb).
male(clancy).
male(homer).
male(bart).

female(mona).
female(jackie).
female(marge).
female(patty).
female(selma).
female(lisa).
female(maggie).
female(ling).

daughter(X,Y):-
    parent(X,Y), female(Y).

son(X,Y):-
    parent(X,Y), male(Y).

mother(X,Y):-
    parent(X,Y), female(X).

father(X,Y):-
    parent(X,Y), male(X).

grandfather(X,Y):-
    parent(X,Z),parent(Z,Y),male(X).

grandmother(X,Y):-
    parent(X,Z),parent(Z,Y),female(X).

brother(X,Y):-
    parent(Z,X),parent(Z,Y),male(X),X \== Y.

sister(X,Y):-
    parent(Z,X),parent(Z,Y),female(X), X \== Y.

uncle(X,Y):-
    male(X),brother(X,Z),parent(Z,Y).

aunt(X,Y):-
    female(X),sister(X,Z),parent(Z,Y).

cousin(X,Y):-
    parent(Z,X),parent(W,Y),sister(Z,W);
    parent(Z,X),parent(W,Y),brother(Z,W).

releated(X,Y):-
    parent(X,Y);parent(Y,X);married(X,Y).

releated(X,Y):-
    releated(X,Z),releated(Z,Y), Z \== X, Z\==Y.