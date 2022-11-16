DROP DATABASE IF EXISTS bd_chang_du_m1;
DROP ROLE IF EXISTS bd_chang_du_m1;
CREATE ROLE bd_chang_du_m1 LOGIN PASSWORD '1234';
SET ROLE postgres;
ALTER USER bd_chang_du_m1 WITH SUPERUSER;
SET ROLE bd_chang_du_m1;
CREATE DATABASE bd_chang_du_m1;
\c bd_chang_du_m1;
SET ROLE bd_chang_du_m1;

drop table if exists Personne cascade;
drop table if exists Artiste cascade;
drop table if exists Agent cascade;
drop table if exists Producteur cascade;
drop table if exists Metier cascade;
drop table if exists Agence cascade;
drop table if exists Formation cascade;
drop table if exists Langue cascade;
drop table if exists Instrument cascade;
drop table if exists StyleDeMusique cascade;
drop table if exists Realisation cascade;
drop table if exists Critique cascade;
drop table if exists Demande cascade;
drop table if exists Contrat cascade;
drop table if exists Contrat_agence_artiste cascade;
drop table if exists Contrat_artiste_producteur cascade;
drop table if exists Paiement cascade;

drop table if exists A_fait cascade;
drop table if exists Parle cascade;
drop table if exists Maitrise cascade;
drop table if exists Joue cascade;
drop table if exists A_participe cascade;
drop table if exists Auditionne cascade;
drop table if exists Exerce cascade;
drop table if exists Connait cascade;
drop table if exists Represente cascade;

CREATE TABLE Personne (
    id_personne serial NOT NULL,
    nom_personne varchar(50) NOT NULL,
    CHECK (nom_personne <> 'NULL'),
    prenom_personne varchar(50) NOT NULL,
    date_naissance date NOT NULL,
    PRIMARY KEY (id_personne)
);

CREATE TABLE Artiste (
    id_artiste serial NOT NULL,
    id_personne integer NOT NULL,
    PRIMARY KEY (id_artiste),
    UNIQUE (id_personne),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne)
);

CREATE TABLE Agence (
    id_agence serial NOT NULL,
    nom_agence text NOT NULL,
    CHECK (nom_agence <> 'NULL'),
    PRIMARY KEY (id_agence)
);

CREATE TABLE Agent (
    id_agent serial NOT NULL,
    id_personne integer NOT NULL,
    id_agence integer NOT NULL,
    UNIQUE (id_personne, id_agence),
    PRIMARY KEY (id_agent),
    FOREIGN KEY (id_agence) REFERENCES Agence (id_agence),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne)
);

CREATE TABLE Producteur (
    id_producteur serial NOT NULL,
    id_personne integer NOT NULL,
    UNIQUE (id_personne),
    PRIMARY KEY (id_producteur),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne)
);

CREATE TABLE Metier (
    id_metier serial NOT NULL,
    nom_metier varchar(50) NOT NULL,
    CHECK (nom_metier <> 'NULL'),
    PRIMARY KEY (id_metier)
);

CREATE TABLE Formation (
    nom_formation varchar(50) NOT NULL,
    CHECK (nom_formation <> 'NULL'),
    PRIMARY KEY (nom_formation)
);

CREATE TABLE Langue (
    nom_langue varchar(50) NOT NULL,
    CHECK (nom_langue <> 'NULL'),
    PRIMARY KEY (nom_langue)
);

CREATE TABLE Instrument (
    nom_instrument varchar(50) NOT NULL,
    CHECK (nom_instrument <> 'NULL'),
    PRIMARY KEY (nom_instrument)
);

CREATE TABLE StyleDeMusique (
    nom_styleDeMusique varchar(50) NOT NULL,
    CHECK (nom_styleDeMusique <> 'NULL'),
    PRIMARY KEY (nom_styleDeMusique)
);

CREATE TABLE Realisation (
    id_realisation serial NOT NULL,
    nom_realisation varchar(50) NOT NULL,
    date_realisation date NOT NULL,
    nbSpectateurs INTEGER DEFAULT 0,
    PRIMARY KEY (id_realisation)
);

CREATE TABLE Critique (
    id_critique serial NOT NULL,
    id_realisation integer NOT NULL,
    commentaire varchar(50) NOT NULL,
    distinction varchar(50), /**/
    PRIMARY KEY (id_critique, id_realisation),
    FOREIGN KEY (id_realisation) REFERENCES Realisation (id_realisation)
);

CREATE TABLE Demande (
    id_demande serial NOT NULL,
    id_producteur integer NOT NULL,
    profil_recherche text NOT NULL,
    projet varchar(50) NOT NULL,
    date_debut_demande date NOT NULL,
    date_fin_demande date NOT NULL,
    CHECK (date_debut_demande < date_fin_demande),
    PRIMARY KEY (id_demande),
    FOREIGN KEY (id_producteur) REFERENCES Producteur (id_producteur)
);

CREATE TABLE Contrat (
    id_contrat serial NOT NULL,
    date_debut_contrat date NOT NULL,
    date_fin_contrat date, /* NULL indeterminé */
    CHECK (date_debut_contrat < date_fin_contrat),
    remuneration decimal(12, 2) NOT NULL,
    PRIMARY KEY (id_contrat)
);

CREATE TABLE Contrat_agence_artiste (
    id_contrat_ag_art serial NOT NULL,
    id_contrat integer NOT NULL,
    id_agence integer NOT NULL,
    id_artiste integer NOT NULL,
    pourcent_art_ag integer NOT NULL,
    UNIQUE (id_contrat),
    PRIMARY KEY (id_contrat_ag_art, id_artiste, id_agence),
    FOREIGN KEY (id_contrat) REFERENCES Contrat (id_contrat),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste),
    FOREIGN KEY (id_agence) REFERENCES Agence (id_agence)
);

CREATE TABLE Contrat_artiste_producteur (
    id_contrat_art_prod serial NOT NULL,
    id_contrat integer NOT NULL,
    id_producteur integer NOT NULL,
    id_artiste integer NOT NULL,
    pourcent_art_prod integer NOT NULL,
    nombreVersements integer NOT NULL DEFAULT 1,
    UNIQUE (id_contrat),
    PRIMARY KEY (id_contrat_art_prod, id_artiste, id_producteur),
    FOREIGN KEY (id_contrat) REFERENCES Contrat (id_contrat),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste),
    FOREIGN KEY (id_producteur) REFERENCES Producteur (id_producteur)
);

CREATE TABLE Paiement (
    id_paiement serial NOT NULL,
    id_contrat integer NOT NULL,
    type_paiement varchar(50), /*(frais, commissions, honoraires)*/
    date_prevu date NOT NULL,
    date_effectue date, /* NULL non payé */
    CHECK (date_prevu <= date_effectue),
    montant decimal(12, 2),
    PRIMARY KEY (id_paiement, id_contrat),
    FOREIGN KEY (id_contrat) REFERENCES Contrat (id_contrat)
);

/* RELATION TABLES */

CREATE TABLE A_fait (
    nom_formation varchar(50) NOT NULL,
    id_personne integer NOT NULL,
    PRIMARY KEY (id_personne, nom_formation),
    FOREIGN KEY (nom_formation) REFERENCES Formation (nom_formation),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne)
);

CREATE TABLE Parle (
    nom_langue varchar(50) NOT NULL,
    id_personne integer NOT NULL,
    PRIMARY KEY (id_personne, nom_langue),
    FOREIGN KEY (nom_langue) REFERENCES Langue (nom_langue),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne)
);

CREATE TABLE Maitrise (
    id_artiste integer NOT NULL,
    nom_instrument varchar(50) NOT NULL,
    PRIMARY KEY (id_artiste, nom_instrument),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste),
    FOREIGN KEY (nom_instrument) REFERENCES Instrument (nom_instrument)
);

CREATE TABLE Joue (
    id_artiste integer NOT NULL,
    nom_styleDeMusique varchar(50) NOT NULL,
    CHECK (nom_styleDeMusique <> 'NULL'),
    PRIMARY KEY (id_artiste, nom_styleDeMusique),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste) ON DELETE CASCADE,
    FOREIGN KEY (nom_styleDeMusique) REFERENCES StyleDeMusique (nom_styleDeMusique) ON DELETE CASCADE
);

CREATE TABLE A_participe (
    id_artiste integer NOT NULL,
    id_realisation integer NOT NULL,
    PRIMARY KEY (id_artiste, id_realisation),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste) ON DELETE CASCADE,
    FOREIGN KEY (id_realisation) REFERENCES Realisation (id_realisation) ON DELETE CASCADE
);

CREATE TABLE Auditionne (
    id_artiste integer NOT NULL,
    id_demande integer NOT NULL,
    PRIMARY KEY (id_artiste, id_demande),
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste) ON DELETE CASCADE,
    FOREIGN KEY (id_demande) REFERENCES Demande (id_demande) ON DELETE CASCADE
);

CREATE TABLE Exerce (
    id_metier integer NOT NULL,
    id_personne integer NOT NULL,
    PRIMARY KEY (id_personne, id_metier),
    FOREIGN KEY (id_personne) REFERENCES Personne (id_personne) ON DELETE CASCADE,
    FOREIGN KEY (id_metier) REFERENCES Metier (id_metier) ON DELETE CASCADE
);

CREATE TABLE Connait (
    id_personne1 integer NOT NULL,
    id_personne2 integer NOT NULL,
    PRIMARY KEY (id_personne1, id_personne2),
    FOREIGN KEY (id_personne1) REFERENCES Personne (id_personne) ON DELETE CASCADE,
    FOREIGN KEY (id_personne2) REFERENCES Personne (id_personne) ON DELETE CASCADE
);

CREATE TABLE Represente (
    id_represente serial NOT NULL,
    id_artiste integer NOT NULL,
    id_agent integer NOT NULL,
    date_debut_represente date NOT NULL,
    date_fin_represente date NOT NULL,
    CHECK (date_debut_represente < date_fin_represente),
    PRIMARY KEY (id_represente),
    FOREIGN KEY (id_agent) REFERENCES Agent (id_agent) ON DELETE CASCADE,
    FOREIGN KEY (id_artiste) REFERENCES Artiste (id_artiste) ON DELETE CASCADE
);

CREATE INDEX idx_personne
ON Personne USING HASH(id_personne);

CREATE INDEX idx_artiste
ON Artiste USING HASH(id_artiste);

CREATE INDEX idx_agent
ON Agent USING HASH(id_agent);

CREATE INDEX idx_prodcuteur
ON Producteur USING HASH(id_producteur);

CREATE INDEX idx_represente
ON Represente(date_debut_represente, date_fin_represente); -- btree

/*CREATE INDEX idx_contrat_agence_artiste
ON Contrat_agence_artiste(id_contrat_ag_art);

CREATE INDEX idx_contrat_artiste_producteur
ON Contrat_artiste_producteur(id_contrat_art_prod);*/
