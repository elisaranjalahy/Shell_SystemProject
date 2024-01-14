# PROJET JSH

Sommaire :
- Structure
- JSH (Main/Fonctionnement)

__________________________________________________________________________________________________________________________________________________________

## **Structure**

### *lib.h*

Ce fichier contient :

-> [color=#26B260]*toutes les bibliotheques utiles*[/color] au bon fonctionnement du code
-> [color=#26B260]*les signatures*[/color] de nos implémentations de **commandes internes**
-> [color=#26B260]*les signatures*[/color] de fonctions utiles aux **redirections**
-> [color=#26B260]*les structures*[/color] utilisées pour la gestion des **jobs** :

-> Un **job** est représenté par une **struct job_node**
-> Une **liste de jobs** est représentée par une **struct job_list**


### *utils.h*

Ce fichier contient :

-> [color=#26B260]*des fonctions utilitaires/auxiliaires*[/color]
-> [color=#26B260]*les fonctions[/color] utiles à la manipulations de **jobs**
-> la commande ***exit***
-> la fonctions de gestions de [color=#26B260]*signaux*[/color]
-> les commandes [color=#26B260]*background* (***bg***)[/color] et [color=#26B260]*foreground* (***fg***) [/color)]


### *redirect.c*

Ce fichier contient :

-> [color=#26B260]*les fonctions*[/color] utiles aux **redirections** et ses **options**


### *my_cd.c*

Ce fichier contient :

-> la commande [color=#26B260]*my_cd* (***cd***)[/color]

### *kill.c*

Ce fichier contient :

-> la commande [color=#26B260]*my_kill* (***kill***)[/color]

### *ext_cmd.c*

Ce fichier contient :

-> la fonction [color=#26B260]*gérant les ***commandes externes*** reçu par notre shell*[/color]

__________________________________________________________________________________________________________________________________________________________

## **JSH**

Le fichier *jsh.c* contient le **main** et deux fonctions indispensables au bon fonctionnement de notre projet:

-> ***parse*** : elle [color=#26B260]*identifie les commandes reçues*[/color] (internes ou externes) par le shell et [color=#26B260]agit en conséquences[/color]
-> ***mkprompt*** : elle [color=#26B260]*génère notre prompt*[/color] de façon esthétique. Elle indique [color=#26B260]le nombre de jobs en cours *entre crochets*[/color] et [color=#26B260]le chemin actuel[/color]. 


Le **main** :

1) initialise une liste de jobs ***jobs*** qui sera commune et transmise pour diverse manipulations entre les fonction jusqu'à la fin d'execution du main.

2) initalise une variable ***last_cmd_success*** qui stocke la valeur de retour de la derniere commande (1 si erreur, 0 si tout s'est bien déroulée)

3) puis lance la boucle lisant l'entrée utilisateur :

    -> chaque chaine de commande entrée est stocké dans un "historique", ce qui permet de récuperé les commandes précédente par les flèches

    -> si la commande est lancée en arrière plan : 
        - le symbole ***"&"*** est retiré puis la fonction **parse** est appelé avec le champ bg à 1 (=true)
        - sinon bg est à 0 (=false)


Le **parser** :

La fonction "parse" reçoit l'entrée utilisateur (dans un ***char***) et [color=#26B260] identifie [/color] la commande demandée.

- Les [color=#26B260]commandes internes[/color] sont ***explicitement*** indiquées par des commentaires. Si le parser en reçoit une, elle appelle la fonctions associée.

- Si la commande reçut est une [color=#26B260]commande externe[/color], le parser va dans le ***else*** et appelle la fonction **execute_ext_cmd**

