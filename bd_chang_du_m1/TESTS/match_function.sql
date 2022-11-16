\echo '\nObtenir les personnes parlant Bengali LIMIT 5'

SELECT * FROM best_match('Bengali') LIMIT 5;

\echo '\nObtenir les personnes ayant un master LIMIT 5'

SELECT * FROM best_match('NULL, master, NULL') LIMIT 5;

\echo '\nObtenir les personnes ayant un master et/ou jouant du rock LIMIT 5'

SELECT * FROM best_match('NULL, master, rock') LIMIT 5;

\echo '\nObtenir les personnes ayant le milleur match avec les criteres: parlant Ndonda ayant une licence et jouant du rock LIMIT 5'

SELECT * FROM best_match('Ndonga, licence, rock') LIMIT 5;

\echo '\nObtenir les personnes jouant de la Guitare et/ou du Piano LIMIT 5'

SELECT * FROM best_match('Bengali, NULL, NULL, Piano Guitare') LIMIT 5;

\echo '\nObtenir les personnes jouant de la Guitare et/ou du Piano ou parlant Bengali/French ou ayant un master ou une licence et jouant du folk LIMIT 5'

SELECT * FROM best_match('Bengali French, master licence, folk, Guitare Piano') LIMIT 5;