-- Drop Triggers
DROP TRIGGER IF EXISTS trig_payment ON paiement;
DROP TRIGGER IF EXISTS trig_represente ON Represente;
DROP TRIGGER IF EXISTS trig_contrat_ag ON contrat_agence_artiste;
DROP TRIGGER IF EXISTS trig_contrat_prod ON contrat_artiste_producteur;
DROP TRIGGER IF EXISTS trig_a_fait ON a_fait;
DROP TRIGGER IF EXISTS trig_parle ON parle;
DROP TRIGGER IF EXISTS trig_maitrise ON maitrise;
DROP TRIGGER IF EXISTS trig_joue ON joue;


/* * * * * * * * * * * * * * * * FUNCTIONS FOR TRIGGERS * * * * * * * * * * * * * * * */


-- Function checking if the payment is acceptable
CREATE OR REPLACE FUNCTION payment()
RETURNS TRIGGER AS $$
    DECLARE
        cur_montant numeric(12,2);
        max_montant numeric(12,2);
    BEGIN
        -- Get the current payments done for a contract, Default 0
        cur_montant := (SELECT COALESCE(SUM(montant), 0) FROM paiement
            WHERE id_contrat = NEW.id_contrat AND id_paiement != NEW.id_paiement);

        -- Get the expected payment from the contract
        max_montant := (SELECT remuneration FROM contrat WHERE id_contrat = NEW.id_contrat);

        -- Check if the payment is conceivable
        IF (cur_montant >= max_montant) THEN
            RAISE NOTICE 'Paiement du contrat fini';
            RETURN NULL;
        ELSIF (NEW.montant <= 0) THEN
            RAISE NOTICE 'Montant négatif = %', NEW.montant;
            RETURN NULL;
        ELSIF ((cur_montant + NEW.montant) > max_montant) THEN
            RAISE NOTICE 'Montant trop élevé = %', NEW.montant;
            RETURN NULL;
        END IF;

        CASE
        WHEN TG_OP = 'INSERT' THEN
            RETURN NEW;

        WHEN TG_OP = 'UPDATE' THEN
            -- We don't UPDATE an already paid payment
            IF OLD.date_effectue IS NOT NULL THEN
                RAISE NOTICE 'Paiement déjà effectué, modification refusée';
                RETURN NULL;
            END IF;

        WHEN TG_OP = 'DELETE' THEN
        -- We don't DELETE an already paid payment
            IF (OLD.date_effectue IS NOT NULL) THEN
                RAISE NOTICE 'Paiement déjà effectué, suppression refusée';
                RETURN NULL;
            ELSE
                RETURN OLD;
            END IF;

        END CASE;
        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Function checking if date overlap in Represente
CREATE OR REPLACE FUNCTION overlap_represente()
RETURNS TRIGGER AS $$
    DECLARE
        passed INTEGER;
    BEGIN
        -- Start1 <= End2 AND Start2 <= End1
        passed :=
            (SELECT COUNT(*)
            FROM Represente
            WHERE id_artiste = NEW.id_artiste AND id_represente != NEW.id_represente
            AND date_debut_represente <= NEW.date_fin_represente
            AND NEW.date_debut_represente <= date_fin_represente);
        IF (passed != 0) THEN
            RAISE NOTICE 'Representation avec des dates qui se chevauchent';
            RETURN NULL;
        END IF;
        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Automatically creates one 'paiement' for a 'contrat_agence_artiste'
CREATE OR REPLACE FUNCTION contrat_ag_paiement()
RETURNS TRIGGER AS $$
    DECLARE
        remun decimal(12,2);    -- total amount
        date_deb DATE;          -- starting date contrat
        date_pay DATE;          -- payment date
    BEGIN
        remun := (SELECT remuneration FROM contrat WHERE id_contrat = NEW.id_contrat);
        date_deb := (SELECT date_debut_contrat FROM contrat WHERE id_contrat = NEW.id_contrat);
        date_pay := (SELECT add_months(date_deb, 1));

        -- Insert a new 'paiement' one month from date_deb
        INSERT INTO paiement (id_contrat, type_paiement, date_prevu, date_effectue, montant)
        VALUES (NEW.id_contrat, 'honoraires', date_pay, null, remun);

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Automatically creates 'nbVersements' paiements for a 'contrat_artiste_producteur'
CREATE OR REPLACE FUNCTION contrat_prod_paiement()
RETURNS TRIGGER AS $$
    DECLARE
        remun decimal(12,2);    -- total amount
        date_deb DATE;          -- starting date contrat
        date_pay DATE;          -- payment date
        sub_pay decimal(12,2);  -- sub payments = remun / nbVersements
    BEGIN
        remun := (SELECT remuneration FROM contrat WHERE id_contrat = NEW.id_contrat);
        date_deb := (SELECT date_debut_contrat FROM contrat WHERE id_contrat = NEW.id_contrat);
        sub_pay := (remun / NEW.nombreVersements); -- 'montant' in each 'paiement'

        FOR i IN 1..NEW.nombreVersements
        LOOP
            date_pay := (SELECT add_months(date_deb, i)); -- Next payment date

            -- Insert a new 'paiement' every month after date_deb
            INSERT INTO paiement (id_contrat, type_paiement, date_prevu, date_effectue, montant)
            VALUES (NEW.id_contrat, 'honoraires', date_pay, null, sub_pay);
        END LOOP;

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Adds a new Formation if this does not exist
CREATE OR REPLACE FUNCTION a_fait()
RETURNS TRIGGER AS $$
    BEGIN
        -- Check if the Formation exists
        PERFORM *
        FROM formation
        WHERE nom_formation = NEW.nom_formation;

        -- Add the new Formation
        IF NOT FOUND THEN
            INSERT INTO formation VALUES (NEW.nom_formation);
        END IF;

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Adds a new Langue if this does not exist
CREATE OR REPLACE FUNCTION parle()
RETURNS TRIGGER AS $$
    BEGIN
        -- Check if the Langue exists
        PERFORM *
        FROM langue
        WHERE nom_langue = NEW.nom_langue;

        -- Add the new Langue
        IF NOT FOUND THEN
            INSERT INTO langue VALUES (NEW.nom_langue);
        END IF;

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Adds a new Instrument if this does not exist
CREATE OR REPLACE FUNCTION maitrise()
RETURNS TRIGGER AS $$
    BEGIN
        -- Check if the Instrument exists
        PERFORM *
        FROM instrument
        WHERE nom_instrument = NEW.nom_instrument;

        -- Add the new Instrument
        IF NOT FOUND THEN
            INSERT INTO instrument VALUES (NEW.nom_instrument);
        END IF;

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;

-- Adds a new StyleDeMusique if this does not exist
CREATE OR REPLACE FUNCTION joue()
RETURNS TRIGGER AS $$
    BEGIN
        -- Check if the StyleDeMusique exists
        PERFORM *
        FROM styledemusique
        WHERE nom_styleDeMusique = NEW.nom_styleDeMusique;

        -- Add the new StyleDeMusique
        IF NOT FOUND THEN
            INSERT INTO styledemusique VALUES (NEW.nom_styleDeMusique);
        END IF;

        RETURN NEW;
    END;
$$ LANGUAGE plpgsql;


/* * * * * * * * * * * * * * * * TRIGGERS * * * * * * * * * * * * * * * */


-- Check the query is acceptable for a payment
CREATE TRIGGER trig_payment
    BEFORE UPDATE OR INSERT OR DELETE
    ON paiement
    FOR EACH ROW
    EXECUTE PROCEDURE payment();

-- Check the query is acceptable for a representation
CREATE TRIGGER trig_represente
    BEFORE INSERT OR UPDATE
    ON Represente
    FOR EACH ROW
    EXECUTE PROCEDURE overlap_represente();

-- Creates one pending paiement for the contrat
CREATE TRIGGER trig_contrat_ag
    AFTER INSERT
    ON contrat_agence_artiste
    FOR EACH ROW
    EXECUTE PROCEDURE contrat_ag_paiement();

-- Creates nbVersements pending paiement for the contrat
CREATE TRIGGER trig_contrat_prod
    AFTER INSERT
    ON contrat_artiste_producteur
    FOR EACH ROW
    EXECUTE PROCEDURE contrat_prod_paiement();

-- Adds a new Formation when the inserted name does not exist
CREATE TRIGGER trig_a_fait
    BEFORE INSERT
    ON a_fait
    FOR EACH ROW
    EXECUTE PROCEDURE a_fait();

-- Adds a new Langue when the inserted name does not exist
CREATE TRIGGER trig_parle
    BEFORE INSERT
    ON parle
    FOR EACH ROW
    EXECUTE PROCEDURE parle();

-- Adds a new Instrument when the inserted name does not exist
CREATE TRIGGER trig_maitrise
    BEFORE INSERT
    ON maitrise
    FOR EACH ROW
    EXECUTE PROCEDURE maitrise();

-- Adds a new StyleDeMusique when the inserted name does not exist
CREATE TRIGGER trig_joue
    BEFORE INSERT
    ON joue
    FOR EACH ROW
    EXECUTE PROCEDURE joue();
