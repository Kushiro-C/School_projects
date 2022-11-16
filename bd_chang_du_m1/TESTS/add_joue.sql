\echo '\n Ajoute dans la table Joue, pour le dernier artiste un StyleDeMusique StyleBidon'

INSERT INTO joue (id_artiste, nom_styleDeMusique)
VALUES ((SELECT COUNT(*) FROM artiste), 'StyleBidon');

SELECT * FROM joue;

SELECT * FROM StyleDeMusique;

