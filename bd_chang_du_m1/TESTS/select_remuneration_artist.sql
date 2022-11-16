\echo '\nRappel: Un Artiste est taxé un pourcentage par contrat_agence, et gagne un pourcentage bonus par contrat_producteur'


\echo '\nRémunération de chaque artiste :'

SELECT *, remuneration_artiste(a.id_artiste)
FROM artiste a;
