\echo '\n Ajoute dans la table Maitrise, pour le dernier artiste un Instrument InstruBidon'

INSERT INTO maitrise (id_artiste, nom_instrument)
VALUES ((SELECT COUNT(*) FROM artiste), 'InstruBidon');

SELECT * FROM maitrise;

SELECT * FROM instrument;

