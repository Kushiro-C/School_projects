-- Function create new Artiste + Personne
CREATE OR REPLACE FUNCTION create_artiste(nom varchar(50), prenom varchar(50), naissance date)
RETURNS VOID AS $$
    DECLARE
        id_pers INTEGER;
    BEGIN
        id_pers := COALESCE((SELECT MAX(id_personne) FROM Personne), 0);
        IF (nom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (prenom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (naissance is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (id_pers is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        END IF;

        INSERT INTO Personne(nom_personne,prenom_personne,date_naissance)
        VALUES (nom, prenom, naissance);

        INSERT INTO Artiste(id_personne)
        VALUES (id_pers +1);
    END;
$$ LANGUAGE plpgsql;

-- Function create new Agent + Personne
CREATE OR REPLACE FUNCTION create_agent(nom varchar(50), prenom varchar(50), naissance date, id_agence INTEGER)
RETURNS VOID AS $$
    DECLARE
        id_pers INTEGER;
    BEGIN
        id_pers := COALESCE((SELECT MAX(id_personne) FROM Personne), 0);
        IF (nom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (prenom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (naissance is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (id_pers is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (id_agence is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        END IF;

        INSERT INTO Personne(nom_personne,prenom_personne,date_naissance)
        VALUES (nom, prenom, naissance);

        INSERT INTO Agent(id_personne, id_agence)
        VALUES (id_pers +1, id_agence);
    END;
$$ LANGUAGE plpgsql;

-- Function create new Prodcteur + Personne
CREATE OR REPLACE FUNCTION create_producteur(nom varchar(50), prenom varchar(50), naissance date)
RETURNS VOID AS $$
    DECLARE
        id_pers INTEGER;
    BEGIN
        id_pers := COALESCE((SELECT MAX(id_personne) FROM Personne), 0);
        IF (nom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (prenom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (naissance is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (id_pers is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        END IF;

        INSERT INTO Personne(nom_personne,prenom_personne,date_naissance)
        VALUES (nom, prenom, naissance);

        INSERT INTO Producteur(id_personne)
        VALUES (id_pers +1);
    END;
$$ LANGUAGE plpgsql;

-- Function create new Prodcteur + Artiste + Personne
CREATE OR REPLACE FUNCTION create_producteur_artiste(nom varchar(50), prenom varchar(50), naissance date)
RETURNS VOID AS $$
    DECLARE
        id_pers INTEGER;
    BEGIN
        id_pers := COALESCE((SELECT MAX(id_personne) FROM Personne), 0);
        IF (nom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (prenom is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (naissance is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        ELSIF (id_pers is NULL) THEN
            RAISE EXCEPTION 'valeur nulle detectée';
            RETURN;
        END IF;

        INSERT INTO Personne(nom_personne,prenom_personne,date_naissance)
        VALUES (nom, prenom, naissance);

        INSERT INTO Producteur(id_personne)
        VALUES (id_pers +1);

        INSERT INTO Artiste(id_personne)
        VALUES (id_pers +1);
    END;
$$ LANGUAGE plpgsql;

-- return all id_artiste that have a match with possible duplicates values
-- profil_recherche 'langue, nom_formation, nom_styleDeMusique, nom_instrument'
-- example : SELECT * FROM match_demande('Bengali French, master licence, folk, Guitare Piano');
CREATE OR REPLACE FUNCTION match_demande(profil_recherche text)
RETURNS SETOF INTEGER AS $$
    DECLARE
        artistes RECORD;
        param TEXT;
        nom TEXT;
    BEGIN
        param := (SELECT split_part(profil_recherche, ', ', 1));
        IF (param IS NOT NULL AND param != ' NULL') THEN
            FOR nom IN SELECT unnest(string_to_array(param, ' ')) AS noms
            LOOP
                IF (nom IS NOT NULL) THEN
                    FOR artistes IN
                    SELECT id_artiste
                    FROM (Artiste NATURAL JOIN Parle NATURAL JOIN Langue) as mytable
                    WHERE mytable.nom_langue LIKE nom
                    LOOP
                        RETURN NEXT artistes.id_artiste;
                    END LOOP;
                END IF;
            END LOOP;
        END IF;

        param := (SELECT split_part(profil_recherche, ', ', 2));
        IF (param IS NOT NULL AND param != ' NULL') THEN
            FOR nom IN SELECT unnest(string_to_array(param, ' ')) AS noms
            LOOP
                IF (nom IS NOT NULL) THEN
                    FOR artistes IN
                    SELECT id_artiste
                    FROM Artiste NATURAL JOIN Formation NATURAL JOIN A_fait
                    WHERE nom_formation LIKE nom
                    LOOP
                        RETURN NEXT artistes.id_artiste;
                    END LOOP;
                END IF;
            END LOOP;
        END IF;

        param := (SELECT split_part(profil_recherche, ', ', 3));
        IF (param IS NOT NULL AND param != ' NULL') THEN
            FOR nom IN SELECT unnest(string_to_array(param, ' ')) AS noms
            LOOP
                IF (nom IS NOT NULL) THEN
                    FOR artistes IN
                    SELECT id_artiste
                    FROM Artiste NATURAL JOIN Joue
                    WHERE nom_styleDeMusique LIKE nom
                    LOOP
                        RETURN NEXT artistes.id_artiste;
                    END LOOP;
                END IF;
            END LOOP;
        END IF;

        param := (SELECT split_part(profil_recherche, ', ', 4));
        IF (param IS NOT NULL AND param != ' NULL') THEN
            FOR nom IN SELECT unnest(string_to_array(param, ' ')) AS noms
            LOOP
                IF (nom IS NOT NULL) THEN
                    FOR artistes IN
                    SELECT id_artiste
                    FROM Artiste NATURAL JOIN Maitrise NATURAL JOIN Instrument
                    WHERE nom_instrument LIKE nom
                    LOOP
                        RETURN NEXT artistes.id_artiste;
                    END LOOP;
                END IF;
            END LOOP;
        END IF;
    END;
$$ LANGUAGE plpgsql;

-- table matchs compared with profils that we are looking for
-- profil_recherche 'langue, nom_formation, nom_styleDeMusique, nom_instrument' best match
-- example : SELECT * FROM best_match('Bengali French, master licence, folk, Guitare Piano');
CREATE OR REPLACE FUNCTION best_match(profil_recherche text)
RETURNS TABLE(id_artiste INTEGER, nbMatch BIGINT) AS $$
    BEGIN
        RETURN QUERY SELECT match_demande AS id_artiste,
            COUNT(*) AS nbMatch
            FROM match_demande(profil_recherche)
            GROUP BY match_demande ORDER BY nbMatch DESC;
    END;
$$ LANGUAGE plpgsql;

-- Adds 'n' months to a date 'd'
CREATE OR REPLACE FUNCTION add_months(d DATE, n INT)
RETURNS DATE AS $$
    BEGIN
      RETURN (SELECT (d + (n || 'months')::INTERVAL)::DATE);
    END;
$$ LANGUAGE plpgsql;

-- Create a contrat Artiste-Agence
CREATE OR REPLACE FUNCTION create_contrat_agence(
    date_debut DATE,
    date_fin DATE,
    montant decimal(12, 2),
    id_ag INT,
    id_art INT,
    pourcentage INT
) RETURNS VOID AS $$
    DECLARE
        id_cont INT;
    BEGIN
        id_cont := 1 + (SELECT COUNT(*) FROM contrat);

        INSERT INTO contrat (date_debut_contrat, date_fin_contrat, remuneration)
        VALUES (date_debut, date_fin, montant);

        INSERT INTO contrat_agence_artiste (id_contrat, id_agence, id_artiste, pourcent_art_ag)
        VALUES (id_cont, id_ag, id_art, pourcentage);
    END;
$$ LANGUAGE plpgsql;


-- Create a contrat Artiste-Producteur
CREATE OR REPLACE FUNCTION create_contrat_prod(
    date_debut DATE,
    date_fin DATE,
    montant decimal(12, 2),
    id_prod INT,
    id_art INT,
    pourcentage INT,
    nbVersements INT
) RETURNS VOID AS $$

    DECLARE
        id_cont INT;
    BEGIN
        id_cont := 1 + (SELECT COUNT(*) FROM contrat);

        INSERT INTO contrat (date_debut_contrat, date_fin_contrat, remuneration)
        VALUES (date_debut, date_fin, montant);

        INSERT INTO contrat_artiste_producteur (id_contrat, id_producteur, id_artiste,
                                                pourcent_art_prod, nombreVersements)
        VALUES (id_cont, id_prod, id_art, pourcentage, nbVersements);
    END;
$$ LANGUAGE plpgsql;

-- Total remuneration perceived by the artist for the contract
-- Agence takes a percentage from the artist's remuneration
CREATE OR REPLACE FUNCTION remuneration_contrat(id_cont INT)
RETURNS DECIMAL(12,2) AS $$
    DECLARE
        remun_c_ag DECIMAL(12, 2);      -- Remuneration contrat agence
        remun_c_prod DECIMAL(12, 2);    -- Remuneration contrat producteur
    BEGIN
        remun_c_ag := COALESCE ((
            SELECT (c.remuneration * (1.0 - cag.pourcent_art_ag / 100.0))
            FROM contrat c, contrat_agence_artiste cag
            WHERE cag.id_contrat = id_cont AND c.id_contrat = cag.id_contrat
        ), 0);
        remun_c_prod := COALESCE ((
            SELECT (c.remuneration * (1.0 + cpd.pourcent_art_prod / 100.0))
            FROM contrat c, contrat_artiste_producteur cpd
            WHERE cpd.id_contrat = id_cont AND c.id_contrat = cpd.id_contrat
        ), 0);

        RETURN remun_c_ag + remun_c_prod;
    END;
$$ LANGUAGE plpgsql;

-- Artist total remuneration perceived
-- Artist gains a bonus percentage from the remuneration
CREATE OR REPLACE FUNCTION remuneration_artiste(id_art INT)
RETURNS DECIMAL(12,2) AS $$
    DECLARE
        remun_c_ag DECIMAL(12, 2);      -- Remuneration contrat agence
        remun_c_prod DECIMAL(12, 2);    -- Remuneration contrat producteur
    BEGIN
        remun_c_ag := COALESCE ((
            SELECT SUM(c.remuneration * (1.0 - cag.pourcent_art_ag / 100.0))
            FROM contrat c, contrat_agence_artiste cag
            WHERE c.id_contrat = cag.id_contrat AND cag.id_artiste = id_art
        ), 0);
        remun_c_prod := COALESCE ((
            SELECT SUM(c.remuneration * (1.0 + cpd.pourcent_art_prod / 100.0))
            FROM contrat c, contrat_artiste_producteur cpd
            WHERE c.id_contrat = cpd.id_contrat AND cpd.id_artiste = id_art
        ), 0);

        RETURN remun_c_ag + remun_c_prod;
    END;
$$ LANGUAGE plpgsql;
