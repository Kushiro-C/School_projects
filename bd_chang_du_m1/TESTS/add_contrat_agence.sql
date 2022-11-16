\echo '\n Ajout contrat agence_artiste via fonction (2019-10-20, 2020-10-20, 5000, 1, 1, 5)'

SELECT create_contrat_agence('2019-10-20', '2020-10-20', 5000, 1, 1, 5);

\echo '\n Faites un SELECT * FROM paiement, contrat ou contrat_agence_artiste Pour voir l\'ajout'
