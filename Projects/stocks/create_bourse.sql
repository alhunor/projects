DROP database bourse;
CREATE database bourse;
use bourse;

create table historique(
action varchar(30),
cours float,
variation float,
ouverture float,
plushaut float,
plusbas float,
volume int,
date date,
time time
);