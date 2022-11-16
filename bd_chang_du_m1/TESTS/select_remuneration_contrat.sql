\echo '\nRappel: Un Artiste est taxé un pourcentage par contrat_agence, et gagne un pourcentage bonus par contrat_producteur'


\echo '\nRémunération net et brut de l\'artiste sur chaque contrat_agence :'

SELECT c.id_contrat, cag.id_artiste, cag.id_contrat_ag_art, cag.pourcent_art_ag,
c.remuneration remun_net, remuneration_contrat(c.id_contrat) remun_net
FROM contrat c, contrat_agence_artiste cag
WHERE c.id_contrat = cag.id_contrat;


\echo '\nRémunération net et brut de l\'artiste sur chaque contrat_producteur :'

SELECT c.id_contrat, cpd.id_artiste, cpd.id_contrat_art_prod, cpd.pourcent_art_prod,
c.remuneration remun_brut, remuneration_contrat(c.id_contrat) remun_net
FROM contrat c, contrat_artiste_producteur cpd
WHERE c.id_contrat = cpd.id_contrat;
