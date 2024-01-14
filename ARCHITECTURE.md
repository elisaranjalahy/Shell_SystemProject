# PROJET JSH

Sommaire :
- Structure
- JSH (Main/Fonctionnement)

__________________________________________________________________________________________________________________________________________________________

## **Structure**

### *lib.h*

Ce fichier contient :

-> **toutes les bibliotheques utiles** au bon fonctionnement du code  
-> **les signatures** de nos implémentations de **commandes internes**  
-> **les signatures** de fonctions utiles aux **redirections**  
-> **les structures** utilisées pour la gestion des **jobs** :

-> Un **job** est représenté par une **struct job_node**
-> Une **liste de jobs** est représentée par une liste chainée **struct job_list**


### *utils.c*

Ce fichier contient :

-> **des fonctions utilitaires/auxiliaires**  
-> **les fonctions** utiles à la manipulations de **jobs**  
-> la commande ***exit***  
-> la fonction de gestions de **signaux**  
-> les commandes **background** (***bg***) et **foreground** (***fg***)  


### *redirect.c*

Ce fichier contient :

-> **les fonctions** utiles aux **redirections**


### *my_cd.c*

Ce fichier contient :

-> la commande **my_cd** (***cd***)

### *kill.c*

Ce fichier contient :

-> la commande **my_kill** (***kill***)

### *ext_cmd.c*

Ce fichier contient :

-> la fonction **gérant les ***commandes externes*** reçu par notre shell**

__________________________________________________________________________________________________________________________________________________________

## **JSH**

Le fichier *jsh.c* contient le **main** et deux fonctions indispensables au bon fonctionnement de notre projet:

-> ***parse*** : elle **identifie les commandes reçues** (internes ou externes) par le shell et **agit en conséquences**
-> ***mkprompt*** : elle **génère notre prompt** de façon esthétique. Elle indique **le nombre de jobs en cours *entre crochets** et **le chemin actuel**


Le **main** :

1) initialise une liste de jobs ***jobs*** qui sera commune et transmise pour diverses manipulations entre les fonctions jusqu'à la fin d'exécution du main.

2) initalise une variable ***last_cmd_success*** qui stocke la valeur de retour de la derniere commande.

3) puis lance la boucle lisant l'entrée utilisateur :

    -> chaque chaine de commande entrée est stockée dans un "historique", ce qui permet de récuperé les commandes précédentes par les flèches

    -> si la commande est lancée en arrière plan : 
        - le symbole ***"&"*** est retiré puis la fonction **parse** est appelée avec le champ bg à 1 (=true)
        - sinon bg est à 0 (=false)


Le **parser** :

La fonction "parse" reçoit l'entrée utilisateur (dans un ***char*****) et **identifie** la commande demandée.

- Les **commandes internes** sont ***explicitement*** indiquées par des commentaires. Si le parser en reçoit une, elle appelle la fonctions associée.

- Si la commande reçut est une **commande externe**, le parser va dans le ***else*** et appelle la fonction **execute_ext_cmd**.

