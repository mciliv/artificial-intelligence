% Morgan Ciliv
% serenity.pl
% Purpose: Translates the given sentence into a Prolog program.
%________________________________________________________________________________


ship(serenity).

captain(mal,serenity).

crewmember(zoe, serenity).
crewmember(wash, serenity).
crewmember(kaylee, serenity).

passenger(simon, serenity).
passenger(river, serenity).

married(zoe, wash).

serves(Z, Y) :- ship(X), captain(Y, X), crewmember(Z, X).

protects(Y, A) :- ship(X), captain(Y, X), passenger(A, X).

friends(kaylee, A) :- passenger(A, serenity).
friends(mal, zoe).


%________________________________________________________________________________
%
% Output after querying the database:
%________________________________________________________________________________

% ?- findall([X, Y], friends(X, Y), RESULTS).
% RESULTS = [[zoe, mal], [wash, mal], [kaylee, mal]].
%
% ?- findall([X, Y], friends(X, Y), RESULTS).
% RESULTS = [[kaylee, simon], [kaylee, river], [mal, zoe]].
%
% ?- findall([X, Y], married(X, Y), RESULTS).
% RESULTS = [[zoe, wash]].

