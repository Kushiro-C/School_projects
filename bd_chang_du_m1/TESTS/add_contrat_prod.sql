\echo '\n Ajout contrat producteur_artiste via fonction (2019-10-20, 2020-10-20, 5000, 1, 1, 5, 2)'

SELECT create_contrat_prod('2019-10-20', '2020-10-20', 5001, 1, 1, 5, 4);

\echo '\n Faites un SELECT * FROM paiement, contrat ou contrat_artiste_producteur Pour voir l\'ajout'
