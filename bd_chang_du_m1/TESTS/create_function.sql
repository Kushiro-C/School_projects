\echo '\nNombre de personne : '
SELECT MAX(id_personne) FROM personne;

\echo '\nAjout d\'un nouveau artiste + personne'

select create_artiste('Jean', 'Pierre', '2002-02-02');

\echo '\nVerification du nouveau artiste + personne'
SELECT * FROM Artiste NATURAL JOIN Personne where nom_personne = 'Jean' AND prenom_personne = 'Pierre';

\echo '\nAjout d\'un nouveau agent + personne'

select create_agent('Maxime', 'Pierre', '2003-01-01', 1);

\echo '\nVerification du nouveau agent + personne'
SELECT * FROM Agent NATURAL JOIN Personne where nom_personne = 'Maxime' AND prenom_personne = 'Pierre';

\echo '\nAjout d\'un nouveau producteur + personne'

select create_producteur('Titou', 'Pierre', '2002-02-02');

\echo '\nVerification du nouveau producteur + personne'
SELECT * FROM Producteur NATURAL JOIN Personne where nom_personne = 'Titou' AND prenom_personne = 'Pierre';

\echo '\nAjout d\'un nouveau producteur + artiste + personne'

select create_producteur_artiste('Fifi', 'Pierre', '2001-01-02');

\echo '\nVerification du nouveau producteur + artiste + personne'
SELECT * FROM Artiste NATURAL JOIN Personne NATURAL JOIN Producteur where nom_personne = 'Fifi' AND prenom_personne = 'Pierre';